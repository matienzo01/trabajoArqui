#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DIMS 100
#define MAX 10

typedef struct nodostring{
    char cadena[DIMS];
    struct nodostring *sig;
}nodostring;
typedef nodostring *tlistastring;

typedef char nombre[MAX];

typedef struct{
    char rotulo[DIMS];
    char mnemonico[MAX];
    nombre argumentos[10]; //22/05 - le cambie de 4 a  10, mas tamaño para los operandos indirectos
    char comentario[DIMS];
}registroinstruccion;

typedef struct nodoR{
    char rotulo[DIMS];
    int numerodelinea;
    struct nodoR *sig;
}nodoR;
typedef nodoR* tlistaR;

typedef struct nodoEQUS{
    char nombre[10];
    int bloque; //bloque de memoria que ocupara
    char valor[10];
    int tipo;
    int tamanio;
    struct nodoEQUS* sig;
}nodoEQUS;

typedef nodoEQUS* tlistaES;

typedef struct NODOEQUC{
    char nombre[10];
    int valor;
    int tipo;
    struct  NODOEQUC* sig;    
}NODOEQUC;

typedef NODOEQUC* tlistaEC;

//proce y func auxiliares
int identificaBase(char);
void agregarotulo(tlistaR*,char[],int);
void agregaConstante(tlistaES*, tlistaEC*,char[], char[]);
int codigooperando(char[]);
int basebtodecimal(char[],int);
int potencia(int,int);
void mayus(char[]);
void muestramemoria(int[]);//despues borrar
void muestralista(tlistastring);
void eliminaCaracter(char *s, char c);
int buscaConstante(tlistaES, char[]);
