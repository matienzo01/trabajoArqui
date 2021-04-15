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

//proce y func para funcionar
int traductor(FILE*, FILE*);

void cargarotulos(tlista*);
int buscarotulo(tlista,char[]);
int buscamnemonico(char[]);
int buscaregistro(char[]);


//proce y func auxiliares
void mayus(char[]);
int identificaBase(char);
void IniciaCadena(char[]);
void agregarotulo(tlista*,char[],int);
int tipodeargumento(char[]);

int main(){ //implementar parametro y si esta se tiene q imprimir
    FILE *instasm;
    FILE *instbin;
    /*int numero;
    char cadena[DIMS];
    strcpy(cadena, "STOP");
    numero=buscamnemonico(cadena); //este bloque es para testeo de cosas
    printf("%lx ", numero);*/ 
    
    if((instasm=fopen("instrucciones.txt","r"))==NULL)   return 1;
    traductor(instasm,instbin)?printf("\nTRADUCCION CORRECTA.\n"):printf("\nERROR.\n");
    
    return 0;
}

int traductor(FILE *instasm, FILE *instbin){
    
    char linea[DIMS],cadena[DIMS]={'\0'};
    int DS=-1;
    int instruccionhexa, cantArgumentos; //cantArgumentos va a definir cuantas veces debe permanecer en el case 1
    int pasoDeLectura=0, banderaFrena=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    
    registroinstruccion instruccion; //ver como integrar esto
    tlista rotulos;
    int cantrotulos;

    cargarotulos(&rotulos);

    
    while(fgets(linea,DIMS,instasm)!=NULL){
        i=0;
        while(i!=DIMS-1 && !banderaFrena){
            switch (pasoDeLectura)
            {
            case 0: //0:busca mnemonico (termina cuando encuentra espacio)
                while(i<DIMS && (linea[i]==' ' || linea[i]=='\n'))
                    i++;
                if(i<DIMS-1){ //una linea puede ser solo un comentario, problema para el futuro
                    while(linea[i]!=' ')
                        cadena[++j]=linea[i++];
                    if(!buscarotulo(rotulos, cadena)){ //busco si es un rotulo o no. Si es, va a salir del if y va a volver a analizar si i==DIMS, como no cumple volvera a entrar al case 0
                        instruccionhexa=0;
                        instruccionhexa= (buscamnemonico(cadena));
                        DS++;
                        strcpy(instruccion.mnemonico, cadena);
                        //pasoDeLectura=1;
                        if(instruccionhexa<=240){
                            instruccionhexa=instruccionhexa<<28;
                            cantArgumentos=2;
                        }else if(instruccionhexa<=4080){
                            instruccionhexa=instruccionhexa<<24;
                            cantArgumentos=1;
                        }else{
                            instruccionhexa=instruccionhexa<<20;
                            cantArgumentos=0;
                        }
                            
                        j=-1;
                        printf("[%d] %4s %LX       tiene %d argumentos\n", DS,  instruccion.mnemonico, instruccionhexa, cantArgumentos);
                        if(strcmp(cadena,"STOP")==0)
                            banderaFrena=1;
                            else
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
    /*if(i<=11)
        return i<<28;
    else if(i<=22)
        return aux<<24;
    else
        return aux<<20;*/
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
    strcat(aux,":");
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

void mayus(char cadena[]){ 
    for(int i=0;i<strlen(cadena);i++)
        cadena[i]=toupper(cadena[i]);
}

void IniciaCadena(char cadena[DIMS]){
    for(int i=0; i<strlen(cadena); i++)
        cadena[i]='\0';
}