	.file	"array.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB41:
	.cfi_startproc
	movl	$2, %eax
	jmp	.L2
.L3:
	addl	$1, %eax
.L2:
	cmpl	$31, %eax
	jbe	.L3
	movl	$0, %eax
	ret
	.cfi_endproc
.LFE41:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
