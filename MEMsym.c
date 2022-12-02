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

typedef struct{
    unsigned int addr;
    unsigned int palabra;
    unsigned int bloque;
    unsigned int linea;
    unsigned int etiqueta;
}MAPA_ADDR;

T_CACHE_LINE tbl[NUM_FILAS];    
MAPA_ADDR datos[NUM_FILAS]; 

int globaltime = 0;
int numfallos = 0;

void LimpiarCACHE();
void VolcarCACHE(T_CACHE_LINE *tbl, MAPA_ADDR *datos);
void ParsearDireccion(unsigned int addr);
void TratarFallo(char *MRAM, int ETQ, int linea, int bloque);
void imprimirCache(char *MRAM, int addr, int ETQ, int linea, int bloque, int palabra);

unsigned int nextAddr(FILE *fd);              //Obtiene la siguiente línea de accesos_memoria.txt
unsigned int getEtq(unsigned int addr);       //Obtiene la etiqueta de una dirección en decimal
unsigned int getLinea(unsigned int addr);     //Obtiene el número de línea de una dirección
unsigned int getBloque(unsigned int addr);    //Obtiene el bloque de una dirección
unsigned int getPalabra(unsigned int addr);   //Obtiene la palabra de una dirección

int main(int argc, char** argv){
   
    unsigned char Simul_RAM[TAM_RAM];
    unsigned int accesos_memoria[TAM_LINEA];
    FILE *fd;
      

    LimpiarCACHE(tbl);

    //Abrimos el archivo de CONTENS_RAM.bin
    fd = fopen("CONTENTS_RAM.bin", "r");  
    if(fd == NULL){
        printf("[Error al cargar la RAM]\n");
        return -1;
    }

    //Bucle que vuelca el fichero CONTENTS_RAM.bin
    int i = 0;
    char c = fgetc(fd); 
    while(c != EOF){   
        Simul_RAM[i++] = c;
        c = fgetc(fd);
    }
    fclose(fd);

    //Abrir el archivo accesos_memoria.txt
    fd = fopen("accesos_memoria.txt", "r"); 
    if(fd == NULL){
        printf("[Error al leer el acceso a memoria]\n");
        return (-1);
    }

    for(int i = 0; i < TAM_LINEA; i++){
        ParsearDireccion(nextAddr(fd));
        if( datos[i].addr != 0){
            globaltime++;
            imprimirCache(Simul_RAM, datos[0].addr, datos[0].etiqueta ,datos[0].linea, datos[0].bloque, datos[0].palabra);
            sleep(1);
        }
    }   
    fclose(fd);
}

unsigned int nextAddr(FILE *fd){  //Devuelve la siguiente dirección a leer
    char linea[5];
    if(fgets(linea, sizeof(linea), fd) == NULL){
        return 0;
    }
    return strtol(linea, NULL, 16);
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

void LimpiarCACHE(){
    for(int i = 0; i < NUM_FILAS; i++){
        tbl[i].ETQ = 0xFF;         
        for(int k = 0; k < TAM_LINEA; k++){
            tbl[i].Data[k] = 0x23;   
        }
    }
}

void ParsearDireccion(unsigned int addr){
    datos->addr = addr;
    datos->palabra = getPalabra(addr);
    datos->bloque = getBloque(addr);
    datos->linea = getLinea(addr);
    datos->etiqueta = getEtq(addr);
}

void imprimirCache(char *MRAM, int addr, int ETQ, int linea, int bloque, int palabra){
    if(addr != tbl[linea].ETQ){
        numfallos++;
        printf("T: %d, Fallo de CACHÉ , ADDR: %04X, Etiqueta: %X, Linea: %02X, Palabra: %02X, Bloque:%02X\n", globaltime, addr, ETQ, linea, palabra, bloque);
        printf("Cargando el Bloque: %02X en Linea: %02X\n", bloque, linea);
        TratarFallo(MRAM, ETQ, linea, bloque);
    }

    for(int i = 0; i < NUM_FILAS; i++){
        printf("ETQ %X | Data", tbl[i].ETQ);
        for(int j = 0; j < TAM_LINEA; j++){
            printf(" %X ", tbl[i].Data[j]);
        }
        printf("\n");
    }
    
}

void TratarFallo(char *MRAM, int ETQ, int linea, int bloque){
    tbl[linea].ETQ = ETQ;
    globaltime += 10;
     
    for(int i = 0; i < TAM_LINEA; i++){
        tbl[linea].Data[i] = MRAM[(bloque+i)];
    }
}