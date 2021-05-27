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
    strlwr(rotulo);
    aux->sig=*rotulos;
    strcpy(aux->rotulo,rotulo);
    aux->numerodelinea=numlinea;
    *rotulos=aux;
}

void agregaConstante(tlistaES* constantesS, tlistaEC* ctesC,char nombre[] ,char valor[]){
    tlistaES auxS;
    tlistaES recorreS;
    tlistaEC auxC;
    int base;

    int bandera; //0 significa numero, 1 significa caracter, 2 significa string
    strlwr(nombre);

    if(*(valor)==39 || *(valor)==34) //39 es ' y 34 "
        if(*valor==39){
            eliminaCaracter(valor, 39);
            bandera=1;
        }else{
            eliminaCaracter(valor, 34);
            bandera=2;
        }
    else
        bandera=0;
    
    if(bandera==0 || bandera==1){
        auxC= (tlistaEC) malloc(sizeof(NODOEQUC));
        strcpy(auxC->nombre, nombre);
        if(bandera)
            auxC->valor= (int) (*valor);
        else{
            eliminaCaracter(valor, 9);
            if((*valor)==45)
                base=10;
                else
                    base=identificaBase(*valor);
            if(base!=10 || *valor=='#')
                auxC->valor= basebtodecimal(valor, base);
            else
                auxC->valor=atoi(valor);
            
        }
        auxC->tipo=bandera;
        auxC->sig=*ctesC;
        *ctesC=auxC;
    }else{
        auxS= (tlistaES) malloc(sizeof(nodoEQUS));
        
        strcpy(auxS->nombre, nombre);
        auxS->tipo=bandera;
        auxS->tamanio=strlen(valor);
        strcpy(auxS->valor, valor);
        auxS->sig=NULL;
        if(*constantesS==NULL){
            auxS->bloque=1;
            
            *constantesS=auxS;
        }else{
            recorreS=*constantesS;
            while(recorreS->sig!=NULL)
                recorreS=recorreS->sig;
            recorreS->sig=auxS;
            auxS->bloque=recorreS->bloque+recorreS->tamanio+1;
        }
    }
}

int codigooperando(char argumento[]){ 
    char caracter,comilla[2]="'";
    int i=0;
    
    while(argumento[i]!='[' && comilla[0]!=argumento[i] && (argumento[i]<'0' || argumento[i]>'9') && (toupper(argumento[i])<'A' || toupper(argumento[i])>'Z'))
        ++i;
    caracter=toupper(argumento[i]);
    if(caracter=='['){
        if((argumento[i+1]>='0' && argumento[i+1]<='9') || (argumento[i+1]=='%' || argumento[i+1]=='@' || argumento[i+1]=='#' || argumento[i+1]==comilla[0]))
            return 2;
        else 
            return 3;
    }
    else{
        char auxiliar[3];
        auxiliar[0]=caracter;
        auxiliar[1]=argumento[i+1];
        if(buscaregistro(auxiliar)<=15) //antes decia caracter>='A' && caracter<='Z' && strlen(argumento)==2 , pero salia un error en el 4 de la primer entrega
            return 1; 
        else
            return 0;//si el buscareg da 16, lo que se le mando no era un registro tipo AX BX DS BP SP,era un rotulo por ejemplo
    }
}

int basebtodecimal(char cadena[],int baseb){
    int numero=0;
    char aux[10];strcpy(aux,cadena);mayus(aux);
    for(int i=1;i<strlen(cadena);i++)
        numero+=((int)aux[i]-48-7*((int)aux[i]>=65))*potencia(baseb,strlen(aux)-(i+1));
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
int esString(tlistaES strings,char arg[]){
    while(strings!=NULL && strcasecmp(strings->nombre,arg)!=0)
        strings=strings->sig;
    return strings!=NULL;
}
int buscaConstante(tlistaES constantesS, tlistaEC ctesC, char nombre[]){
    int respuesta=0xFFFFFF; //supone q no lo encuentra, llega hasta ahi para no dar stack overflow
    int bandera=0;

    while(constantesS!=NULL && !bandera){
        if(strcasecmp(constantesS->nombre, nombre)==0){
            respuesta=constantesS->bloque;
            bandera=1;
        }
        constantesS=constantesS->sig;
    }
    if(!bandera){
        while(ctesC!=NULL && !bandera){
            if(strcasecmp(ctesC->nombre, nombre)==0){
                respuesta=ctesC->valor;
                bandera=1;
            }
            ctesC=ctesC->sig;
        }
    }
    return respuesta;
}

int buscaregistro(char registro[]){
    int i=0;
    nombre registros[16]={"DS","SS","ES","CS","HP","IP","SP","BP","CC","AC","AX","BX","CX","DX","EX","FX"};
    while(i<=15 && strcasecmp(registro,registros[i])!=0)
        ++i;
    return i;
}

int buscaTipoCte(tlistaES constantesS, tlistaEC ctesC, char nombre[]){
    int respuesta=4; //supone q no lo encuentra
    int bandera=0;

    while(constantesS!=NULL && !bandera){
        if(strcasecmp(constantesS->nombre, nombre)==0){
            respuesta=constantesS->tipo;
            bandera=1;
        }
        constantesS=constantesS->sig;
    }
    if(!bandera){
        while(ctesC!=NULL && !bandera){
            if(strcasecmp(ctesC->nombre, nombre)==0){
                respuesta=ctesC->tipo;
                bandera=1;
            }
            ctesC=ctesC->sig;
        }
    }
    return respuesta;
}

int buscaSimbolo(tlistastring simbolos, char cadena[]){
    int respuesta=0;
    while(simbolos!=NULL){
        if(strcasecmp(simbolos->cadena, cadena)==0)
            respuesta++;
        simbolos=simbolos->sig;
    }
    return respuesta;
}

void agregaSimbolos(tlistastring* simbolos, char cadena[]){
    tlistastring aux;
    aux=(tlistastring) malloc(sizeof(nodostring));
    strlwr(cadena);
    strcpy(aux->cadena, cadena);
    aux->sig=*simbolos;
    *simbolos=aux;
}
