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
typedef struct{
    char rotulo[DIMS];
    int numerodelinea;
}registrorotulos;

int traductor(FILE*, FILE*);
int buscamnemonico(char[]);
void mayus(char[]);
void cargarotulos(registrorotulos[],int*);
int buscarotulo(char[],registrorotulos[],int);
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
    int DS=-1,indicecadena=-1,mnemonicoencontrado,argumentoscargados,cantargumentos,instruccionhexa,codigomnemo, j;
    registroinstruccion instruccion;
    registrorotulos rotulos[DIMS];
    int cantrotulos;

    cargarotulos(rotulos,&cantrotulos);

    
    while(fgets(linea,DIMS,instasm)!=NULL){
        
        mnemonicoencontrado=argumentoscargados=cantargumentos=0;

        for(int i=0; i<strlen(linea); i++){
            
            if(linea[i]!=' '){
                if(!mnemonicoencontrado){
                    
                    instruccionhexa=0; //se escribira una nueva instruccion
                    for(int j=i; j<=i+4; j++){ //todos los mnemonicos son de 4 carac o menos
                        cadena[++indicecadena]=linea[j];
                        
                    }
                    DS++; //como encontre una nueva instruccion aumento la cant de instrucciones
                    instruccionhexa= buscamnemonico("MOV"); //<<24 & 0xFFF00000;
                    printf("Pase por aca ");
                    //cadena[++indicecadena]=' ';
                    mnemonicoencontrado=1; //encontre el mnemonico
                    printf("%x \n", instruccionhexa);
                    
                }
            }
        }
    }
    fclose(instasm);

    return 1;
}

int buscamnemonico(char mnemonico[]){ //validar que la instruccion no exista???
    nombre mnemonicos[25]={"MOV","ADD","SUB ","SWAP","MUL ","DIV","CMP","SHL","SHR","AND","OR","XOR","SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","RND","NOT","STOP"};
    int i=0;
    mayus(mnemonico);
    while(strcmp(mnemonico,mnemonicos[i])!=0 && i<=25)
        ++i;
    return i+(i/12)*228+(i/24)*3061;
}
void mayus(char cadena[]){ //la idea era hacerla con return pero no la pude hacer funcionar bien
    for(int i=0;i<strlen(cadena);i++)
        cadena[i]=toupper(cadena[i]);
}


void cargarotulos(registrorotulos rotulos[],int *cantrotulos){
    char linea[256];
    FILE *instasm;
    char posiblerotulo[256];
    int numlinea=-1;
    int i;
    int indice;
    *cantrotulos=-1;
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
        if(posiblerotulo[indice]==':'){
            strcpy(rotulos[++(*cantrotulos)].rotulo,posiblerotulo);
            rotulos[*cantrotulos].numerodelinea=numlinea;
        }
    }
    fclose(instasm);
    for (int j=0;j<=(*cantrotulos);j++){
        printf("\n ROTULO = %s \n",rotulos[j].rotulo);
        printf(" LINEA = %d \n",rotulos[j].numerodelinea);
    }
}
int buscarotulo(char rotulo[],registrorotulos rotulos[],int cantrotulos){
    int i=0;
    strcat(rotulo,":");// le agrega el : al rotulo leido como argumento para comparar.. ver otra forma mejor
    while(i<=cantrotulos && strcmp(rotulo,rotulos[i].rotulo)!=0)
        ++i;
    if(i<=cantrotulos)//encontro el rotulo
        return rotulos[i].numerodelinea;
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