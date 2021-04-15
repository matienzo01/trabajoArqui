#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DIMS 15
#define MAX 5

typedef char nombre[MAX];
typedef char args[MAX];

typedef struct{
    char rotulo[DIMS];
    char mnemonico[MAX];
    args argumentos[MAX];
    char comentario[DIMS];
}registroinstruccion;

typedef struct nodo{
    char rotulo[DIMS];
    int numerodelinea;
    struct nodo *sig;
}nodo;
typedef nodo* tlista;

int traductor(FILE*, FILE*);
long int buscamnemonico(char[]);
void mayus(char[]);
void cargarotulos(tlista*);
int buscarotulo(tlista,char[]);
int buscaregistro(char[]);
int tipodeargumento(char[]);
int identificaBase(char);
void agregarotulo(tlista*,char[],int);
void IniciaCadena(char[]);

int main(){
    FILE *instasm;
    FILE *instbin;
    int numero;
    char cadena[DIMS];
    strcpy(cadena, "ADD");

    if((instasm=fopen("instrucciones.txt","r"))==NULL)   return 1;
    traductor(instasm,instbin)?printf("\nTRADUCCION CORRECTA.\n"):printf("\nERROR.\n");
    //printf("%x ", buscamnemonico(cadena)<<28);
    return 0;
}

int traductor(FILE *instasm, FILE *instbin){
    
    char linea[DIMS],cadena[DIMS]={'\0'};
    int DS=-1;
    long int instruccionhexa;
    int pasoDeLectura=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    
    registroinstruccion instruccion;
    tlista rotulos;
    int cantrotulos;

    cargarotulos(&rotulos);

    
    while(fgets(linea,DIMS,instasm)!=NULL){
        i=0;
        while(i!=DIMS-1){
            switch (pasoDeLectura)
            {
            case 0: //0:busca mnemonico (ver que onda si encuentra un rotulo)(termina cuando encuentra espacio)
                while(i<DIMS && (linea[i]==' ' || linea[i]=='\n'))
                    i++;
                if(i<DIMS-1){
                    while(linea[i]!=' ')
                        cadena[++j]=linea[i++];
                    if(!buscarotulo(rotulos, cadena)){ //busco a ver si lo que encontre es un rotulo o no. Si llega a ser un rotulo, va a salir del if y va a volver a analizar si i==DIMS, y como no cumple volvera a entrar al case 0
                        instruccionhexa=0;
                        instruccionhexa= (buscamnemonico(cadena));
                        DS++;
                        //pasoDeLectura=1;
                        j=-1;
                        printf("%s %LX\n", cadena, instruccionhexa);
                        IniciaCadena(cadena);

                    }
                }
                break;
            case 1: //1 busca argumentos(finaliza con ; ,  ver que onda si son 2), 
                i++;
                pasoDeLectura=2;
                break;
            case 2: //2 busca comentario(finaliza con )
                pasoDeLectura=0;
                i++;
                break;
            
            }
        }
        
    }
    fclose(instasm);

    return 1;
}

long int buscamnemonico(char mnemonico[]){
    nombre mnemonicos[25]={"MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","RND","NOT","STOP"};
    long int i=0;

    mayus(mnemonico);

    while(i<=24 && strcmp(mnemonico,mnemonicos[i])!=0)
        ++i;
    if(i<=24)
        i= i+(i>=12)*228+(i>23)*3829;
    else
        i= 4095;
    if(i<=11)
        return i<<28;
    else if(i<=22)
        return i<<24;
    else
        return i<<20;
}


void cargarotulos(tlista *rotulos){
    FILE *instasm;
    char linea[256];
    char posiblerotulo[256];
    int numlinea=-1;
    int i;
    int indice;
    *rotulos=NULL;
    if((instasm=fopen("instrucciones.txt","r"))==NULL)
        printf("Error");//Deberia retornar 1, segun clase de youtube

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
    /*probando mostrar lista*/
    tlista aux=*rotulos;
    while(aux!=NULL){
        printf(" %s  %d\n",aux->rotulo,aux->numerodelinea);
        aux=aux->sig;
    }
}

int buscarotulo(tlista rotulos,char rotulo[]){
    int i=0;
    char aux[DIMS]; //auxiliar para agregarle el : sin modificar el original
    strcpy(aux, rotulo);
    strcat(aux,":");// le agrega el : al rotulo leido como argumento para comparar.. ver otra forma mejor
    while(rotulos!=NULL && strcmp(aux,rotulos->rotulo)!=0)
        rotulos=rotulos->sig;
    if(rotulos!=NULL)//encontro el rotulo
        return rotulos->numerodelinea;
    else
        return 0; //ojo que aca habia un 0xFFF, ver implicancias
}

int buscaregistro(char argumento[]){//deberia llamarse buscaargumento
    nombre registros[16]={"DS","\0","\0","\0","\0","IP","\0","\0","CC","AC","AX","BX","CX","DX","EX","FX"};
    int i=0;
    mayus(argumento);
    printf("\nArgumento == %s\n",argumento);
    if(argumento[0]=='['){
        int x;char aux[20];
        for(int i=1;i<strlen(argumento)-1;i++)
            aux[i-1]=argumento[i];
        x=atoi(aux);
        return x;
    }
    else
        if(argumento[0]<='9')
            return atoi(argumento);
        else{
            while(strcmp(argumento,registros[i])!=0) //validar que no este el nombre del registro??
                ++i;
            return i;
        }
}

int tipodeargumento(char argumento[]){
    if(argumento[0]=='[')
        return 2;
    else
        if(argumento[0]>='0' && argumento[0]<='9')
            return 0;
        else
            return 1;
}

void agregarotulo(tlista *rotulos,char rotulo[],int numlinea){
    tlista aux;
    aux=(tlista)malloc(sizeof(nodo));
    aux->sig=*rotulos;
    strcpy(aux->rotulo,rotulo);
    aux->numerodelinea=numlinea;
    *rotulos=aux;
}

int identificaBase(char dato){
    char base[17]={'*','*','*','*','*','*','*','@','*','*','#','*','*','*','*','*','%'};
    int i=0;

    while(i<=strlen(base) && base[i]!=dato){
        i++;
    }
    if(i>strlen(base))
        return i;
        else
            return 10; //si no hay nada es base 10
}

void mayus(char cadena[]){ //la idea era hacerla con return pero no la pude hacer funcionar bien
    for(int i=0;i<strlen(cadena);i++)
        cadena[i]=toupper(cadena[i]);
}

void IniciaCadena(char cadena[DIMS]){
    for(int i=0; i<strlen(cadena); i++)
        cadena[i]='\0';
}