.data 
num: .word 3822
dec: .asciiz "Decimal: "
bin: .asciiz "\nInverted Binary: "
nel: .asciiz "\n"
.text
	lw $s0,num
	la $a0,dec
	jal sprint
	addi $a0,$s0,0
	li $v0,1
	syscall
	
	la $a0,nel
	jal sprint
	la $a0,bin
	jal sprint
	
	addi $a1,$s0,0
loop:
	jal mod_proc
	addi $a0,$v0,1
	li $v0,1
	syscall
	srl $a1,$a1,1
	bne $a1,$zero,loop
	
	la $a0,nel
	jal sprint
	
	li $v0,10
	syscall
	
mod_proc:	#a1: number, v0: number%2
	srl $t9,$a1,1
	add $t9,$t9,$t9
	sub $v0,$a1,$t9
	addi $v0,$v0,-1
	jr $ra
	
sprint:
	li $v0,4
	syscall
	jr $ra
	
