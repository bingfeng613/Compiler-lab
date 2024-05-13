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

main:
  addi $sp, $sp, -4
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -32
  move $t0, $zero
  sw $t0, -4($fp)
  li $t1, 1
  move $t2, $t1
  sw $t2, -8($fp)
  move $t3, $zero
  sw $t3, -12($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t4, $v0
  sw $t4, -16($fp)
  lw $t5, -16($fp)
  move $t6, $t5
  sw $t6, -20($fp)
label1:
  lw $t7, -12($fp)
  lw $s0, -20($fp)
  blt $t7, $s0, label2
  j label3
label2:
  lw $s1, -4($fp)
  lw $s2, -8($fp)
  add $s3, $s1, $s2
  sw $s3, -24($fp)
  lw $s4, -24($fp)
  move $s5, $s4
  sw $s5, -28($fp)
  lw $s6, -8($fp)
  move $a0, $s6
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $s7, -8($fp)
  move $t8, $s7
  sw $t8, -4($fp)
  lw $t9, -28($fp)
  move $t1, $t9
  sw $t1, -8($fp)
  lw $t0, -12($fp)
  li $t2, 1
  add $t3, $t0, $t2
  sw $t3, -32($fp)
  lw $t2, -32($fp)
  move $t4, $t2
  sw $t4, -12($fp)
  j label1
label3:
  move $v0, $zero
  move $sp, $fp
  lw $fp, 0($sp)
  addi $sp, $sp, 4
  jr $ra
