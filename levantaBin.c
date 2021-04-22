#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE* bin;
    int hexa;

    bin=fopen("binario.bin","rb");fread(&hexa,sizeof(int), 1, bin);
    while(!feof(bin)){
        
        printf("%x\n", hexa);fread(&hexa,sizeof(int), 1, bin);
    }
}