#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DIMS 50
#define MAX 5

typedef char nombre[MAX];

typedef struct{
    char rotulo[DIMS];
    char mnemonico[MAX];
    int argumentos[4];
    char comentario[DIMS];
}registroinstruccion;

typedef struct nodo{
    char rotulo[DIMS];
    int numerodelinea;
    struct nodo *sig;
}nodo;
typedef nodo* tlista;

//proce y func para funcionar
int traductor(FILE*,int[],int, char[], int*);

void cargarotulos(tlista*, char[]);
int buscarotulo(tlista,char[]);
int buscamnemonico(char[]);
int buscaargumento(char[], tlista);
int buscaregistro(char[]);
void imprimeLineas(registroinstruccion, int, int, int, tlista);


//proce y func auxiliares
int identificaBase(char);
void agregarotulo(tlista*,char[],int);
int codigooperando(char[]);
int basebtodecimal(char[],int);
int potencia(int,int);
void mayus(char[]);
void imprimeRotulo(tlista, int, int);


int main(int argc, char* argv[]){ 
    FILE *instasm;
    FILE *instbin;
    int memoria[4096], parametro;
    int errores=97;
    char aux[DIMS]={'\0'};
    
    if((instasm=fopen("instrucciones.txt","r"))==NULL) //if((instasm=fopen(argv[1],"r"))==NULL) 
        return 1;
    printf("%x",2<<1);
    //parametro= (strcmp(argv[1],"-o")==0)? 1:0;
    //traductor(instasm, memoria, parametro, argv[1], &errores)?printf("\nTRADUCCION CORRECTA.\n"):printf("\nERROR.\n");
    
    
    return 0;
}

