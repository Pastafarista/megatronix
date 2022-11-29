//Antonio Cabrera y Alejandro Gómez 2ºMAIS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define TAM_LINEA 16
#define TAM_RAM 4096
#define NUM_FILAS 8
#define ETIQUETA 5
#define LINEA 3
#define PALABRA 4
typedef struct {
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

int hexToDec(char hex[3]);                    //Transforma un número de hexadecimal a decimal
unsigned int nextAddr(FILE *fd);              //Obtiene la siguiente línea de accesos_memoria.txt
unsigned int getEtq(unsigned int addr);       //Obtiene la etiqueta de una dirección en decimal
unsigned int getLinea(unsigned int addr);     //Obtiene el número de línea de una dirección
unsigned int getBloque(unsigned int addr);    //Obtiene el bloque de una dirección
unsigned int getPalabra(unsigned int addr);   //Obtiene la palabra de una dirección

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

    int *ETQ, *palabra, *linea, *bloque;
    ETQ = malloc(sizeof(int)*2);
    palabra = malloc(sizeof(int)*2);
    linea = malloc(sizeof(int)*2);
    bloque= malloc(sizeof(int)*2);


    ParsearDireccion(nextAddr(fd), ETQ, palabra, linea, bloque);
    printf("Etiqueta:%i\nPalabra:%i\nLinea:%i\nBloque:%i", *ETQ, *palabra, *linea, *bloque);

    return 0;
}

int hexToDec(char hex[3]){   //Conversor de Hexadecimal a Decimal 
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

unsigned int nextAddr(FILE *fd){  //Devuelve la siguiente dirección a leer
    char linea[5];
    if(fgets(linea, sizeof(linea), fd) == NULL){
        return 0;
    }
    return hexToDec(linea);
}

unsigned int getEtq(unsigned int addr){
    int ret;
    int mascara = ((TAM_RAM - 1) >> ETIQUETA) ^ (TAM_RAM - 1);  //111110000000 en el caso de etiqueta = 5
    ret = addr & mascara;                                       //Cogemos solo los 5 bits de mayor peso
    ret = (ret >> PALABRA) >> LINEA;                            //Los volvemos a desplazar para obtener la etiqueta
    return ret;
}

unsigned int getLinea(unsigned int addr){
    int ret;
    int mascara = ((TAM_RAM - 1) >> ETIQUETA) & ((TAM_RAM - 1) << PALABRA); //000001110000 en el caso de la linea = 3
    ret = addr & mascara;                                                   //Cogemos solo los 3 bits de mayor peso
    ret = ret >> PALABRA;                                                   //Lo desplazamos para obtener la linea
    return ret;
} 

unsigned int getBloque(unsigned int addr){
    int ret;
    ret = (getEtq(addr) << LINEA) + getLinea(addr); //Bloque = Etiqueta + Linea
    return ret;
}

unsigned int getPalabra(unsigned int addr){
    int ret;
    int mascara = (((TAM_RAM - 1) << PALABRA) ^ (TAM_RAM - 1)); //000000001111 en el caso de la palabra = 4
    ret = addr & mascara;
    return ret;
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
    *ETQ = getEtq(addr);
    *palabra = getPalabra(addr);
    *linea = getLinea(addr);
    *bloque = getBloque(addr);
}