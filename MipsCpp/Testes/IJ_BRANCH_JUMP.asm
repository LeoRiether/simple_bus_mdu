.data
w1: .word 46
w2: .word 77
w3: .word 600
c1: .asciiz "Numbers for beq,bne: "
c2: .asciiz " and "
c3: .asciiz "\nNumber for bgtz,blez: "
nl: .asciiz "\n"
beq_s: .asciiz "Instruction beq activated.\n"
beq_n: .asciiz "Instruction beq not activated.\n"
bne_s: .asciiz "Instruction bne activated.\n"
bne_n: .asciiz "Instruction bne not activated.\n"
bgtz_s: .asciiz "Instruction bgtz activated.\n"
bgtz_n: .asciiz "Instruction bgtz not activated.\n"
blez_s: .asciiz "Instruction blez activated.\n"
blez_n: .asciiz "Instruction blez not activated.\n"

.text
	lw $s0,w1
	lw $s1,w2
	lw $s2,w3
	
	la $a0,c1
	jal sprint
	addi $a0,$s0,0
	jal iprint
	la $a0,c2
	jal sprint
	addi $a0,$s1,0
	jal iprint
	la $a0,c3
	jal sprint
	addi $a0,$s2,0
	jal iprint
	la $a0,nl
	jal sprint
	jal sprint
	
	beq $s0,$s1,L1
	la $a0,beq_n
	j U1
L1:
	la $a0,beq_s
U1:
	jal sprint
	
	bne $s0,$s1,L2
	la $a0,bne_n
	j U2
L2:
	la $a0,bne_s
U2:
	jal sprint
	
	bgtz $s2,L3
	la $a0,bgtz_n
	j U3
L3:
	la $a0,bgtz_s
U3:
	jal sprint
	
	blez $s2,L4
	la $a0,blez_n
	j U4
L4:
	la $a0,blez_s
U4:
	jal sprint

	la $a0,nl
	jal sprint
	
	jal end

sprint:
	li $v0,4
	syscall
	jr $ra
	
iprint:
	li $v0,1
	syscall
	jr $ra

end:
	li $v0,10
	syscall