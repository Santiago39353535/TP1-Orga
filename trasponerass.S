#include<mips/regdef.h>

	.text
	.globl trasponer_ass
	.align 2
	.ent trasponer_ass

trasponer_ass:
	subu sp,sp,8
	sw $fp, 4(sp)
	sw gp, 0(sp)
	and t4, a1, a1
	mulou t4, t4,a0
	li t5, 0
for:	beq t4, t5, fin
	divu t6, t5, a1
	remu t7, t5, a1
	mulou t8, t7, a0
	addu t8, t8, t6
	sll t6, t5, 3
	add t6, t6, a2
	sll t7, t8, 3
	add t7, t7, a3
	ld t0, (t6)
	sd t0, (t7)
	addi t5, t5, 1
	b for
fin:	lw gp, 0(sp)
	lw $fp, 4(sp)
	addiu sp,sp,8
	li v0, 0
	jr ra

	.end trasponer_ass
