.data
op1: .word 50
op2: .word 4
op3: .word 4099
op4: .word 16
di: .asciiz " / "
rs: .asciiz " % "
eq: .asciiz " = "
mu: .asciiz " * "
nl: .asciiz "\n"
bl: .asciiz "("
br: .asciiz ") "

.text
	lw $s0,op1
	lw $s1,op2
	lw $s2,op3
	lw $s3,op4
	
	mult $s0,$s1
	mfhi $t0
	mflo $t1
	div $s2,$s3
	mfhi $t2
	mflo $t3
	
	add $a0,$s0,$zero
	li $v0,1
	syscall
	
	la $a0,mu
	li $v0,4
	syscall
	
	add $a0,$s1,$zero
	li $v0,1
	syscall
	
	la $a0,eq
	li $v0,4
	syscall
	
	la $a0,bl
	syscall
	
	li $v0,1
	add $a0,$t0,$zero
	syscall
	
	la $a0,br
	li $v0,4
	syscall
	
	li $v0,1
	add $a0,$t1,$zero
	syscall
	
	li $v0,4
	la $a0,nl
	syscall
	
	add $a0,$s2,$zero
	li $v0,1
	syscall
	
	la $a0,di
	li $v0,4
	syscall
	
	add $a0,$s3,$zero
	li $v0,1
	syscall
	
	la $a0,eq
	li $v0,4
	syscall
	
	li $v0,1
	add $a0,$t3,$zero
	syscall
	
	li $v0,4
	la $a0,nl
	syscall
	
	add $a0,$s2,$zero
	li $v0,1
	syscall
	
	la $a0,rs
	li $v0,4
	syscall
	
	add $a0,$s3,$zero
	li $v0,1
	syscall
	
	la $a0,eq
	li $v0,4
	syscall
	
	li $v0,1
	add $a0,$t2,$zero
	syscall
	
	li $v0,4
	la $a0,nl
	syscall
	
	li $v0,10
	syscall