	.file	"cond.c"
	.text
	.globl	max
	.type	max, @function
max:
.LFB0:
	.cfi_startproc
	movq	%rsi, %rax
	cmpq	%rsi, %rdi
	jg	.L3
.L2:
	rep ret
.L3:
	movq	%rdi, %rax
	jmp	.L2
	.cfi_endproc
.LFE0:
	.size	max, .-max
	.globl	min
	.type	min, @function
min:
.LFB1:
	.cfi_startproc
	movq	%rsi, %rax
	cmpq	%rsi, %rdi
	jl	.L6
.L5:
	rep ret
.L6:
	movq	%rdi, %rax
	jmp	.L5
	.cfi_endproc
.LFE1:
	.size	min, .-min
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
