	.file	"alloc.c"
	.text
	.globl	init_allocator
	.type	init_allocator, @function
init_allocator:
.LFB0:
	.cfi_startproc
	leaq	allocator_base(%rip), %rax
	movq	%rax, cur_allocator(%rip)
	ret
	.cfi_endproc
.LFE0:
	.size	init_allocator, .-init_allocator
	.globl	allocate
	.type	allocate, @function
allocate:
.LFB1:
	.cfi_startproc
	movq	cur_allocator(%rip), %rax
	leaq	(%rax,%rdi,8), %rdx
	movq	%rdx, cur_allocator(%rip)
	ret
	.cfi_endproc
.LFE1:
	.size	allocate, .-allocate
	.comm	allocator_base,8,8
	.comm	cur_allocator,8,8
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
