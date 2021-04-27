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

//proce y func para funcionar
void traductor(FILE*,int[],int,char[],int*,int*,tlistastring*,tlistastring*,int*);

void cargarotulos(tlista*, char[]);
int buscarotulo(tlista,char[]);
int buscamnemonico(char[]);
int buscaargumento(char[]);
int buscaregistro(char[]);
void imprimeLineas(registroinstruccion,int,int,int,int,int);
void generainstruccion(int,registroinstruccion,int*,int,tlista,int*,int*,int,tlistastring*,tlistastring*);
void agregainforme(tlistastring*,char[],int,char[],char[]);
void generabin(int[], char[], FILE*,int);


//proce y func auxiliares
int identificaBase(char);
void agregarotulo(tlista*,char[],int);
int codigooperando(char[]);
int basebtodecimal(char[],int);
int potencia(int,int);
void mayus(char[]);
void muestramemoria(int[]);//despues borrar
void muestralista(tlistastring);

int main(int argc, char* argv[]){ //implementar parametro y si esta se tiene q imprimir
    FILE *instasm;
    FILE *instbin;
    tlistastring informeserrores=NULL,informeswarnings=NULL;
    int memoria[4096],maxmem,parametro;
    int errores=0,warnings=0;
    char aux[DIMS]={'\0'};

    if(argc<3){ //si no le meti el archivo para levantar y/o el a generar, finaliza la ejecucucion
        printf("Cantidad de parametros minimos insuficientes");
        return 1;
    }else if(argc>4){
        printf("Exceso de parametros");
        return 1;
    }
    
    if((instasm=fopen(argv[1],"r"))==NULL) 
        return 1;
    
    if(argc==3) //si solamente viene con los nombres de los archivos
        parametro=1;
    else
        parametro= (strcmp(argv[3],"-o")==0)? 0:1; //chequea si tiene el -o

    traductor(instasm,memoria,parametro,argv[1],&errores,&warnings,&informeserrores,&informeswarnings,&maxmem);
    
    printf("Errores = %d \n",errores);
    muestralista(informeserrores);
    if(errores==0){
        generabin(memoria, argv[2],instbin,maxmem);
        printf("Se genero el binario con nombre %s\n", argv[2]);
    }
    else
        printf("No se genero el archivo %s ya que hubo errores en la traduccion.\n", argv[2]);
    printf("Warnings = %d \n",warnings);
    muestralista(informeswarnings);


    return 0;
}

void traductor(FILE *instasm,int memoria[],int bandera, char archivo[], int* errores,int *warnings,tlistastring *informeserrores,tlistastring *informeswarnings,int *maxmem){
    
    char linea[DIMS],cadena[DIMS]={'\0'};
    int DS=-1;
    int cantArgumentos;
    int pasoDeLectura=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    int instruccionHexa;
    int linearotulo;
    int huboerror;
    int lineasininstruccion;
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
            if(i<strlen(linea)){
                j=-1;
                while(linea[i]!=' ' && i<strlen(linea) && linea[i]!='\n' && linea[i]!=',' && linea[i]!=';')
                    cadena[++j]=linea[i++];
                if((linea[i]==';' || linea[i]=='\n') && pasoDeLectura==0 && strlen(cadena)==0){//estaba buscando mnemonico y encontro comentario o linea en blanco   
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
                                }
                                else{
                                    huboerror=1;
                                    ++(*errores);
                                    agregainforme(informeserrores,"En la linea ",DS+1," no existe el mnemonico : ",instruccion.mnemonico);
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
                    case 2: 
                        strcat(instruccion.comentario,cadena);
                        strcat(instruccion.comentario," ");
                        memset(cadena,0,strlen(cadena));
                        break;
                    }
            }
            ++i;
        }  
        if(!huboerror && !lineasininstruccion)
            generainstruccion(codmnemo,instruccion,&instruccionHexa,cantArgumentos,rotulos,errores,warnings,DS,informeserrores,informeswarnings);
        if(bandera)
            imprimeLineas(instruccion,DS,cantArgumentos,instruccionHexa,lineasininstruccion,huboerror);
        memoria[DS]=instruccionHexa;
        memset(instruccion.rotulo,0,strlen(instruccion.rotulo));
    }
    *maxmem=DS;
    fclose(instasm);
}

//proce y func para funcionar

