//Antonio Cabrera y Alejandro Gómez

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define TAM_LINEA 16

typedef struct {
unsigned char ETQ;
unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;


int main(int argc, char** argv){
    int globaltime = 0;
    int numfallos = 0;

    T_CACHE_LINE linea_cache;
    linea_cache.ETQ = 255;
    for(int i = 0; i < TAM_LINEA; i++){
        linea_cache.Data[i] = 35;
    }


}

