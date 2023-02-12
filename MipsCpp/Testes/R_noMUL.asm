.data
n1: .word 6 
n2: .word -10
n3: .word 4
n4: .word 15
n5: .word -20
n6: .word 2
n7: .word -16
n8: .word -34
sp: .asciiz " "
nl: .asciiz " (srl - sra)\n"

.text
	lw $s0,n1
	lw $s1,n2
	lw $s2,n3
	lw $s3,n4
	lw $s4,n5
	lw $s5,n6
	lw $s6,n7
	lw $s7,n8
	
	add $t0,$s0,$s2
	sub $t1,$s4,$s7
	and $t2,$s0,$s5
	or $t3,$s2,$s5
	xor $t4,$s3,$s1
	nor $t5,$s6,$s4
	slt $t6,$s4,$s6
	sll $t7,$s2,4
	srl $t8,$s6,2
	sra $t9,$s6,2
	
	li $v0,1
	add $a0,$zero,$t8
	syscall
	li $v0,4
	la $a0,sp
	syscall
	
	li $v0,1
	add $a0,$zero,$t9
	syscall
	li $v0,4
	la $a0,nl
	syscall
	
	li $v0,10
	syscall