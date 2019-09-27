#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "support.h"
#include "wires.h"
#include "arithmetic.h"
#include "memory.h"
#include "registers.h"
#include "ip_reg.h"
#include "compute.h"

// major opcodes
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

#define JMP 0xF
#define CALL 0xE

int main(int argc, char* argv[]) {
    // Check command line parameters.
    if (argc < 2)
        error("missing name of programfile to simulate");

    if (argc < 3)
        error("Missing starting address (in hex notation)");

    /*** SETUP ***/
    // We set up global state through variables that are preserved between
    // cycles.

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
        bool is_return = is(RETURN, major_op);
        bool is_reg_movq = is(REG_MOVQ, major_op);
        bool is_imm_movq = is(IMM_MOVQ, major_op);

        // determine instruction size
        bool size2 = is_return || is_reg_movq;
        bool size6 = is_imm_movq;

        val ins_size = or(use_if(size2, from_int(2)), use_if(size6, from_int(6)));

        // setting up operand fetch and register read and write for the datapath:
	bool use_imm = is_imm_movq;
        val reg_read_dz = reg_d;
        // - other read port is always reg_s
        // - write is always to reg_d
        bool reg_wr_enable = is_reg_movq || is_imm_movq;

        // Datapath:
        //
        // read immediates based on instruction type
        val imm_offset_2 = or(or(put_bits(0, 8, inst_bytes[2]), put_bits(8,8, inst_bytes[3])),
                              or(put_bits(16, 8, inst_bytes[4]), put_bits(24,8, inst_bytes[5])));
        val imm_i = imm_offset_2; // <--- could be more
        val sext_imm_i = sign_extend(31, imm_i);

        /*** EXECUTE ***/
        // read registers
        val reg_out_a = reg_read(regs, reg_read_dz); // <-- will generate warning, as we don't use it yet
        val reg_out_b = reg_read(regs, reg_s);
	val op_b = or(use_if(use_imm, sext_imm_i),use_if(!use_imm, reg_out_b));

        // perform calculations
        // not really any calculations yet!
        // val agen_result = ....   // generate address for memory access
        // ....

        // address of succeeding instruction in memory
        val pc_incremented  = add(pc, ins_size);

        // determine the next position of the program counter
        val pc_next = or(use_if(is_return, reg_out_b), use_if(!is_return, pc_incremented));

        /*** MEMORY ***/
        // read from memory if needed
        // Not implemented yet!
        // val mem_out = memory_read(mem, agen_result, is_load);

        /*** WRITE ***/
        // choose result to write back to register
        val datapath_result = op_b;

        // write to register if needed
        reg_write(regs, reg_d, datapath_result, reg_wr_enable);

        // write to memory if needed
        // Not implemented yet!
        // memory_write(mem, agen_result, reg_out_a, is_store);

        // update program counter
        ip_write(ip, pc_next, true);

        // terminate when returning to zero
        if (pc_next.val == 0 && is_return) stop = true;
    }
    memory_destroy(mem);
    regs_destroy(regs);

    printf("Done\n");
}
