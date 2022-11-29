//Antonio Cabrera y Alejandro Gómez
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define TAM_LINEA 16
#define TAM_RAM 4096
#define ETIQUETA 5
#define LINEA 3
#define PALABRA 4

typedef struct {
unsigned char ETQ;
unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

int hexToDec(char hex[3]);  //Transforma un número de hexadecimal a decimal
int nextDir(FILE *fd);  //Obtiene la siguiente línea de accesos_memoria.txt
int getEtq(int num);    //Obtiene la etiqueta de una dirección en decimal

int main(int argc, char** argv){
    int globaltime = 0;
    int numfallos = 0;
    unsigned char Simul_RAM[TAM_RAM];
    unsigned int accesos_memoria[TAM_LINEA];
    FILE *fd;
    T_CACHE_LINE linea_cache;       

    linea_cache.ETQ = 0xFF;         
    for(int i = 0; i < TAM_LINEA; i++){
        linea_cache.Data[i] = 0x23;   
    }

    fd = fopen("CONTENTS_RAM.bin", "r");
    if(fd == NULL){
        printf("[Error al cargar la RAM]\n");
        return -1;
    }
    int i = 0;
    char c = fgetc(fd); //Leemos el contenido de la ram y lo volcamos en Simul_RAM
    while(c != EOF){   
        Simul_RAM[i++] = c;
        c = fgetc(fd);
    }
    fclose(fd);

    fd = fopen("accesos_memoria.txt", "r");
    if(fd == NULL){
        printf("[Error al leer el acceso a memoria]\n");
        return -1;
    }
    
    int num_linea = 0;

    int dir = nextDir(fd);
    int etq = getEtq(dir);
    printf("Etiqueda de %i: %i\n",dir, etq);
    num_linea++;

    return 0;
}

int hexToDec(char hex[3]){
    int len = 2;
    int decimal, val;

    for(int i = 0; i < 3; i++)
    {
        if(hex[i] >= '0' && hex[i] <= '9')
        {
            val = hex[i] - 48;
        }
        else if(hex[i] >= 'a' && hex[i] <= 'f')
        {
            val = hex[i] - 87;
        }
        else if(hex[i] >= 'A' && hex[i] <= 'F')
        {
            val = hex[i] - 55;
        }

        decimal += val * pow(16, len);
        len--;
    }
    return decimal;
}

int nextDir(FILE *fd){
    char linea[5];
    if(fgets(linea, sizeof(linea), fd) == NULL){
        return 0;
    }
    return hexToDec(linea);
}

int getEtq(int num){
    int ret;
    int mascara = ((TAM_RAM - 1) >> ETIQUETA) ^ (TAM_RAM - 1);  //111110000000 en el caso de etiqueta = 5
    ret = num & mascara;    //Cogemos solo los 5 bits de mayor peso
    ret = ret >> ETIQUETA;  //Los volvemos a desplazar para obtener la etiqueta
    return ret;
}