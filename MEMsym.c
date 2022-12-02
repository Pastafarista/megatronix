//Antonio Cabrera y Alejandro Gómez 2ºMAIS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define TEXTO_TAM 100
#define TAM_LINEA 16
#define TAM_RAM 4096
#define NUM_FILAS 8
#define ETIQUETA 5
#define LINEA 3
#define PALABRA 4
typedef struct {
    unsigned char etiqueta;
    unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

typedef struct{
    unsigned int addr;
    unsigned int palabra;
    unsigned int bloque;
    unsigned int linea;
    unsigned int etiqueta;
}MAPA_ADDR;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion(unsigned int addr, MAPA_ADDR *datos);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, MAPA_ADDR datos, int *numfallos, int *globaltime, char *texto);
void imprimirCache(char *MRAM, MAPA_ADDR datos, T_CACHE_LINE *tbl, int *numfallos, int *globaltime, char *texto);

unsigned int nextAddr(FILE *fd);              //Obtiene la siguiente línea de accesos_memoria.txt
unsigned int getetiqueta(unsigned int addr);  //Obtiene la etiqueta de una dirección en decimal
unsigned int getLinea(unsigned int addr);     //Obtiene el número de línea de una dirección
unsigned int getBloque(unsigned int addr);    //Obtiene el bloque de una dirección
unsigned int getPalabra(unsigned int addr);   //Obtiene la palabra de una dirección

int main(int argc, char** argv){
   
    unsigned char Simul_RAM[TAM_RAM];
    unsigned int accesos_memoria[TAM_LINEA];
    char texto[TEXTO_TAM];
    FILE *fd;
      
    T_CACHE_LINE tbl[NUM_FILAS];    
    MAPA_ADDR datos[NUM_FILAS]; 

    int globaltime = 0;
    int numfallos = 0;

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

    int accesos = 0;

    for(int i = 0; i < TAM_LINEA; i++){
        ParsearDireccion(nextAddr(fd), &datos[i]);
        if( datos[i].addr != 0){
            accesos++;
            imprimirCache(Simul_RAM, datos[i], tbl , &numfallos, &globaltime, texto);
            sleep(1);
        }
    }   
    printf("Accesos totales: %i; fallos: %i; Tiempo: %f\nTexto leído: %s", accesos, numfallos, (float)globaltime/(float)accesos, texto);
    for(int i = 0; i < TEXTO_TAM; i++){
        printf("%c", texto[i]);
    }
    fclose(fd);
    VolcarCACHE(tbl);
}

unsigned int nextAddr(FILE *fd){  //Devuelve la siguiente dirección a leer
    char linea[5];
    if(fgets(linea, sizeof(linea), fd) == NULL){
        return 0;
    }
    return strtol(linea, NULL, 16);
}

unsigned int getetiqueta(unsigned int addr){
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
    ret = (getetiqueta(addr) << LINEA) + getLinea(addr); //Bloque = Etiqueta + Linea
    return ret;
}

unsigned int getPalabra(unsigned int addr){
    int ret;
    int mascara = (((TAM_RAM - 1) << PALABRA) ^ (TAM_RAM - 1)); //000000001111 en el caso de la palabra = 4
    ret = addr & mascara;
    return ret;
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
    for(int i = 0; i < NUM_FILAS; i++){
        tbl[i].etiqueta = 0xFF;         
        for(int k = 0; k < TAM_LINEA; k++){
            tbl[i].Data[k] = 0x23;   
        }
    }
}

void ParsearDireccion(unsigned int addr, MAPA_ADDR *datos){
    datos->addr = addr;
    datos->palabra = getPalabra(addr);
    datos->bloque = getBloque(addr);
    datos->linea = getLinea(addr);
    datos->etiqueta = getetiqueta(addr);
}

void imprimirCache(char *MRAM, MAPA_ADDR datos, T_CACHE_LINE *tbl, int *numfallos, int *globaltime, char *texto){
    *globaltime += 1;
    if(datos.etiqueta != tbl[datos.linea].etiqueta){
        *numfallos = *numfallos + 1;
        printf("T: %i, Fallo de CACHÉ %d, ADDR: %04X, Etiqueta: %X, Linea: %02X, Palabra: %02X, Bloque:%02X\n", *globaltime, *numfallos, datos.addr, datos.etiqueta, datos.linea, datos.palabra, datos.bloque);
        printf("Cargando el Bloque: %02X en Linea: %02X\n", datos.bloque, datos.linea);
        TratarFallo(tbl, MRAM, datos, numfallos, globaltime, texto);
    }
    else{
        printf("T: %d, Acierto de CACHE, ADDR %04X Label %X linea %02X palabra %02X DATO %02X", *globaltime,datos.addr, datos.etiqueta, datos.linea, datos.palabra, datos.bloque);
        printf("\n");
        for(int i = 0; i < NUM_FILAS; i++){
            printf("etiqueta %X | Data", tbl[i].etiqueta);
            for(int j = 0; j < TAM_LINEA; j++){
                printf(" %X ", tbl[i].Data[j]);
                texto[TAM_LINEA * i + j] = tbl[i].Data[j];
            }
            printf("\n");
        }
    }
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, MAPA_ADDR datos, int *numfallos, int *globaltime, char *texto){
    tbl[datos.linea].etiqueta = datos.etiqueta;
    *globaltime = *globaltime + 9; //9 + 1 al 
    for(int i = 0; i < TAM_LINEA; i++){
        tbl[datos.linea].Data[i] = MRAM[(datos.bloque + i)];
    }
    imprimirCache(MRAM, datos, tbl, numfallos, globaltime, texto);
}

void VolcarCACHE(T_CACHE_LINE *tbl){
    FILE *fd;
    fd = fopen ("CONTENTS_CACHE.bin", "w");
    for(int i = 0; i < NUM_FILAS; i++){
        fwrite(tbl[i].Data, sizeof(tbl[i].Data), TAM_LINEA, fd);
    }
    fclose(fd);
}