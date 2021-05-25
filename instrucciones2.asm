    MOV AX, %1
    MOV DX, 10
    MOV CX, 1
    SYS 1
    PUSH [10]
    CALL FACTO
    ADD SP, 1    
    MOV [10], AX
    MOV AX, %1
    SYS 2
    STOP
FACTO: PUSH BP ;guarda BP
      MOV BP,SP
      SUB SP,1 ;Espacio var local
      PUSH BX
      PUSH CX
      CMP [BP+2],1 
      JP SIGO
      MOV AX,1
      JMP FIN
SIGO: MOV BX,[BP+2]
      SUB BX,1
      PUSH BX
      CALL FACTO
      ADD SP,1
      MOV CX,[BP+2]
      MUL CX,AX
      MOV [BP-1],CX
      MOV AX,[BP-1]
FIN:  POP CX
      POP BX
      ADD SP,1
      MOV SP,BP
      POP BP
      RET