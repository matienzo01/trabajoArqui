#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DIMS 256
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
int buscamnemonico(char[]);
void mayus(char[]);
void cargarotulos(tlista*);
int buscarotulo(tlista,char[]);
int buscaregistro(char[]);
int tipodeargumento(char[]);
int identificaBase(char);

int main(){
    FILE *instasm;
    FILE *instbin;

    if((instasm=fopen("instrucciones.txt","r"))==NULL)   return 1;
    traductor(instasm,instbin)?printf("\nTRADUCCION CORRECTA.\n"):printf("\nERROR.\n");

    return 0;
}

int traductor(FILE *instasm, FILE *instbin){
    
    char linea[DIMS],cadena[DIMS];
    int DS=-1,indicecadena=-1,instruccionhexa;
    int pasoDeLectura=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    
    registroinstruccion instruccion;
    tlista rotulos;
    int cantrotulos;

    cargarotulos(&rotulos);

    
    while(fgets(linea,DIMS,instasm)!=NULL){
        i=0;
        while(i!=DIMS){
            switch (pasoDeLectura)
            {
            case 0: //0:busca mnemonico (ver que onda si encuentra un rotulo)(termina cuando encuentra espacio)
                while(linea[i]==' ')
                    i++;
                while(linea[i]!=' ')
                    cadena[++j]=linea[i];
                if(!buscarotulo(rotulos, cadena)){ //busco a ver si lo que encontre es un rotulo o no. Si llega a ser un rotulo, va a salir del if y va a volver a analizar si i==DIMS, y como no cumple volvera a entrar al case 0
                    instruccionhexa=0;
                    instruccionhexa= buscamnemonico(cadena)<<24 & 0xFFF00000;
                    pasoDeLectura=1;
                    DS++;
                }
                break;
            case 1: //1 busca argumentos(finaliza con ; ,  ver que onda si son 2), 
                break;
            case 2: //2 busca comentario(finaliza con )
                pasoDeLectura=0;
                break;
            
        }
        }
        
    }
    fclose(instasm);

    return 1;
}

int buscamnemonico(char mnemonico[]){
    nombre mnemonicos[25]={"MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","RND","NOT","STOP"};
    int i=0;

    mayus(mnemonico);

    while(i<=24 && strcmp(mnemonico,mnemonicos[i])!=0)
        ++i;
    if(i<=24)
        return i+(i>=12)*228+(i>23)*3829;
    else
        return 4095;
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
    //probando mostrar lista
    tlista aux=*rotulos;
    while(aux!=NULL){
        printf(" %s  %d\n",aux->rotulo,aux->numerodelinea);
        aux=aux->sig;
    }
}
int buscarotulo(tlista rotulos,char rotulo[]){
    int i=0;
    printf("Buscando rotulo : %s Tenemos como primero de la lista:  %s \n",rotulo,rotulos->rotulo);
    strcat(rotulo,":");// le agrega el : al rotulo leido como argumento para comparar.. ver otra forma mejor
    while(rotulos!=NULL && strcmp(rotulo,rotulos->rotulo)!=0)
        rotulos=rotulos->sig;
    if(rotulos!=NULL)//encontro el rotulo
        return rotulos->numerodelinea;
    else
        return 0xFFF;
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

int identificaBase(char dato){
    char base[17]={'*','*','*','*','*','*','*','@','*','*','#','*','*','*','*','*','%'};
    int i=0;

    while(i<=strlen(base) && base[i]!=dato){
        i++;
    }

    return i;
}