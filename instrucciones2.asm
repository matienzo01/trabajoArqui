    MOV AX, %1
    MOV DX, 10
    MOV CX, 1
    SYS 1
    PUSH [10]
    CALL FACT
    ADD SP, 1    
    MOV [10], AX
    MOV AX, %1
    SYS 2
    STOP

FACT: PUSH BP ;guarda BP
      MOV BP,SP
      SUB SP,1 ;Espacio var local
      PUSH BX
      PUSH CX
      CMP [BP+2],1 