void cargarotulos(tlista *rotulos, char archivo[]){
    FILE *instasm;
    char linea[DIMS];
    char posiblerotulo[DIMS];
    int numlinea=-1;
    int i;
    int indice;
    *rotulos=NULL;
    instasm=fopen(archivo,"r");
    while(fgets(linea,256,instasm)!=NULL){
        i=0;
        indice=-1;
        posiblerotulo[0]='\0';
        while(i<strlen(linea) && linea[i]==' ')
            ++i;
        if(i<strlen(linea) && linea[i]!=';')
            numlinea++;
        while(i<strlen(linea) && linea[i]!=' ' && posiblerotulo[strlen(posiblerotulo)]!=':' ){
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
    if(rotulo[strlen(rotulo)-1]!=':')
        strcat(aux,":");
    while(rotulos!=NULL && strcasecmp(aux,rotulos->rotulo)!=0)
        rotulos=rotulos->sig;
    if(rotulos!=NULL)
        return rotulos->numerodelinea;
    else
        return 0xFFF;
}

int buscamnemonico(char mnemonico[]){
    nombre mnemonicos[25]={"MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","RND","NOT","STOP"};
    int i=0;
    while(i<=24 && strcasecmp(mnemonico,mnemonicos[i])!=0)
        ++i;
    if(i<=24)
        return i+(i>=12)*228+(i>23)*3829;
    else
        return 4095;
}

int buscaargumento(char argumento[]){
    int i=0,baseb;
    char aux[MAX];memset(aux,0,strlen(aux));
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
            return (int)argumento[1];//era 'A' o algo asi        
    }
}

int buscaregistro(char registro[]){
    int i=0;
    nombre registros[16]={"DS","\0","\0","\0","\0","IP","\0","\0","CC","AC","AX","BX","CX","DX","EX","FX"};
    while(strcasecmp(registro,registros[i])!=0)
        ++i;
    return i;
}

void imprimeLineas(registroinstruccion instruccion, int DS, int cantidadArgumetos, int instruccionHexa,int sininstruccion,int huboerror){
if(!sininstruccion){
    printf("[%04d]: %02x %02x %02x %02x",DS,instruccionHexa>>24 & 0x000000FF,instruccionHexa>>16 & 0x000000FF,instruccionHexa>>8 & 0x000000FF,instruccionHexa & 0x000000FF);
    if(strlen(instruccion.rotulo)!=0)
        printf(" %11s ",instruccion.rotulo);
    else
        printf(" %10d: ",DS+1);
    if(!huboerror){
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
        printf("Error de sintaxis.\n");
}
else
    printf("%59s %s\n","",instruccion.comentario);
}

void generainstruccion(int codigomnemo,registroinstruccion instruccion,int *instruccionhexa,int cantargumentos,tlista rotulos,int *errores,int *warnings,int DS,tlistastring *informeserrores,tlistastring *informeswarnings){
    int i,linearotulo,esrotulo;
    int codoperando[3];
    long int args[3];
    for(int i=0;i<cantargumentos;i++){
        codoperando[i]=codigooperando(instruccion.argumentos[i]);
        esrotulo=codoperando[i]==0 && toupper(instruccion.argumentos[i][0])>='A' && toupper(instruccion.argumentos[i][0])<='Z';
        if(!esrotulo){
            args[i]=buscaargumento(instruccion.argumentos[i]);
            args[i]=buscaargumento(instruccion.argumentos[i]);//dejar las dos lineas repetidas, nose por que pero anda si esta dos veces al menos a mi
            if(codoperando[i]==0 && (args[i]>(4095+((-cantargumentos+2)*61440))  ))//se trunca el operando
            {
                ++(*warnings);
                agregainforme(informeswarnings,"En la linea ",DS+1," se trunco el operando ",instruccion.argumentos[i]);
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
                    agregainforme(informeserrores,"En la linea ",DS+1," no existe el rotulo : ",instruccion.argumentos[0]);
                    ++(*errores);
                }
                *instruccionhexa=codigomnemo<<24 & 0xFF000000 | 00<<22 & 0x00C00000 | (000000<<16 & 0x003F0000) | linearotulo & 0x00000FFF;
            }
            else if(codigomnemo==0xF0)
                *instruccionhexa=codigomnemo<<24 & 0xFF000000 | args[0] & 0x00000FFF;
                else
                    *instruccionhexa=codigomnemo<<24 & 0xFF000000 | codoperando[0]<<22 & 0x00C00000 | (000000<<16 & 0x003F0000) | args[0] & 0x00000FFF;
        else
            *instruccionhexa=codigomnemo<<28 & 0xF0000000 | codoperando[0]<<26 & 0x0C000000 | codoperando[1]<<24 & 0x03000000 | args[0]<<12 & 0x00FFF000 | args[1] & 0x00000FFF;
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

void generabin(int memoria[],char nombre[],FILE *instabin,int maxmem){
    int DS;//este vendria desde el main
    instabin=fopen(nombre,"wb");
    if(instabin!=NULL) 
        for(int i=0;i<maxmem;i++)
            fwrite((memoria+i),sizeof(memoria[i]),1,instabin);
}

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

void agregarotulo(tlista *rotulos,char rotulo[],int numlinea){
    tlista aux;
    aux=(tlista)malloc(sizeof(nodo));
    aux->sig=*rotulos;
    strcpy(aux->rotulo,rotulo);
    aux->numerodelinea=numlinea;
    *rotulos=aux;
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



















