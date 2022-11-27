#define TAM_LINEA 16

typedef struct {
unsigned char ETQ;
unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

int globaltime = 0;
int numfallos = 0;