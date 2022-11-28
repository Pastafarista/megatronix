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
    unsigned int accesos_memoria[TAM_LINEA];
    FILE *fd;

    T_CACHE_LINE linea_cache;       //Reset total de la caché
    linea_cache.ETQ = 255;          // xFF -> 255
    for(int i = 0; i < TAM_LINEA; i++){
        linea_cache.Data[i] = 35;   // x23 -> 35
    }

    fd = fopen("CONTENTS_RAM.bin", "r");
    if(fd == NULL){
        printf("[Error al cargar la RAM]\n");
        return -1;
    }else{
        int i=0;
        char c = fgetc(fd);         //Leemos el contenido de la ram y lo volcamos en Simul_RAM
        while(c != EOF){   
            Simul_RAM[i++] = c;
            c = fgetc(fd);
        }
        fclose(fd);

        fd = fopen("accesos_memoria.txt", "r");
        if(fd == NULL){
            printf("[Error al leer el acceso a memoria]\n");
            return -1;
        }else{
            int direccion;
            
            return 0;
        }
    }
}

