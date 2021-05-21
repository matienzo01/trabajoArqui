#include "secundarios.h"
//proce y func auxiliares

int identificaBase(char dato){
    char base[17]={'*','*','*','*','*','*','*','*','@','*','#','*','*','*','*','*','%'};
    int i=0;
    while(i<=strlen(base) && base[i]!=dato)
        i++;
    if(i<=16)
        return i;
    else if(dato=='-' || (dato>='0' && dato<='9'))
        return 10;
    else

        return 99;//si no tenia ningun simbolo, y tampoco era directamente el numero en decimal, entonces era un caracter entre comillas
}

void agregarotulo(tlistaR *rotulos,char rotulo[],int numlinea){
    tlistaR aux;
    aux=(tlistaR)malloc(sizeof(nodoR));
    aux->sig=*rotulos;
    strcpy(aux->rotulo,rotulo);
    aux->numerodelinea=numlinea;
    *rotulos=aux;
}

void agregaConstante(tlistaE* constantes, int valor[], int numlinea){
    tlistaE aux;
    aux= (tlistaE) malloc(sizeof(nodoEQU));
    aux->sig=*constantes;
    for(int i=0; i<10; i++){
        aux->valor[i]=*(valor+i);
    }
    aux->linea=numlinea;
    *constantes=aux;
}

int codigooperando(char argumento[]){ 
    char caracter,comilla[2]="'";
    int i=0;
    while(argumento[i]!='[' && comilla[0]!=argumento[i] && (argumento[i]<'0' || argumento[i]>'9') && (toupper(argumento[i])<'A' || toupper(argumento[i])>'Z'))
        ++i;
    caracter=toupper(argumento[i]);
    if(caracter=='[')
        return 2;
    else
        if(caracter>='A' && caracter<='Z' && strlen(argumento)==2)
            return 1;
        else
            return 0;
}

int basebtodecimal(char cadena[],int baseb){
    int numero=0;
    for(int i=1;i<strlen(cadena);i++)
        numero+=((int)cadena[i]-48-7*((int)cadena[i]>=65))*potencia(baseb,strlen(cadena)-(i+1));
    return numero;
}

int potencia(int n,int exponente){
    int resultado=1;
    for(int i=1;i<=exponente;i++)
        resultado*=n;
    return resultado;
}

void mayus(char cadena[]){
    for(int i=0; i<strlen(cadena); i++)
        cadena[i]=toupper(cadena[i]);
}

void muestramemoria(int memoria[]){//de prueba
    printf("\n\n\n");
    for(int i=0;i<=100;i++){
        printf("%x",memoria[i]);
        if((i+1)%8==0)
            printf("\n");
    }
}

void muestralista(tlistastring informe){
    while (informe!=NULL){
        printf("%s \n",informe->cadena);
        informe=informe->sig;
    }
}