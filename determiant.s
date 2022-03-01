.data
format_scan:   .asciz "%d"
format_print:  .asciz "%d\n"

.text
.global main
 
main:
 push {r4, lr}
 mov r4, #0
 scan:
  sub sp, #4
  ldr r0, =format_scan
  mov r1, sp
  bl scanf
  add r4, #1
  cmp r4, #9
  bne scan

 

 mov r0, #0
 ldr r1, [sp, #32]
 ldr r2, [sp, #16]
 mul r1, r2
 add r0, r1

 ldr r1, [sp, #16]
 ldr r2, [sp]
 mul r1, r2
 add r0, r1

 ldr r1, [sp]
 ldr r2, [sp, #32]
 mul r1, r2
 add r0, r1

 ldr r1, [sp, #28]
 ldr r2, [sp, #20]
 mul r1, r2
 sub r0, r1

 ldr r1, [sp, #24]
 ldr r2, [sp, #8]
 mul r1, r2
 sub r0, r1

 ldr r1, [sp, #12]
 ldr r2, [sp, #4]
 mul r1, r2
 sub r0, r1


        mov r1, r0
 ldr r0, = format_print
 bl printf

 add sp, #36
 pop {r4, lr}
 bx lr
