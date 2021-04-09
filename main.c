#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CANTM 25

typedef struct {
    int clave;
    char mnemonico[4];
}nodo;

void cargaInstrucciones(nodo[]);

int main()
{
    FILE* texto;
    FILE* archivoBinario;
    nodo instrucciones[CANTM];
    int instruccionActual;

    cargaInstrucciones(instrucciones);

    //texto= fopen("datos.txt", "r");
    //archivoBinario= fopen("salida.bin", "wb");
    //cargar las instrucciones
    //recorre el archivo en busca de rotulos
    //leer una linea de una instruccion
    //reconocer el mnomico
    //

    fwrite(&instruccionActual, sizeof(int), 1, archivoBinario);
    return 0;
}
//salto:  MOV [10], 10

void cargaInstrucciones(nodo instrucciones[CANTM]){

    instrucciones[0].clave=0x0; strcpy(instrucciones[0].mnemonico, "MOV");
    instrucciones[1].clave=0x1; strcpy(instrucciones[1].mnemonico, "ADD");
    instrucciones[2].clave=0x2; strcpy(instrucciones[2].mnemonico, "SUB");
    instrucciones[3].clave=0x3; strcpy(instrucciones[3].mnemonico, "SWAP");
    instrucciones[4].clave=0x4; strcpy(instrucciones[4].mnemonico, "MUL");
    instrucciones[5].clave=0x5; strcpy(instrucciones[5].mnemonico, "DIV");
    instrucciones[6].clave=0x6; strcpy(instrucciones[6].mnemonico, "CMP");
    instrucciones[7].clave=0x7; strcpy(instrucciones[7].mnemonico, "SHL");
    instrucciones[8].clave=0x8; strcpy(instrucciones[8].mnemonico, "SHE");
    instrucciones[9].clave=0x9; strcpy(instrucciones[9].mnemonico, "AND");
    instrucciones[10].clave=0xA; strcpy(instrucciones[10].mnemonico, "OR");
    instrucciones[11].clave=0xB; strcpy(instrucciones[11].mnemonico, "XOR");
    instrucciones[12].clave=0xF0; strcpy(instrucciones[12].mnemonico, "SYS");
    instrucciones[13].clave=0xF1; strcpy(instrucciones[13].mnemonico, "JMP");
    instrucciones[14].clave=0xF2; strcpy(instrucciones[14].mnemonico, "JZ");
    instrucciones[15].clave=0xF3; strcpy(instrucciones[15].mnemonico, "JP");
    instrucciones[16].clave=0xF4; strcpy(instrucciones[16].mnemonico, "JN");
    instrucciones[17].clave=0xF5; strcpy(instrucciones[17].mnemonico, "JNZ");
    instrucciones[18].clave=0xF6; strcpy(instrucciones[18].mnemonico, "JNP");
    instrucciones[19].clave=0xF7; strcpy(instrucciones[19].mnemonico, "JNN");
    instrucciones[20].clave=0xF8; strcpy(instrucciones[20].mnemonico, "LDL");
    instrucciones[21].clave=0xF9; strcpy(instrucciones[21].mnemonico, "LDH");
    instrucciones[22].clave=0xFA; strcpy(instrucciones[22].mnemonico, "RND");
    instrucciones[23].clave=0xFB; strcpy(instrucciones[23].mnemonico, "NOT");
    instrucciones[24].clave=0xFF1; strcpy(instrucciones[24].mnemonico, "STOP");

}
