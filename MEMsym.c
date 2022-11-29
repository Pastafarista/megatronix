//Antonio Cabrera y Alejandro Gómez 2ºMAIS
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
int nextDir(FILE *fd);      //Obtiene la siguiente línea de accesos_memoria.txt
int getEtq(int dir);        //Obtiene la etiqueta de una dirección en decimal
int getLinea(int dir);      //Obtiene el número de línea de una dirección
int getBloque(int dir);     //Obtiene el bloque de una dirección
int getPalabra(int dir);    //Obtiene la palabra de una dirección

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

    fd = fopen("CONTENTS_RAM.bin", "r");  //Abrimos y volcamos el contenido en la RAM
    if(fd == NULL){
        printf("[Error al cargar la RAM]\n");
        return -1;
    }
    int i = 0;
    char c = fgetc(fd); //Bucle que vuelca el fichero
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
    
    int num_linea = 0;  //Contabilizamos la ninea que leemos

    int dir = nextDir(fd);
    int etq = getEtq(dir);
    int lin = getLinea(dir);
    int block = getBloque(dir);
    int pal = getPalabra(dir);
    printf("Bloque: %i, Etiqueta: %i, Linea: %i, Palabra: %i",block,etq,lin,pal);
    num_linea++;

    return 0;
}

int hexToDec(char hex[3]){   //Conversor de Hexadecimal a Decimal (Antonio Cabrera)
    int len = 2;
    int decimal, val;

    for(int i = 0; i < 3; i++){
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

int nextDir(FILE *fd){  //Devuelve la siguiente dirección a leer
    char linea[5];
    if(fgets(linea, sizeof(linea), fd) == NULL){
        return 0;
    }
    return hexToDec(linea);
}

int getEtq(int dir){
    int ret;
    int mascara = ((TAM_RAM - 1) >> ETIQUETA) ^ (TAM_RAM - 1);  //111110000000 en el caso de etiqueta = 5
    ret = dir & mascara;                                        //Cogemos solo los 5 bits de mayor peso
    ret = (ret >> PALABRA) >> LINEA;                            //Los volvemos a desplazar para obtener la etiqueta
    return ret;
}

int getLinea(int dir){
    int ret;
    int mascara = ((TAM_RAM - 1) >> ETIQUETA) & ((TAM_RAM - 1) << PALABRA); //000001110000 en el caso de la linea = 3
    ret = dir & mascara;                                                    //Cogemos solo los 3 bits de mayor peso
    ret = ret >> PALABRA;                                                   //Lo desplazamos para obtener la linea
    return ret;
} 

int getBloque(int dir){
    int ret;
    ret = (getEtq(dir) << LINEA) + getLinea(dir);   //Bloque = Etiqueta + Linea
    return ret;
}

int getPalabra(int dir){
    int ret;
    int mascara = (((TAM_RAM - 1) << PALABRA) ^ (TAM_RAM - 1)); //000000001111 en el caso de la palabra = 4
    ret = dir & mascara;
    return ret;
}
