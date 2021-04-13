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

int traductor();
int buscamnemonico(char[]);
void mayus(char[]);
void cargarotulos(registrorotulos[],int*);
int buscarotulo(char[],registrorotulos[],int);
int buscaregistro(char[]);
int tipodeargumento(char[]);
int identificaBase(char);

int main(){

    traductor()?printf("\nTRADUCCION CORRECTA.\n"):printf("\nERROR.\n");

    return 0;
}

int traductor(){
    FILE *instasm;
    FILE *instbin;
    char linea[DIMS],cadena[DIMS];
    int DS=-1,indicecadena,mnemonicoencontrado,argumentoscargados,cantargumentos,instruccionhexa,codigomnemo;
    registroinstruccion instruccion;
    registrorotulos rotulos[DIMS];
    int cantrotulos;

    cargarotulos(rotulos,&cantrotulos);

    if((instasm=fopen("instrucciones.txt","r"))==NULL)
        return 0;
    while(fgets(linea,DIMS,instasm)!=NULL){
        ++DS;//corregir
        printf("\n Leyendo la linea %d del archivo. \n",DS);
        mnemonicoencontrado=0;//equivale a que la linea sea una instruccion o simplemente una linea en blanco o comentario
        indicecadena=-1;
        for(int i=0;i<strlen(linea);i++){
            if(linea[i]!=' ' || indicecadena==-1 || (cadena[0]==';' && !(linea[i]==' ' && linea[i+1]==' '))){
                if(linea[i]!=' ' || indicecadena!=-1)
                    cadena[++indicecadena]=linea[i];
                cadena[indicecadena+1]='\0';//hacerlo de una manera mejor, inicializar la cadena de alguna forma 
            }
            else 
            if(indicecadena!=-1){
                if(!mnemonicoencontrado){
                    if(cadena[indicecadena]==':'){
                        strcpy(instruccion.rotulo,cadena);
                        indicecadena=-1;
                    }
                    else
                        if(cadena[0]!=';')
                        {//encontramos el mnemonico
                        strcpy(instruccion.mnemonico,cadena);
                        indicecadena=-1;
                        mnemonicoencontrado=1;
                        argumentoscargados=0;
                        cantargumentos=0+(buscamnemonico(instruccion.mnemonico)<4080)+(buscamnemonico(instruccion.mnemonico)<240);
                    }
                }
                else
                    if(argumentoscargados<cantargumentos && !(indicecadena==0 && cadena[indicecadena]==',')){
                        if(cadena[indicecadena]==',')
                            cadena[indicecadena]='\0';
                        strcpy(instruccion.argumentos[argumentoscargados],cadena);
                        indicecadena=-1;
                        argumentoscargados++;
                    }
                    else{
                        cadena[0]='\0';
                        indicecadena=-1;
                    }
            }
        }
        if(indicecadena!=-1)//existia un comentario en la linea
            strcpy(instruccion.comentario,cadena);

        printf("[%04d]: %8s %8s , %-8s",DS,instruccion.mnemonico,instruccion.argumentos[0],instruccion.argumentos[1]);

        codigomnemo=buscamnemonico(instruccion.mnemonico);
        if(cantargumentos==0)
            instruccionhexa=codigomnemo<<20 & 0xFFF00000;
        else 
            if(cantargumentos==1)
                if (codigomnemo>240 && codigomnemo<248)
                    instruccionhexa=codigomnemo<<24 & 0xFF000000 | 00<<22 & 0x00C00000 | (000000<<16 & 0x003F0000) | buscarotulo(instruccion.argumentos[0],rotulos,cantrotulos)  & 0x00000FFF;
                else
                    instruccionhexa=codigomnemo<<24 & 0xFF000000 | tipodeargumento(instruccion.argumentos[0])<<22 & 0x00C00000 | (000000<<16 & 0x003F0000) | buscaregistro(instruccion.argumentos[0]) & 0x00000FFF;
            else
                instruccionhexa=codigomnemo<<28 & 0xF0000000 | tipodeargumento(instruccion.argumentos[0])<<26 & 0x0C000000 | tipodeargumento(instruccion.argumentos[1])<<24 & 0x03000000 | buscaregistro(instruccion.argumentos[0])<<12 & 0x00FFF000 | buscaregistro(instruccion.argumentos[1])<<0 & 0x00000FFF;

        printf("\nInstruccion Hexadecimal = %08x\n",instruccionhexa);

    }
    fclose(instasm);

    return 1;
}

int buscamnemonico(char mnemonico[]){ //validar que la instruccion no exista???
    nombre mnemonicos[25]={"MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","RND","NOT","STOP"};
    int i=0;
    mayus(mnemonico);
    if(strcmp(mnemonico,"STOP")==0) return 4081;//o sino 0xFF1; se puede sacar esta linea agregando en el return final la condicion ="+(i>23)*3853"
    while(strcmp(mnemonico,mnemonicos[i])!=0)
        ++i;
    //return i-(i>=12)*12+240*(i>=12); ABAJO SE SINTETIZA EN :
    return i+(i>=12)*228;
}
void mayus(char cadena[]){ //la idea era hacerla con return pero no la pude hacer funcionar bien
    for(int i=0;i<strlen(cadena);i++)
        cadena[i]=toupper(cadena[i]);
}


void cargarotulos(registrorotulos rotulos[],int *cantrotulos){
    FILE *instasm;
    char linea[256];
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
