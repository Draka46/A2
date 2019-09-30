#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"
#include "wires.h"
#include "arithmetic.h"
#include "memory.h"
#include "registers.h"
#include "ip_reg.h"
#include "compute.h"

// major opcodes (1st 4 bits of instruction encoding (see A2.pdf section A "encoding.txt"))
// Specifies right-side details of operation to be performed (ex. movq ...,d), special cases excluded (ex. reversed for movq d,(s))
#define RETURN         0x0
#define REG_ARITHMETIC 0x1
#define REG_MOVQ       0x2
#define REG_MOVQ_MEM   0x3
#define CFLOW          0x4
#define IMM_ARITHMETIC 0x5
#define IMM_MOVQ       0x6
#define IMM_MOVQ_MEM   0x7
#define LEAQ2          0x8
#define LEAQ3          0x9
#define LEAQ6          0xA
#define LEAQ7          0xB
#define IMM_CBRANCH    0xF

// minor opcodes (2nd 4 bits of instruction encoding)
// Specifies left-side details of operation to be performed (ex. movq movq s,...)
#define COPY_MOVQ_REG     0x1
#define IMM_MOVQ_REG      0x4
#define IMM_MEM_MOVQ_REG  0x5
#define MOVQ_MEM_REG      0x9
#define MOVQ_MEM_IMM      0xd

#define JMP 0xF
#define CALL 0xE

// major opcode registers (3rd 4 bits of instruction encoding). Specifies right-side register address
// minor opcode registers (4th 4 bits of instruction encoding). Specifies left-side register address

