 MOV AX, 1
 MOV [0], AX
 MOV BX, [0]
 SUB AX, 1
 MOV [1], AX
 MOV EX, [1]
 SUB AX, 1
 MOV [2], AX
 MOV EX, [2]
 MUL AX, %ffff
 MOV [3], AX
 MOV FX, [3]
 LDH 65535
 LDL 65535
 MOV [4], AC
 SHR AC, 32
 MOV CX, 1
 SHL CX, 31
 SHR CX, 31
 MOV [5], CX
 SYS %F
 STOP