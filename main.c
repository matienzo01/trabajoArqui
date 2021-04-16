#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DIMS 30
#define MAX 5

typedef char nombre[MAX];

typedef struct{
    char rotulo[DIMS];
    char mnemonico[MAX];
    int argumentos[3];
    char comentario[DIMS];
}registroinstruccion;

typedef struct nodo{
    char rotulo[DIMS];
    int numerodelinea;
    struct nodo *sig;
}nodo;
typedef nodo* tlista;

//proce y func para funcionar
int traductor(FILE*,int[],int*);

void cargarotulos(tlista*);
int buscarotulo(tlista,char[]);
int buscamnemonico(char[]);
int buscaargumento(char[]);
int buscaregistro(char[]);


//proce y func auxiliares
void mayus(char[]);
int identificaBase(char);
void IniciaCadena(char[]);
void agregarotulo(tlista*,char[],int);
int codigooperando(char[]);
int basebtodecimal(char[],int);
int potencia(int,int);


int main(){ //implementar parametro y si esta se tiene q imprimir
    FILE *instasm;
    FILE *instbin;
    int memoria[4096];
    int errores;
    
    if((instasm=fopen("instrucciones.txt","r"))==NULL) 
        return 1;
    traductor(instasm,memoria,&errores)?printf("\nTRADUCCION CORRECTA.\n"):printf("\nERROR.\n");

    //if(errores==0)
    //    generobin(memoria);
    //else
    //    printf("No se llevo a cabo la traudccion porque hubieron %d errores.\n",errores);


    return 0;
}

int traductor(FILE *instasm,int memoria[],int *errores){
    
    char linea[DIMS],cadena[DIMS]={'\0'};
    int DS=-1;
    int cantArgumentos; //cantArgumentos va a definir cuantas veces debe permanecer en el case 1
    int pasoDeLectura=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    
    registroinstruccion instruccion;
    tlista rotulos;

    int argumentosCargados,codmnemo;

    cargarotulos(&rotulos);

    while(fgets(linea,DIMS,instasm)!=NULL){
        strcpy(instruccion.comentario,"\0");
        pasoDeLectura=0;
        i=0;
        while(i<strlen(linea)){
            while(i<strlen(linea) && (linea[i]==' ' || linea[i]==','))
                 i++;
            if(i<strlen(linea)){ //una linea puede ser solo un comentario, problema para el futuro
                j=-1;
                while(linea[i]!=' ' && linea[i]!=',' && linea[i]!='\n' && linea[i]!=';')//la condicion de la ',' es para cuuando lee dos argumentos
                    cadena[++j]=linea[i++];
                switch (pasoDeLectura){
                    case 0: //se busca el menmonico ... o el posible rotulo
                            if(buscarotulo(rotulos,cadena)==0xFFF){//es un mnemonico
                                ++DS;
                                strcpy(instruccion.mnemonico,cadena);
                                printf("MNEMONICO = %s \n",instruccion.mnemonico);
                                codmnemo=buscamnemonico(cadena);
                                if(codmnemo!=0xFFF){
                                    argumentosCargados=0;
                                    cantArgumentos=(codmnemo<4080)+(codmnemo<240);    
                                    printf(" Cant Argumentos = %d \n",cantArgumentos);      
                                }
                                else//error, se puso un mnemonico que no existe
                                    ++(*errores);
                                pasoDeLectura=1;
                                memset(cadena,0,strlen(cadena));
                            }
                            else{//era un rotulo
                                strcpy(instruccion.rotulo,cadena);
                                memset(cadena,0,strlen(cadena));
                            }
                        break;
                    case 1: // se buscan el o los argumentos... habia un "i++" para que sirve??
                        if(argumentosCargados < cantArgumentos){
                            instruccion.argumentos[++argumentosCargados]=buscaargumento(cadena);
                            printf(" ARGUMENTO %d = %d ",argumentosCargados,instruccion.argumentos[argumentosCargados]);
                            memset(cadena,0,strlen(cadena));
                        }
                        else 
                            pasoDeLectura=2;
                        break;
                    case 2: //2 busca comentario(, tambien habia un i++ ...
                        if(i<strlen(linea))
                            strcat(instruccion.comentario,cadena);
                        break;
                    }
            }
            ++i;//puse aca el i++ pero nose cualquiera mande para ver si andaba
        }   
        printf("\n\n [%04d] %s %d %d \n\n",DS,instruccion.mnemonico,instruccion.argumentos[0],instruccion.argumentos[1]);
        //if(*errores==0)
            //armoinstruccionhexa(memoria,instruccion); y la paso al vector memoria
    }
    fclose(instasm);

    return 1;
}

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
    int i=0;
    nombre registros[16]={"DS","\0","\0","\0","\0","IP","\0","\0","CC","AC","AX","BX","CX","DX","EX","FX"};
    while(strcmp(registro,registros[i])!=0)
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
    else if(dato>='0' && dato<='9')
        return 10;
    else
        return 99;//si no tenia ningun simbolo, y tampoco era directamente el numero en decimal, entonces era un caracter entre comillas
}
void mayus(char cadena[]){ 
    for(int i=0;i<strlen(cadena);i++)
        cadena[i]=toupper(cadena[i]);
}


int basebtodecimal(char cadena[],int baseb){
    int numero=0;
    for(int i=1;i<strlen(cadena);i++)
        numero+=((int)cadena[i]-48)*potencia(baseb,strlen(cadena)-(i+1));
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
void cargarotulos(tlista *rotulos){
    FILE *instasm;
    char linea[256];
    char posiblerotulo[256];
    int numlinea=-1;
    int i;
    int indice;
    *rotulos=NULL;
    
    instasm=fopen("instrucciones.txt","r");
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
    if(rotulo[strlen(rotulo)]!=':'){
        strcpy(aux,rotulo);    
    }
    while(rotulos!=NULL && strcmp(aux,rotulos->rotulo)!=0)
        rotulos=rotulos->sig;
    if(rotulos!=NULL)
        return rotulos->numerodelinea;
    else
        return 0xFFF;
}