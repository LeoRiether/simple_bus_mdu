.data
.align 2
p1: .byte 200
.align 2
p2: .half 50000
.align 2
p3: .word 4097
.align 2
x1: .space 1
.align 2
x2: .space 2
.align 2
x3: .space 4

.text
	lb $s0,p1
	lbu $s1,p1
	sb $s1,x1
	
	lh $s2,p2
	lhu $s3,p2
	sh $s3,x2
	
	lw $s4,p3
	sw $s4,x3
	
	sb $s1,0x2019($zero)
	sh $s3,0x2002($zero)
	
	li $v0,10
	syscall
