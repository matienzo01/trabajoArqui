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

void agregaConstante(tlistaE* constantes, char nombre[] ,char valor[]){
    tlistaE aux;
    tlistaE recorre;
    char cadena[10];
    int j=1;
    int k=0;

    int bandera=1; //1 significa numero

    aux= (tlistaE) malloc(sizeof(nodoEQU));
    aux->sig=NULL;
    strcpy(aux->nombre, nombre);
    if(*(valor)==39 || *(valor)==34){ //39 es ' y 34 "
        bandera=0;
        if(*valor==39)
            eliminaCaracter(valor, 39);
        else
            eliminaCaracter(valor, 34);
    }
    aux->tipo=bandera;
    if(bandera){
        aux->valor[0]=atoi(valor);
    }else{
        strcpy((char *)aux->valor, valor);
    }
    aux->tamanio=strlen(valor);
    if(*constantes==NULL){
        aux->bloque=0;
        *constantes=aux;
    }else{
        recorre=*constantes;
        while(recorre->sig!=NULL)
            recorre=recorre->sig;
        recorre->sig=aux;
        aux->bloque=recorre->bloque+recorre->tamanio;
    }
        
    
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

void eliminaCaracter(char *s, char c)
{
    int writer = 0, reader = 0;

    while (s[reader])
    {
        if (s[reader]!=c) 
        {   
            s[writer++] = s[reader];
        }

        reader++;       
    }

    s[writer]=0;
}