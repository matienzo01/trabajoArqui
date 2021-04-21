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

typedef struct nodo{
    char rotulo[DIMS];
    int numerodelinea;
    struct nodo *sig;
}nodo;
typedef nodo* tlista;

//--------
void generainstruccion(int,registroinstruccion,int*,int,tlista,int*,int*,int,tlistastring*,tlistastring*);
void agregainforme(tlistastring*,char[],int,char[],char[]);
void muestramemoria(int[]);//despues borrar
void muestralista(tlistastring);
void generabin(int[],FILE*);
//---------

//proce y func para funcionar
int traductor(FILE*,int[],int,char[],int*,int*,tlistastring*,tlistastring*);

void cargarotulos(tlista*, char[]);
int buscarotulo(tlista,char[]);
int buscamnemonico(char[]);
int buscaargumento(char[]);
int buscaregistro(char[]);
void imprimeLineas(registroinstruccion,int,int,int,int);


//proce y func auxiliares
int identificaBase(char);
void agregarotulo(tlista*,char[],int);
int codigooperando(char[]);
int basebtodecimal(char[],int);
int potencia(int,int);
void mayus(char[]);


int main(int argc, char* argv[]){ //implementar parametro y si esta se tiene q imprimir
    FILE *instasm;
    FILE *instbin;
    tlistastring informeserrores=NULL,informeswarnings=NULL;
    int memoria[4096], parametro;
    int errores=0,warnings=0;
    char aux[DIMS]={'\0'};
    
    if((instasm=fopen("instrucciones.txt","r"))==NULL) //if((instasm=fopen(argv[1],"r"))==NULL) 
        return 1;
    //parametro= (strcmp(argv[1],"-o")==0)? 1:0;
    traductor(instasm,memoria,parametro,argv[1],&errores,&warnings,&informeserrores,&informeswarnings)?printf("\nTRADUCCION CORRECTA.\n"):printf("\nERROR.\n");
    
    /*printf("Errores = %d \nWarnings = %d \n",errores,warnings);

    if(errores==0)
        generabin(memoria,instbin);
    else
        printf("No se genero el archivo Programa Codigo Maquina ya que hubo errores en la trauduccion.\n");
    
    muestralista(informeserrores);muestralista(informeswarnings);*/


    return 0;
}

int traductor(FILE *instasm,int memoria[],int bandera, char archivo[], int* errores,int *warnings,tlistastring *informeserrores,tlistastring *informeswarnings){
    
    char linea[DIMS],cadena[DIMS]={'\0'};
    int DS=-1;
    int cantArgumentos; //cantArgumentos va a definir cuantas veces debe permanecer en el case 1
    int pasoDeLectura=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    int instruccionHexa;
    
//----------lo de abajo agregado el 20/04
    int linearotulo;
    int huboerror;
    int lineasininstruccion;
//----------lo de arriba agregado el 20/04
    registroinstruccion instruccion;
    tlista rotulos;

    int argumentosCargados,codmnemo;

    cargarotulos(&rotulos, archivo);
    memset(instruccion.rotulo,0,strlen(instruccion.rotulo));

    while(fgets(linea,DIMS,instasm)!=NULL){
        memset(instruccion.comentario,0,strlen(instruccion.comentario));
        pasoDeLectura=0;
        i=0;
        huboerror=0;
        lineasininstruccion=0;
        while(i<strlen(linea)){
            while(i<strlen(linea) && (linea[i]==' ' || linea[i]==','))
                 i++;
            if(i<strlen(linea)){ //una linea puede ser solo un comentario, problema para el futuro
                j=-1;
                while(linea[i]!=' ' && linea[i]!='\n' && linea[i]!=',' && linea[i]!=';')// la condicion de la ',' es para cuuando lee dos argumentos
                    cadena[++j]=linea[i++];
                if((linea[i]==';' || linea[i]=='\n') && pasoDeLectura==0){//estaba buscando mnemonico y encontro comentario o linea en blanco   
                    if(linea[i]==';')
                        instruccion.comentario[0]=linea[i];
                    lineasininstruccion=1;
                    pasoDeLectura=2;
                }
                switch (pasoDeLectura){
                    case 0:
                            if(buscarotulo(rotulos,cadena)==0xFFF){
                                ++DS;
                                strcpy(instruccion.mnemonico,cadena);
                                codmnemo=buscamnemonico(cadena);
                                if(codmnemo!=0xFFF){
                                    argumentosCargados=0;
                                    cantArgumentos=(codmnemo<4080)+(codmnemo<240);
                                    instruccionHexa=codmnemo<<(20+cantArgumentos*4);          
                                }
                                else{
                                    huboerror=1;
                                    ++(*errores);
                                    agregainforme(informeserrores,"En la linea ",DS," no existe el mnemonico : ",instruccion.mnemonico);
                                    instruccionHexa=0xFFFFFFFF;
                                }
                                pasoDeLectura=1;
                                memset(cadena,0,strlen(cadena));
                            }
                            else{//era un rotulo
                                strcpy(instruccion.rotulo,cadena);
                                memset(cadena,0,strlen(cadena));
                            }
                        break;
                    case 1:
                            if(argumentosCargados<cantArgumentos){
                                strcpy(instruccion.argumentos[argumentosCargados],cadena);
                                argumentosCargados++;
                                memset(cadena,0,strlen(cadena));
                                if(argumentosCargados==cantArgumentos)
                                    pasoDeLectura=2;
                            }
                            else{
                                memset(instruccion.comentario,0,strlen(instruccion.comentario));
                                pasoDeLectura=2;
                            }
                        break;
                    case 2: //2 busca comentario
                        strcat(instruccion.comentario," ");
                        strcat(instruccion.comentario,cadena);
                        memset(cadena,0,strlen(cadena));
                        break;
                    }
            }
            ++i;
        }  
        if(!huboerror && !lineasininstruccion)
            generainstruccion(codmnemo,instruccion,&instruccionHexa,cantArgumentos,rotulos,errores,warnings,DS,informeserrores,informeswarnings);
        imprimeLineas(instruccion,DS,cantArgumentos,instruccionHexa,lineasininstruccion);
        for(int h=0;h<=7;h++)
            memoria[DS*8+h]=instruccionHexa>>(28-(h*4)) & 0x0000000F;
        memset(instruccion.rotulo,0,strlen(instruccion.rotulo));
    }
    fclose(instasm);

    return 1;
}//-----------------------------------------------------------------------Agrego todo a partir de aca
void generabin(int memoria[],FILE *instabin){
    //completar
}

