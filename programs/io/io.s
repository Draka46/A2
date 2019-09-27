	.file	"io.c"
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
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
