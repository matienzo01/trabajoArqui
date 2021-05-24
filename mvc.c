#include "secundarios.h"

//proce y func para funcionar
void traductor(FILE*,int[],int,char[],int*,int*,tlistastring*,tlistastring*,int*);

void preproceso(tlistaR*, tlistaES*, tlistaEC*,char[], int*);
void determinaSegmentos(char*, int*, int);
int buscarotulo(tlistaR,char[]);
int buscamnemonico(char[]);
int buscaargumento(char[],int*,tlistastring*,int,tlistaES,tlistaEC);
int operandoindirecto(char[],int*,tlistastring*,int,tlistaES,tlistaEC);
void imprimeLineas(registroinstruccion,int,int,int,int,int);
void generainstruccion(int,registroinstruccion,int*,int,tlistaR,int*,int*,int,tlistastring*,tlistastring*,tlistaES,tlistaEC);
void agregainforme(tlistastring*,char[],int,char[],char[]);
void generabin(int[], char[], FILE*,int);

int main(int argc, char* argv[]){ //implementar parametro y si esta se tiene q imprimir
    FILE *instasm;
    FILE *instbin;
    tlistastring informeserrores=NULL,informeswarnings=NULL;
    int memoria[4096]={0},maxmem,parametro;
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
    int CS=-1;
    int cantArgumentos;
    int pasoDeLectura=0; 
    int i, j=-1; //i es indice de lectura y j de la cadena aux
    int instruccionHexa;
    int linearotulo;
    int huboerror;
    int lineasininstruccion;
    int cantInstrucciones=-1; //vendria a ser un CS paralelo
    registroinstruccion instruccion;
    tlistaR rotulos;
    tlistaES ctesString;
    tlistaEC ctesCarac;

    int argumentosCargados,codmnemo;

    preproceso(&rotulos, &ctesString, &ctesCarac, archivo, &CS);    
    memset(instruccion.rotulo,0,strlen(instruccion.rotulo));

    while(fgets(linea,DIMS,instasm)!=NULL){
        memset(instruccion.comentario,0,strlen(instruccion.comentario));
        pasoDeLectura=0;
        i=0;
        huboerror=0;
        lineasininstruccion=0;
        if(linea[0]==92){ //no me reconocia el caracter \ entonces le tuve q poner el valor en ASCII
            if(linea[1]==92){
                 determinaSegmentos(linea, memoria, CS);
                 i=strlen(linea);
                 lineasininstruccion=1;
            }
            else{
                //habia una \ colgada ahi en el medio, no se como decirlo
            }
        }
        while(i<strlen(linea)){
            while(i<strlen(linea) && (linea[i]==' ' || linea[i]==','))
                 i++;
            if(i<strlen(linea)){
                j=-1;
                while(linea[i]!=' ' && i<strlen(linea) && linea[i]!='\n' && linea[i]!=',' && linea[i]!=';')
                    cadena[++j]=linea[i++];
                if(buscaConstante(ctesString, ctesCarac, cadena)!=0xFFFFFF){
                    lineasininstruccion=1;
                    pasoDeLectura=3;
                    i=strlen(linea)+1;
                    memset(cadena, 0, strlen(cadena));
                }
                if((linea[i]==';' || linea[i]=='\n' || linea[i]==92) && pasoDeLectura==0 && strlen(cadena)==0){//estaba buscando mnemonico y encontro comentario o linea en blanco   
                    if(linea[i]==';'){
                        instruccion.comentario[0]=linea[i];
                        pasoDeLectura=2;
                    }
                    lineasininstruccion=1;
                }
                switch (pasoDeLectura){
                    case 0:
                            if(buscarotulo(rotulos,cadena)==0xFFF){
                                cantInstrucciones++;
                                strcpy(instruccion.mnemonico,cadena);
                                codmnemo=buscamnemonico(cadena);
                                if(codmnemo!=0xFFF){
                                    argumentosCargados=0;
                                    cantArgumentos=(codmnemo<4080)+(codmnemo<240);        
                                }
                                else{
                                    huboerror=1;
                                    ++(*errores);
                                    agregainforme(informeserrores,"En la linea ",CS+1," no existe el mnemonico : ",instruccion.mnemonico);
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
            generainstruccion(codmnemo,instruccion,&instruccionHexa,cantArgumentos,rotulos,errores,warnings,cantInstrucciones,informeserrores,informeswarnings,ctesString,ctesCarac);
        if(bandera)
            imprimeLineas(instruccion,cantInstrucciones,cantArgumentos,instruccionHexa,lineasininstruccion,huboerror);
        memoria[CS]=instruccionHexa;
        memset(instruccion.rotulo,0,strlen(instruccion.rotulo));
    }
    if(*(memoria+1)==0)
        determinaSegmentos("\\", memoria , CS);
    *maxmem=CS;
    fclose(instasm);
}

//proce y func para funcionar

void preproceso(tlistaR *rotulos, tlistaES* constantesS, tlistaEC* cteC, char archivo[], int* CS){
    FILE *instasm;
    char linea[DIMS];
    char cadena[DIMS]={0};
    char aux[13]={0}; //esta cadena solo se usa para verificar el EQU y despues reutilizada para el valor
    int i;
    int j=0;
    int indice;
    int largo=sizeof(linea);
    *rotulos=NULL; *constantesS=NULL; *cteC=NULL;

    tlistaES recorreS;
    tlistaEC recorreC;

    *CS=-1;
    instasm=fopen(archivo,"r");
    while(fgets(linea,256,instasm)!=NULL){
        i=0;
        indice=0;
        memset(cadena, 0, strlen(cadena));
        if(linea[0]==92 && linea[1]==92) //si es la linea del asm lo salto de una
            i=largo+1;
        while(i<largo && linea[i]==' ')
            ++i;
        if(i<largo && linea[i]!=';'){//si no es una linea vacia o un comentario
            while(i<largo && linea[i]!=' '){
                cadena[indice++]=linea[i++];
            }
            if(buscamnemonico(cadena)!=0xFFF){ //si es un mnemonico
                (*CS)++;
                i=largo+1;
            }else if(cadena[indice-1]==':'){ //si es un rotulo
                (*CS)++;
                agregarotulo(rotulos,cadena,(*CS));
                i=largo+1;
            }else{
                i++;
                while(i<largo && linea[i]==' ')
                    ++i;
                if(i<largo){
                    j=0;
                    while(i<largo && linea[i]!=' '){
                        aux[j++]=linea[i++];
                    }
                    if(strcasecmp(aux, "equ")==0){
                        memset(aux, 0, strlen(aux));
                        while(i<largo && linea[i]==' ')
                            ++i;
                        j=0;
                        if(linea[i]!=34)
                            while(i<largo && linea[i]!=' ' && linea[i]!='\n'){
                                aux[j++]=linea[i++];
                            }
                        else
                            while(i<largo  && linea[i]!='\n'){
                                aux[j++]=linea[i++];
                            }
                        agregaConstante(constantesS, cteC,cadena, aux);
                        memset(aux, 0, strlen(aux));
                    }else{
                        printf("Simbolo desconocido\n");//-----------------------------------------------------------simbolo desconocido
                    }
                }else{
                    printf("Simbolo desconocido\n");//----------------------------------------------------------------simbolo desconocido
                }
            }
                

        }else{
            i=largo+1;
        }            
    }
    
    recorreS=*constantesS;
    while(recorreS!=NULL){
        recorreS->bloque+=(*CS);
        recorreS=recorreS->sig;
    }
    
    printf("Las constanes de tipo caracter y/o numericos son:\n");
    recorreC=*cteC;
    while(recorreC!=NULL){
        printf("%s ", recorreC->nombre);
        if(recorreC->tipo)
            printf("%d\n", recorreC->valor);
            else
                printf("%c\n", recorreC->valor);
        recorreC=recorreC->sig;
    }
    printf("Las constantes de tipo string son:\n");

    recorreS=*constantesS;
    while(recorreS!=NULL){
        printf("%s almacena {%s} y ocupa %d espacios, de usarla sera reemplazada por el numero %d\n", recorreS->nombre, recorreS->valor, recorreS->tamanio, recorreS->bloque);
        recorreS=recorreS->sig;
    }
    fclose(instasm);
}

void determinaSegmentos(char* linea, int* memoria, int cs){
    int i=2, largo=strlen(linea); //i sera de la lectura de la linea
    int j=0, bandera=0; //j sera el indice de la cadina
    int l; //l sera el nro de bloque
    int reg, valor;
    char cadena[15]={'\0'};

    *(memoria+3)=cs;
    while(i<largo && *(linea+i)==' '){
        i++;
    }
    if(i==largo){
        printf("habia dos lineas \\ colgadas en el medio de la nada"); //--------------------------------------------------------
    }else{
        while(i<largo){
            while(i<largo && (*(linea+i)==' ' || *(linea+i)=='\n')){
                i++;
            }
            if(i<largo){
                while(*(linea+i)!=' ' && *(linea+i)!='=' ){
                    cadena[j++]=*(linea+i);
                    i++;
                }
                if(bandera==1){
                    if(strcasecmp("data", cadena)==0)
                        reg=0;
                    else if(strcasecmp("extra", cadena)==0)
                        reg=1;
                    else if(strcasecmp("stack", cadena)==0)
                        reg=2;
                    else{
                        printf("Registro desconocido: no existe el segmento %s\n", cadena); //--------------------------------------------------- aca iria un error de segmento desconocido
                        reg=-1;
                    }
                        
                    i++;//en esta posicion iria el igual
                    j=0;
                    memset(cadena, 0, strlen(cadena));
                    if(reg>=0){
                        while(*(linea+i)!=' ' && *(linea+i)!=0){
                            cadena[j++]=*(linea+i);
                            i++;
                        }
                        j=0;    
                        valor=atoi(cadena);
                        *(memoria+reg)=valor;
                    }else
                        i=largo+1; //si no existe el registo se frena la leida de la linea
                    
                }else{
                    if(strcasecmp(cadena,"ASM")==0)
                        bandera=1;
                        else{
                            printf("lo primero que se encontro fue algo que no era ASM\n"); //------------------------------------------
                            i=largo+1;
                        }
                            
                    memset(cadena, 0, strlen(cadena));
                    i++;
                    j=0;
                }
            }
            
        }   
    }
    for(int z=0; z<=3; z++){
        if(*(memoria+z)==0){
            *(memoria+z)=1024; //si no se le asigno un valor, va el por defecto
        }
    }
}

int buscarotulo(tlistaR rotulos,char rotulo[]){
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
    nombre mnemonicos[32]={"MOV","ADD","SUB","SWAP","MUL","DIV","CMP","SHL","SHR","AND","OR","XOR","SLEN", "SMOV", "SCMP", "SYS","JMP","JZ","JP","JN","JNZ","JNP","JNN","LDL","LDH","RND","NOT","PUSH","POP", "CALL","RET", "STOP"};
    int i=0;
    while(i<=32 && strcasecmp(mnemonico,mnemonicos[i])!=0)
        ++i;
    if(i<=32)
        return i+(i>=14)*225+(i>=30)*3825;
    else
        return 4095;
}

int operandoindirecto(char argumento[],int* errores,tlistastring* informeserrores,int CS,tlistaES ctesString,tlistaEC ctesCarac){
    char reg[3];
    reg[0]=argumento[1];
    reg[1]=argumento[2];
    int numRegistro=buscaregistro(reg);
    int retorno;
    if(argumento[3]==']') //era solo [AX] o [BX]
        retorno=numRegistro&0x00F;
    else{                //puede ser [AX+-algo]
        int offset=1;
        char extra[10];memset(extra,0,strlen(extra));               
        if(argumento[3]=='-')
            offset=-1;
        int z=0; //argumento[4] ya tiene el primer caracter del offset
        int esNumero=1;
        while (z+4<=strlen(argumento) && argumento[4+z]!=']'){
            extra[z]=argumento[4+z];
            if (esNumero==1 && (extra[z]<'0' || extra[z]>'9') )
                esNumero=0;
        }
        if(esNumero)
            offset*=atoi(extra);
        else{ //se suma o resta una constante, en "extra" esta cargada
            int valorconst=buscaConstante(ctesString,ctesCarac,extra);
            if(valorconst==0xFFFFFF){
                ++(*errores);
                agregainforme(informeserrores,"En la linea ",CS," --> Simbolo Desconocido: ",extra);
            }
            offset*=valorconst;
        }
        retorno= offset<<4 & 0xFF0 | numRegistro & 0x00F; 
    }
    return retorno;
}

int buscaargumento(char argumento[],int *errores,tlistastring* informeserrores,int CS,tlistaES ctesString,tlistaEC ctesCarac){
    int i=0,baseb;
    char aux[MAX];memset(aux,0,strlen(aux));
    char aux2[MAX];strcpy(aux2,argumento);
    mayus(aux2);
    if(aux2[0]>='A' && aux2[0]<='Z'){ //aca entraeria el caso que venga una constante cuyo nombre empieza con una letra
        int valor=buscaregistro(aux2);
        if(valor<=15)
            return valor;
        else{
            valor=buscaConstante(ctesString,ctesCarac,argumento);
            if(valor==0xFFFFFF){
                ++(*errores);
                agregainforme(informeserrores,"En la linea ",CS," --> Simbolo Desconocido: ",argumento);
            }        
            return valor;
        }
    }
    else{
        if(argumento[0]=='['){ //puede ser DIRECTO ejemplo [10] o puede ser INDIRECTO ejemplo [CX] o [BX+2]
            if(argumento[1]>='0' && argumento[1]<='9')
                for(int i=1;i<strlen(argumento)-1;i++)
                    aux[i-1]=argumento[i];
            else
                return operandoindirecto(argumento,errores,informeserrores,CS,ctesString,ctesCarac);
        }
        else
            strcpy(aux,argumento);//habia un for antes
        //------
        //Comprueba si el argumento no era una constante cuyo nombre empezaba con un numero
        int k=1,esNumero=1; //k empieza en 1 en vez de 0 porque se sabe que el primer caracter no es una letra y
                            // ademas para evitar analizar los posibles simbolos # % @ ' 
        while(esNumero && k<strlen(aux)){
            if(aux[k]<'0' || aux[k]>'9')
                esNumero=0;
            k++;
        }
        if(!esNumero)
            return buscaConstante(ctesString,ctesCarac,aux);
        //------
        baseb=identificaBase(*aux);
        if(baseb==10)
            return atoi(aux);
        else if(baseb<=16)
            return basebtodecimal(aux,baseb);
        else{
            char comilla[2]="'"; //solucion del ej 5 de la primer entrega, contempla el ' ' y el '  como numero 32
            if(argumento[1]=='\0' || argumento[1]==comilla[0])
                return 32;
            else
                return (int)argumento[1]; //solucion tmb del ej 5 que en vez de 'o' devolvia 'O', la solcuon es en realidad el aux2 nuevo que hay
        }//poner caso que el nombre de una constante empieze con un numero?      
    }
}

void imprimeLineas(registroinstruccion instruccion, int CS, int cantidadArgumetos, int instruccionHexa,int sininstruccion,int huboerror){
if(!sininstruccion){
    printf("[%04d]: %02x %02x %02x %02x",CS,instruccionHexa>>24 & 0x000000FF,instruccionHexa>>16 & 0x000000FF,instruccionHexa>>8 & 0x000000FF,instruccionHexa & 0x000000FF);
    if(strlen(instruccion.rotulo)!=0)
        printf(" %11s ",instruccion.rotulo);
    else
        printf(" %10d: ",CS+1);
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

void generainstruccion(int codigomnemo,registroinstruccion instruccion,int *instruccionhexa,int cantargumentos,tlistaR rotulos,int *errores,int *warnings,int CS,tlistastring *informeserrores,tlistastring *informeswarnings,tlistaES ctesString,tlistaEC ctesCarac){
    int i,linearotulo,esrotulo;
    int codoperando[3];
    long int args[3];
    for(int i=0;i<cantargumentos;i++){
        if(buscaConstante(ctesString,ctesCarac,instruccion.argumentos[i])==0xFFFFFFFF)
            codoperando[i]=codigooperando(instruccion.argumentos[i]);
        else
            codoperando[i]=3;//para no mandar la lista en el metodo codigooperando
        esrotulo=codoperando[i]==0 && toupper(instruccion.argumentos[i][0])>='A' && toupper(instruccion.argumentos[i][0])<='Z';
        if(!esrotulo){
            args[i]=buscaargumento(instruccion.argumentos[i],errores,informeserrores,CS,ctesString,ctesCarac);
            args[i]=buscaargumento(instruccion.argumentos[i],errores,informeserrores,CS,ctesString,ctesCarac);//--
            if(codoperando[i]==0 && (args[i]>(4095+((-cantargumentos+2)*61440))  ))//se trunca el operando
            {
                ++(*warnings);
                agregainforme(informeswarnings,"En la linea ",CS+1," se trunco el operando ",instruccion.argumentos[i]);
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
                    agregainforme(informeserrores,"En la linea ",CS+1," no existe el rotulo : ",instruccion.argumentos[0]);
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
    int CS;//este vendria desde el main
    int header=1297494577;
    instabin=fopen(nombre,"wb");
    if(instabin!=NULL){
        fwrite(&header, sizeof(int), 1, instabin); //agrega la cabezera de la maquina virtual
        for(int i=0;i<maxmem;i++)
            fwrite((memoria+i),sizeof(memoria[i]),1,instabin);
    } 
        
}
