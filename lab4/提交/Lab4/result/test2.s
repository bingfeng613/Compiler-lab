.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

fact:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -72
  sw $t9, 68($sp)
  sw $t8, 64($sp)
  sw $s7, 60($sp)
  sw $s6, 56($sp)
  sw $s5, 52($sp)
  sw $s4, 48($sp)
  sw $s3, 44($sp)
  sw $s2, 40($sp)
  sw $s1, 36($sp)
  sw $s0, 32($sp)
  sw $t7, 28($sp)
  sw $t6, 24($sp)
  sw $t5, 20($sp)
  sw $t4, 16($sp)
  sw $t3, 12($sp)
  sw $t2, 8($sp)
  sw $t1, 4($sp)
  sw $t0, 0($sp)
  addi $sp, $sp, -16
  move $t0, $a0
  sw $t0, -76($fp)
  lw $t1, -76($fp)
  li $t2, 1
  beq $t1, $t2, label1
  lw $t3, -76($fp)
  li $t4, 1
  sub $t5, $t3, $t4
  sw $t5, -80($fp)
  lw $t6, -80($fp)
  move $a0, $t6
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t7, $v0
  sw $t7, -84($fp)
  lw $s0, -76($fp)
  lw $s1, -84($fp)
  mul $s2, $s0, $s1
  sw $s2, -88($fp)
  lw $s3, -88($fp)
  move $v0, $s3
  addi $sp, $fp, -72
  lw $t0, 0($sp)
  lw $t1, 4($sp)
  lw $t2, 8($sp)
  lw $t3, 12($sp)
  lw $t4, 16($sp)
  lw $t5, 20($sp)
  lw $t6, 24($sp)
  lw $t7, 28($sp)
  lw $s0, 32($sp)
  lw $s1, 36($sp)
  lw $s2, 40($sp)
  lw $s3, 44($sp)
  lw $s4, 48($sp)
  lw $s5, 52($sp)
  lw $s6, 56($sp)
  lw $s7, 60($sp)
  lw $t8, 64($sp)
  lw $t9, 68($sp)
  addi $sp, $sp, 72
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
  j label3
label1:
  lw $s4, -76($fp)
  move $v0, $s4
  addi $sp, $fp, -72
  lw $t0, 0($sp)
  lw $t1, 4($sp)
  lw $t2, 8($sp)
  lw $t3, 12($sp)
  lw $t4, 16($sp)
  lw $t5, 20($sp)
  lw $t6, 24($sp)
  lw $t7, 28($sp)
  lw $s0, 32($sp)
  lw $s1, 36($sp)
  lw $s2, 40($sp)
  lw $s3, 44($sp)
  lw $s4, 48($sp)
  lw $s5, 52($sp)
  lw $s6, 56($sp)
  lw $s7, 60($sp)
  lw $t8, 64($sp)
  lw $t9, 68($sp)
  addi $sp, $sp, 72
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
label3:

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -16
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -4($fp)
  lw $t1, -4($fp)
  move $t2, $t1
  sw $t2, -8($fp)
  lw $t3, -8($fp)
  li $t4, 1
  bgt $t3, $t4, label4
  li $t5, 1
  move $t6, $t5
  sw $t6, -12($fp)
  j label6
label4:
  lw $t7, -8($fp)
  move $a0, $t7
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $s0, $v0
  sw $s0, -16($fp)
  lw $s1, -16($fp)
  move $s2, $s1
  sw $s2, -12($fp)
label6:
  lw $s3, -12($fp)
  move $a0, $s3
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $v0, $zero
  move $sp, $fp
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