int main(int argc, char* argv[]) {
    // Check command line parameters.
    if (argc < 2)
        error("missing name of programfile to simulate");

    if (argc < 3)
        error("Missing starting address (in hex notation)");

    /*** SETUP ***/
    // We set up global state through variables that are preserved between cycles.

    // Program counter / Instruction Pointer
    ip_reg_p ip = ip_reg_create();

    // Register file:
    reg_p regs = regs_create();

    // Memory:
    // Shared memory for both instructions and data.
    mem_p mem = memory_create();
    memory_read_from_file(mem, argv[1]);

    int start;
    int scan_res = sscanf(argv[2],"%x", &start);
    if (scan_res != 1)
        error("Unable to interpret starting address");

    if (argc == 4) { // tracefile specified, hook memories to it
        memory_tracefile(mem, argv[3]);
        regs_tracefile(regs, argv[3]);
        ip_reg_tracefile(ip, argv[3]);
    }
    ip_write(ip, from_int(start), true);

    // a stop signal for stopping the simulation. 
    bool stop = false;

    // We need the instruction number to show how far we get
    int instruction_number = 0;

    while (!stop) { // for each cycle:
        val pc = ip_read(ip);
        ++instruction_number;
        printf("%d %lx\n", instruction_number, pc.val);
        /*** FETCH ***/

        // We're fetching 10 bytes in the form of 10 vals with one byte each
        val inst_bytes[10];
        memory_read_into_buffer(mem, pc, inst_bytes, true);

        /*** DECODE ***/
        // read 4 bit values
        val major_op = pick_bits(4,  4, inst_bytes[0]);
        val minor_op = pick_bits(0,  4, inst_bytes[0]); // <--- essential for further decode, but not used yet

        val reg_d = pick_bits(4, 4, inst_bytes[1]);
        val reg_s = pick_bits(0, 4, inst_bytes[1]);

        // decode instruction type
        // read major operation code
        bool is_return = is(RETURN, major_op);                     // ret s
        bool is_reg_movq = is(REG_MOVQ, major_op);                 // movq s,...
        bool is_imm_movq = is(IMM_MOVQ, major_op);                 // movq $i,...
		bool is_load = is(REG_MOVQ_MEM, major_op);                 // movq (s),...
		bool is_imm_movq_mem = is(IMM_MOVQ_MEM, major_op);         // movq $i(s),...
		
		// read minor operation code
		bool is_copy_movq_reg = is(COPY_MOVQ_REG, minor_op);       // movq d,(s)
		bool is_imm_movq_reg = is(IMM_MOVQ_REG, minor_op);         // movq $i,d
		bool is_imm_mem_movq_reg = is(IMM_MEM_MOVQ_REG, minor_op); // movq i(s),d
		bool is_movq_mem_imm = is(MOVQ_MEM_IMM, minor_op);         // movq d,i(s)
		bool is_store = is(MOVQ_MEM_REG, minor_op);                // movq ...,d

		is_load = (is_load || is_imm_movq_mem) && !(is_store || is_movq_mem_imm);     // fix for is_load = true, whenever is_store is true
		is_imm_movq_mem = is_imm_movq_mem && !is_movq_mem_imm;                        // fix for is_imm_movq_mem = true, whenever is_movq_mem_imm is true
		is_reg_movq = is_reg_movq || (!is_load && !is_imm_movq_mem && !is_imm_movq);  // fix for is_reg_movq = false, whenever is_store is true or is_movq_mem_imm is true

		printf("major_op: %lx\n", major_op.val);
		printf("minor_op: %lx\n", minor_op.val);
		//printf("is_return: %d\n", is_return);
		printf("is_reg_movq: %d\n", is_reg_movq);
		//printf("is_imm_movq: %d\n", is_imm_movq);
		printf("is_load: %d\n", is_load);
		printf("is_imm_movq_mem: %d\n", is_imm_movq_mem);
		//printf("is_imm_movq_reg: %d\n", is_imm_movq_reg);
		//printf("is_store: %d\n", is_store);

        // determine instruction size
        bool size2 = (is_return || is_reg_movq || is_load) && !is_movq_mem_imm;
        bool size6 = is_imm_movq || is_imm_movq_mem || is_movq_mem_imm;

        val ins_size = or(use_if(size2, from_int(2)), use_if(size6, from_int(6)));

        // setting up operand fetch and register read and write for the datapath:
		bool use_imm = is_imm_movq || is_imm_movq_mem || is_movq_mem_imm;
        val reg_read_dz = reg_d;
        val reg_read_sz = reg_s;
        // - other read port is always reg_s
        // - write is always to reg_d
        bool reg_wr_enable = is_copy_movq_reg || is_imm_movq_reg || is_imm_mem_movq_reg;
		bool mem_wr_enable = is_store || is_movq_mem_imm;

        // Datapath:
        //
        // read immediates based on instruction type
        val imm_offset_2 = or(or(put_bits(0, 8, inst_bytes[2]), put_bits(8,8, inst_bytes[3])),
                              or(put_bits(16, 8, inst_bytes[4]), put_bits(24,8, inst_bytes[5])));
        val imm_i = imm_offset_2; // <--- could be more
        val sext_imm_i = sign_extend(31, imm_i);

        /*** EXECUTE ***/
        // read registers
        val reg_out_a = reg_read(regs, reg_read_dz);
        val reg_out_b = reg_read(regs, reg_read_sz);
		val op_b = or(use_if(use_imm, sext_imm_i),use_if(!use_imm, reg_out_b));  // movq $i,.. -> $i | movq s,d -> s | movq i(s),d -> $i | movq d,i(s) -> $i
		
		//printf("sext_imm_i: %lx\n", sext_imm_i.val);
		//printf("op_b: %lx\n", op_b.val);

        // perform calculations
        // not really any calculations yet!
		// generate address for memory access..
        val agen_result = reg_out_a;   // ..right-side instruction argument
        val bgen_result = or(use_if(is_imm_movq_mem || is_movq_mem_imm, add(op_b, reg_out_b)), use_if(!(is_imm_movq_mem || is_movq_mem_imm), reg_out_b));   // ..left-side instruction argument
		
		printf("reg_out_b: %lx\n", reg_out_b.val);
		printf("add(op_b, reg_out_b): %lx\n", add(op_b, reg_out_b).val);
		printf("bgen_result: %lx\n", bgen_result.val);

        // address of succeeding instruction in memory
        val pc_incremented  = add(pc, ins_size);

        // determine the next position of the program counter
        val pc_next = or(use_if(is_return, reg_out_b), use_if(!is_return, pc_incremented));

        /*** MEMORY ***/
        // read from memory if needed
        // Not implemented yet!
        val mem_out = memory_read(mem, bgen_result, is_load);

        /*** WRITE ***/
        // choose result to write back to register
        val datapath_result = or(use_if(is_reg_movq || is_imm_movq, op_b), use_if(is_load, mem_out));
		
		//printf("reg_d: %lx\n", reg_d.val);
		printf("reg_wr_enable: %d\n", reg_wr_enable);
		printf("mem_wr_enable: %d\n", mem_wr_enable);
		printf("datapath_result: %lx\n", datapath_result.val);
		printf("reg_out_a: %lx\n", reg_out_a.val);
		printf("mem_out: %lx\n", mem_out.val);

        // write to register if needed
        reg_write(regs, reg_d, datapath_result, reg_wr_enable);

		//printf("mem_wr_enable: %d\n", mem_wr_enable);

        // write to memory if needed
        // Not implemented yet!
        memory_write(mem, bgen_result, reg_out_a, mem_wr_enable);

        // update program counter
        ip_write(ip, pc_next, true);

        // terminate when returning to zero
        if (pc_next.val == 0 && is_return) stop = true;

		printf("------ \n");
    }
    memory_destroy(mem);
    regs_destroy(regs);

    printf("Done\n");
}
