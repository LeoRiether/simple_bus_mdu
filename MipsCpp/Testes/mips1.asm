.data
word:	.word 0xFAFEF1F0

TAB: 	.asciiz "\t"
NL: 	.asciiz	"\n"
Label:	.asciiz "Teste"
l_ok:	.asciiz	" OK"
l_fail:	.asciiz	" FAIL"

.text

teste1:	la $t0, word	# Le palavra da memoria com Lw e compara com constante
	lw $t1, 0($t0)
	li $t2, 0xFAFEF1F0
	li $a1, 1
	beq $t1, $t2, t1_ok
	jal FAIL
	j teste2
t1_ok:	jal OK

teste2:	la $t0, word	# Le byte da memoria com lb e compara com constante
	lb $t1, 0($t0)  # Verifica extensao de sinal
	li $t2, 0xFFFFFFF0
	li $a1, 2
	beq $t1, $t2, t2_ok
	jal FAIL
	j teste3
t2_ok:	jal OK

teste3:	la $t0, word	# Le byte da memoria com lbu e compara com constante
	lbu $t1, 0($t0) # Verifica leitura sem sinal
	li $t2, 0x0F0
	li $a1, 3
	beq $t1, $t2, t3_ok
	jal FAIL
t3_ok:	jal OK


FIM:   	li $v0, 10
	syscall

OK:	# $a1 eh o numero do teste
	la $a0, Label
	li $v0, 4
	syscall
	add $a0, $zero, $a1
	li $v0, 1
	syscall
	la $a0, l_ok
	li $v0, 4
	syscall
	la $a0, NL
	syscall
	jr $ra

FAIL:   # $a1 eh o numero do teste
	la $a0, Label
	li $v0, 4
	syscall

	add $a0, $zero, $a1
	li $v0, 1
	syscall

	la $a0, l_fail
	li $v0, 4
	syscall
	la $a0, NL
	syscall
	jr $ra