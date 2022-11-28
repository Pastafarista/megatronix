//Antonio Cabrera y Alejandro Gómez

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define TAM_LINEA 16
#define TAM_RAM 4096

typedef struct {
unsigned char ETQ;
unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;


int main(int argc, char** argv){
    int globaltime = 0;
    int numfallos = 0;
    unsigned char Simul_RAM[TAM_RAM];


    T_CACHE_LINE linea_cache;       //RESET TOTAL DE LA CACHÉ
    linea_cache.ETQ = 255;          // xFF -> 255
    for(int i = 0; i < TAM_LINEA; i++){
        linea_cache.Data[i] = 35;   // x23 -> 35
    }

    FILE *fd;
    fd = fopen("CONTENTS_RAM", "r");
    if(fd == NULL){
        printf("[ERROR AL CARGAR EN RAM]");
    }else{

    }
}