int traductor(FILE *instasm,int memoria[],int bandera, char archivo[], int* errores){
    
    char linea[DIMS],cadena[DIMS]={'\0'};
    int DS=-1;
    int cantArgumentos; //cantArgumentos va a definir cuantas veces debe permanecer en el case 1
    int pasoDeLectura=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    int instruccionHexa, hexaAux;
    
    registroinstruccion instruccion;
    tlista rotulos;

    int argumentosCargados,codmnemo;

    cargarotulos(&rotulos, archivo);

    while(fgets(linea,DIMS,instasm)!=NULL){
        //strcpy(instruccion.comentario,"\0");
        memset(instruccion.comentario,0,strlen(instruccion.comentario));
        memset(instruccion.rotulo,0,strlen(instruccion.rotulo));
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
                                codmnemo=buscamnemonico(cadena);
                                if(codmnemo!=0xFFF){
                                    argumentosCargados=0;
                                    cantArgumentos=(codmnemo<4080)+(codmnemo<240);
                                    instruccionHexa=codmnemo<<(20+cantArgumentos*4);          
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
                    case 1: //aca habria q entrar a fijarse el warning
                        if(argumentosCargados < cantArgumentos){
                            hexaAux=0;
                            instruccion.argumentos[argumentosCargados+2]= hexaAux= codigooperando(cadena);
                            if(cantArgumentos==2){
                                switch (hexaAux)
                                {
                                case 1:
                                    hexaAux=hexaAux<<26;
                                    break;
                                case 2:
                                    break;
                                default:
                                    break;
                                }
                            }else{

                            }
                            instruccion.argumentos[argumentosCargados]= hexaAux= buscaargumento(cadena, rotulos);
                            
                            //cargamos el tipo y el operando en el hexa
                            argumentosCargados++;
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
        imprimeLineas(instruccion,DS, cantArgumentos, instruccionHexa, rotulos);
        memset(instruccion.rotulo,0,strlen(instruccion.rotulo));
        //if(*errores==0)
            //armoinstruccionhexa(memoria,instruccion); y la paso al vector memoria
    }
    fclose(instasm);

    return 1;
}

int buscaargumento(char argumento[], tlista rotulos){
    int i=0,baseb, j;
    char aux[20];
    mayus(argumento);
    if(argumento[0]>='A' && argumento[0]<='Z'){
        j=buscaregistro(argumento);
        if(j==17){ //no era un registro sino un rotulo
            return buscarotulo(rotulos, argumento);
        }else
            return i;
    }       
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
    while(i<=16 && strcmp(registro,registros[i])!=0)
        ++i;
    return i;
}

int codigooperando(char argumento[]){ /*devuelve el codigo del operando */
    if(argumento[0]=='[') //directo
        return 2;
    else
        if(argumento[0]>='A' && argumento[0]<='Z'){ //lo explico en imprimeLinea
            return (buscaregistro(argumento)==17)? 3:1;
        } 
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
        if(posiblerotulo[indice]==':'){
            posiblerotulo[indice]='\0';
            agregarotulo(rotulos,posiblerotulo,numlinea);
        }
            
    }
    fclose(instasm);
}
int buscarotulo(tlista rotulos,char rotulo[]){
    char aux[DIMS]={'\0'};
    strcpy(aux,rotulo); 
    if(aux[strlen(aux)-1]==':'){
        aux[strlen(aux)-1]='\0';   
    }
    while(rotulos!=NULL && strcmp(aux,rotulos->rotulo)!=0)
        rotulos=rotulos->sig;
    if(rotulos!=NULL)
        return rotulos->numerodelinea;
    else
        return 0xFFF;
}

void imprimeRotulo(tlista rotulos, int numeroLinea, int j){
    while(rotulos!=NULL && rotulos->numerodelinea!=numeroLinea)
        rotulos=rotulos->sig;
    if(rotulos!=NULL){
        while(j<46-strlen(rotulos->rotulo)){ 
            j++;
            printf(" ");
        }
        printf("%s", rotulos->rotulo);
    }
        
}

void mayus(char cadena[]){
    for(int i=0; i<strlen(cadena); i++)
        cadena[i]=toupper(cadena[i]);
}

void imprimeLineas(registroinstruccion instruccion, int DS, int cantidadArgumetos, int instruccionHexa, tlista rotulos){
    char aux[DIMS]={'\0'};
    char cadena[DIMS]={'\0'};
    int indice, j;

    printf("[%04d]: ",DS);
    //aca falta agregar la impresion en hexa, agrego los espacios q ocuparian
    printf("%02X %02X %02X %02X", instruccionHexa>>24 & 0xFF, instruccionHexa>>16 & 0xFF, instruccionHexa>>8 & 0xFF);
    
    
    if(strlen(instruccion.rotulo)!=0){
        strcpy(cadena, instruccion.rotulo);
        indice=strlen(cadena);
    }
    else{
        itoa(DS+1, cadena, 10);
        indice=strlen(cadena);
        cadena[indice]=':';
        indice++;
    }

    j=19; //despues de todo lo de la izquierda quedan un total de 19 espacios

    while(j<32-indice){
        printf(" ");
        j++;
    }

    printf("%s ", cadena);
    printf("%s ", instruccion.mnemonico);
    j+=strlen(instruccion.mnemonico)+strlen(cadena); //setea el indice luego del mnemonico

    memset(cadena, 0, strlen(cadena));
    
    for(int i=0; i<cantidadArgumetos; i++){
        if(i==1) //esta linea solo se ejecuta si hay 2 argumentos
            printf(", ");
        switch (instruccion.argumentos[i+2])//segun  el tipo del parametro
        {
        case 0: //inmediato
            itoa(instruccion.argumentos[i], aux, 10);
            if(i==0)
                while(j<46-strlen(aux)){ //el menos 2 son los dos corchetes
                    j++;
                    printf(" ");
                }
            printf("%s", aux);
            memset(aux, 0, strlen(aux));
            break;
        case 1: //de registro
            
            break;
        case 2: //directo
            itoa(instruccion.argumentos[i], aux, 10);
            if(i==0)
                while(j<46-strlen(aux)-2){ //el menos 2 son los dos corchetes
                    j++;
                    printf(" ");
                }
            printf("[%s]", aux);
            memset(aux, 0, strlen(aux));
            break;
        case 3: //nuevo tipo inventado que bautizo POR REFERENCIA (solo importa para la impresion)
            imprimeRotulo(rotulos, instruccion.argumentos[i], j);
            break;
        }
    }
    printf("\n");
    //aca se imprimiria el comentario
    
    
}