void generainstruccion(int codigomnemo,registroinstruccion instruccion,int *instruccionhexa,int cantargumentos,tlista rotulos,int *errores,int *warnings,int DS,tlistastring *informeserrores,tlistastring *informeswarnings){
    int i,linearotulo,esrotulo;
    int codoperando[3],args[3];
    for(int i=0;i<cantargumentos;i++){
        esrotulo=instruccion.argumentos[i][0]>='A' && instruccion.argumentos[i][0]<='Z';
        codoperando[i]=codigooperando(instruccion.argumentos[i]);
        if(!esrotulo){
            args[i]=buscaargumento(instruccion.argumentos[i]);
            if(codoperando[i]==0 && (args[i]>4095+((-cantargumentos+2)*61440)) )//se trunca el operando
            {
                ++(*warnings);
                agregainforme(informeswarnings,"En la linea ",DS," se trunco el operando ",instruccion.argumentos[i]);
            } 
        }
        else
            linearotulo=buscarotulo(rotulos,instruccion.argumentos[0]);
    }
    if(cantargumentos==0)
        *instruccionhexa=codigomnemo<<20 & 0xFFF00000;
    else 
        if(cantargumentos==1)
            if (codigomnemo>240 && codigomnemo<248 && esrotulo){
                if(linearotulo==0xFFF){
                    //agregainforme(informeserrores,"En la linea ",DS," no existe el rotulo : ",instruccion.argumentos[0]);
                    ++(*errores);
                }
                *instruccionhexa=codigomnemo<<24 & 0xFF000000 | 00<<22 & 0x00C00000 | (000000<<16 & 0x003F0000) | linearotulo & 0x00000FFF;
            }
            else
                *instruccionhexa=codigomnemo<<24 & 0xFF000000 | codoperando[0]<<22 & 0x00C00000 | (000000<<16 & 0x003F0000) | args[0] & 0x00000FFF;
        else
            *instruccionhexa=codigomnemo<<28 & 0xF0000000 | codoperando[0]<<26 & 0x0C000000 | codoperando[1]<<24 & 0x03000000 | args[0]<<12 & 0x00FFF000 | args[1] & 0x00000FFF;
}

void muestramemoria(int memoria[]){
    printf("\n\n\n");
    for(int i=0;i<=100;i++){
        printf("%x",memoria[i]);
        if((i+1)%8==0)
            printf("\n");
    }
}
void muestralista(tlistastring informe){
    while (informe!=NULL)
    {
        printf(informe->cadena);
        informe=informe->sig;
    }
}
void agregainforme(tlistastring *informe,char frase1[],int linea,char frase2[],char dato[]){
    tlistastring nuevo;
    char aux[MAX];
    sprintf(aux,"%d",linea);
    nuevo=(tlistastring)malloc(sizeof(nodostring));
    strcpy(nuevo->cadena,frase1);strcat(nuevo->cadena,aux);
    strcat(nuevo->cadena,frase2);strcat(nuevo->cadena,dato);
    nuevo->sig=*informe;
    *informe=nuevo;
}

