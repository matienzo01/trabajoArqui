\\ASM  DATA=50 Stack=20 
        text equ "Hola"
        carac equ 'c'
        num equ 20
                                mov [10], 0 ;inicializo variables
         mov                   [20],    1
OTRO:   cmp [20], 100 ;compara
        jp fin ;salta si llego a 100 o mas
                        ;probando linea de solo comentario
        swap [10],              [20]
        add [20], [10]
                mov AX, %F01
        mov CX, 1
                        mov DX, 10
                        not [30]
        sys %F ;print [10] en decimal
        jmp otro
FIN:    stop