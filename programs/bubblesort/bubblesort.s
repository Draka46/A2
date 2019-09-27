	.file	"bubblesort.c"
	.text
	.globl	read_long
	.type	read_long, @function
read_long:
.LFB0:
	.cfi_startproc
	movq	268435456, %rax
	ret
	.cfi_endproc
.LFE0:
	.size	read_long, .-read_long
	.globl	gen_random
	.type	gen_random, @function
gen_random:
.LFB1:
	.cfi_startproc
	movq	268435457, %rax
	ret
	.cfi_endproc
.LFE1:
	.size	gen_random, .-gen_random
	.globl	write_long
	.type	write_long, @function
write_long:
.LFB2:
	.cfi_startproc
	movq	%rdi, 268435458
	ret
	.cfi_endproc
.LFE2:
	.size	write_long, .-write_long
	.globl	init_allocator
	.type	init_allocator, @function
init_allocator:
.LFB3:
	.cfi_startproc
	leaq	allocator_base(%rip), %rax
	movq	%rax, cur_allocator(%rip)
	ret
	.cfi_endproc
.LFE3:
	.size	init_allocator, .-init_allocator
	.globl	allocate
	.type	allocate, @function
allocate:
.LFB4:
	.cfi_startproc
	movq	cur_allocator(%rip), %rax
	leaq	(%rax,%rdi,8), %rdx
	movq	%rdx, cur_allocator(%rip)
	ret
	.cfi_endproc
.LFE4:
	.size	allocate, .-allocate
	.globl	get_random_array
	.type	get_random_array, @function
get_random_array:
.LFB5:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	movq	%rdi, %r12
	call	allocate
	movq	%rax, %r13
	movl	$0, %ebx
	jmp	.L7
.L8:
	leaq	0(%r13,%rbx,8), %rbp
	movl	$0, %eax
	call	gen_random
	movq	%rax, 0(%rbp)
	addq	$1, %rbx
.L7:
	cmpq	%r12, %rbx
	jl	.L8
	movq	%r13, %rax
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE5:
	.size	get_random_array, .-get_random_array
	.globl	sort
	.type	sort, @function
sort:
.LFB6:
	.cfi_startproc
	movl	$0, %r10d
	jmp	.L11
.L13:
	addq	$1, %rax
.L12:
	cmpq	%rdi, %rax
	jge	.L16
	leaq	(%rsi,%r10,8), %r8
	movq	(%r8), %rcx
	leaq	(%rsi,%rax,8), %rdx
	movq	(%rdx), %r9
	cmpq	%r9, %rcx
	jle	.L13
	movq	%r9, (%r8)
	movq	%rcx, (%rdx)
	jmp	.L13
.L16:
	movq	%r11, %r10
.L11:
	cmpq	%rdi, %r10
	jge	.L17
	leaq	1(%r10), %r11
	movq	%r11, %rax
	jmp	.L12
.L17:
	rep ret
	.cfi_endproc
.LFE6:
	.size	sort, .-sort
	.globl	print_array
	.type	print_array, @function
print_array:
.LFB7:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movq	%rdi, %rbp
	movq	%rsi, %r12
	movl	$0, %ebx
	jmp	.L19
.L20:
	movq	(%r12,%rbx,8), %rdi
	call	write_long
	addq	$1, %rbx
.L19:
	cmpq	%rbp, %rbx
	jl	.L20
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE7:
	.size	print_array, .-print_array
	.globl	run
	.type	run, @function
run:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movl	$0, %eax
	call	init_allocator
	movl	$0, %eax
	call	read_long
	movq	%rax, %rbx
	movq	%rax, %rdi
	call	get_random_array
	movq	%rax, %rbp
	movq	%rax, %rsi
	movq	%rbx, %rdi
	call	sort
	movq	%rbp, %rsi
	movq	%rbx, %rdi
	call	print_array
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE8:
	.size	run, .-run
	.comm	allocator_base,8,8
	.comm	cur_allocator,8,8
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