//--------------------------------------------------------Hasta aca agregue

int buscaargumento(char argumento[]){
    int i=0,baseb;
    char aux[20];
    mayus(argumento);
    if(argumento[0]>='A' && argumento[0]<='Z')
        return buscaregistro(argumento);
    else{
        if(argumento[0]=='[')
            for(int i=1;i<strlen(argumento)-1;i++)
                aux[i-1]=argumento[i];
        else
            for(int i=0;i<strlen(argumento);i++)
                aux[i]=argumento[i];
        baseb=identificaBase(*aux);
        if(baseb==10)
            return atoi(aux);
        else if(baseb<=16)
            return basebtodecimal(aux,baseb);
        else
            return (int)argumento[1];//era 'A' ...o... 'A ...o... 'a ...o... 'a'        
    }
}
int buscaregistro(char registro[]){
    char aux[DIMS]={'\0'};
    int i=0;
    strcpy(aux, registro);
    mayus(aux);
    nombre registros[16]={"DS","\0","\0","\0","\0","IP","\0","\0","CC","AC","AX","BX","CX","DX","EX","FX"};
    while(strcmp(aux,registros[i])!=0)
        ++i;
    return i;
}

int codigooperando(char argumento[]){ /*devuelve el codigo del operando */
    if(argumento[0]=='[')
        return 2;
    else
        if(argumento[0]>='A' && argumento[0]<='Z')
            return 1;
        else
            return 0;
}

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

int buscamnemonico(char mnemonico[]){
    nombre mnemonicos[25]={"MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","RND","NOT","STOP"};
    int i=0;
    long int aux;
    mayus(mnemonico);
    while(i<=24 && strcmp(mnemonico,mnemonicos[i])!=0)
        ++i;
    if(i<=24)
        return i+(i>=12)*228+(i>23)*3829;
    else
        return 4095;
}
void agregarotulo(tlista *rotulos,char rotulo[],int numlinea){
    tlista aux;
    aux=(tlista)malloc(sizeof(nodo));
    aux->sig=*rotulos;
    strcpy(aux->rotulo,rotulo);
    aux->numerodelinea=numlinea;
    *rotulos=aux;
}

void cargarotulos(tlista *rotulos, char archivo[]){
    FILE *instasm;
    char linea[256];
    char posiblerotulo[256];
    int numlinea=-1;
    int i;
    int indice;
    *rotulos=NULL;
    
    instasm=fopen("instrucciones.txt","r");//fopen(archivo,"r");
    while(fgets(linea,256,instasm)!=NULL){
        ++numlinea;
        i=0;
        indice=-1;
        posiblerotulo[0]='\0';
        while(i<strlen(linea) && (linea[i]!=' ' || indice==-1)){
            posiblerotulo[++indice]=linea[i];
            posiblerotulo[indice+1]='\0';
            ++i;
        }
        if(posiblerotulo[indice]==':')
            agregarotulo(rotulos,posiblerotulo,numlinea);
    }
    fclose(instasm);
}
int buscarotulo(tlista rotulos,char rotulo[]){
    char aux[DIMS];
    strcpy(aux,rotulo);
    if(rotulo[strlen(rotulo)-1]!=':'){
        strcat(aux,":");    
    }
    while(rotulos!=NULL && strcmp(aux,rotulos->rotulo)!=0)
        rotulos=rotulos->sig;
    if(rotulos!=NULL)
        return rotulos->numerodelinea;
    else
        return 0xFFF;
}

void mayus(char cadena[]){
    for(int i=0; i<strlen(cadena); i++)
        cadena[i]=toupper(cadena[i]);
}

void imprimeLineas(registroinstruccion instruccion, int DS, int cantidadArgumetos, int instruccionHexa,int sininstruccion){
if(!sininstruccion){
    printf("[%04d]: %02x %02x %02x %02x",DS,instruccionHexa>>24 & 0x000000FF,instruccionHexa>>16 & 0x000000FF,instruccionHexa>>8 & 0x000000FF,instruccionHexa & 0x000000FF);
    if(strlen(instruccion.rotulo)!=0)
        printf(" %11s ",instruccion.rotulo);
    else
        printf(" %10d: ",DS+1);
    switch (cantidadArgumetos){
        case 2:
            printf("%-04s %10s, %-10s ;%s\n",instruccion.mnemonico,instruccion.argumentos[0],instruccion.argumentos[1],instruccion.comentario);
        break;
        case 1:
            printf("%-04s %10s %-10s  ;%s\n",instruccion.mnemonico,instruccion.argumentos[0],"",instruccion.comentario);
        break;
        case 0:
            printf("%-04s %23s;%s\n",instruccion.mnemonico,"",instruccion.comentario);
        break;
    }
}
else
    printf("%20s\n",instruccion.comentario);
}