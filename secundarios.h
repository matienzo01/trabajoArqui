#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DIMS 80
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
    nombre argumentos[4];
    char comentario[DIMS];
}registroinstruccion;

typedef struct nodoR{
    char rotulo[DIMS];
    int numerodelinea;
    struct nodoR *sig;
}nodoR;
typedef nodoR* tlistaR;

typedef struct nodoEQU{
    int linea;
    int valor[10];
    struct nodoEQU* sig;
}nodoEQU;

typedef nodoEQU* tlistaE;

//proce y func auxiliares
int identificaBase(char);
void agregarotulo(tlistaR*,char[],int);
void agregaConstante(tlistaE*, int[], int);
int codigooperando(char[]);
int basebtodecimal(char[],int);
int potencia(int,int);
void mayus(char[]);
void muestramemoria(int[]);//despues borrar
void muestralista(tlistastring);