#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>


#define RING_SIZE 15            // Tamanho do buffer circular partilhado
#define FINAL_SIZE 10000        // Tamanho do buffer final
#define START_NUM 10001         // Número inicial a produzir
#define END_NUM 20000           // Número final a produzir

// Buffers e variáveis globais
int ring_buffer[RING_SIZE];      // Buffer circular para comunicação entre produtores e consumidores
int cell_status[RING_SIZE];      // Estado das células do ring_buffer: 0 = livre, 1 = ocupada
int final_buffer[FINAL_SIZE];    // Buffer final para armazenar os valores processados
int final_count = 0;             // Contador de elementos inseridos no final_buffer
int next_prod = START_NUM;       // Próximo número a ser produzido
int next_cons = START_NUM;       // Próximo número a ser consumido

// Mutex e variáveis de condição para sincronização
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;   // Sinaliza que buffer não está cheio (produtores podem produzir)
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;  // Sinaliza que buffer não está vazio (consumidores podem consumir)

// Contadores individuais de produção e consumo por thread
int prod_count[4] = {0};
int cons_count[2] = {0};

// Função de comparação para ordenação do buffer final com qsort
int compara(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// Thread produtora: produz números crescentes e os insere no ring_buffer
void *produtor(void *arg) {
    int id = *(int *)arg;

    while (1) {
        pthread_mutex_lock(&mutex);

        if (next_prod > END_NUM) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        int i = next_prod++;
        int index = (i - 1) % RING_SIZE;

        while (cell_status[index] == 1)
            pthread_cond_wait(&not_full, &mutex);

        ring_buffer[index] = i;
        cell_status[index] = 1;
        prod_count[id]++;

        //--------------------------debug 2---------------------------------         
        printf("P[%p]\n", (void *)arg);         
        fflush(stdout);         
        for (int jj = 0; jj < 15; jj++) {               
            printf("|%d", ring_buffer[jj]);   
            fflush(stdout);
        }		
        printf("\n>>>>>>>\n"); 
        fflush(stdout);
        //------------------------------------------------------------- 	

        pthread_cond_broadcast(&not_empty);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

// Thread consumidora: consome números do ring_buffer, aplica módulo e armazena no final_buffer
void *consumidor(void *arg) {
    int id = *(int *)arg;

    while (1) {
        pthread_mutex_lock(&mutex);

        if (next_cons > END_NUM) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        int i = next_cons++;
        int index = (i - 1) % RING_SIZE;

        while (cell_status[index] == 0)
            pthread_cond_wait(&not_empty, &mutex);

        int val = ring_buffer[index] % 10000;
        ring_buffer[index] = -1;  // <- alteração para debug
        cell_status[index] = 0;

        //--------------------------debug 3---------------------------------         
        printf("C[%p]\n", (void *)arg);       
        fflush(stdout);         
        for (int zz = 0; zz < 15; zz++) {                
            printf("|%d", ring_buffer[zz]);  
            fflush(stdout);
        }
        printf("\n.......\n"); 
        fflush(stdout);
        //------------------------------------------------------------- 

        final_buffer[final_count++] = val;
        cons_count[id]++;

        pthread_cond_broadcast(&not_full);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t produtores[4], consumidores[2];
    int prod_ids[4] = {0, 1, 2, 3};
    int cons_ids[2] = {0, 1};

    for (int i = 0; i < RING_SIZE; i++) {
        ring_buffer[i] = 0;
        cell_status[i] = 0;
    }

    for (int i = 0; i < 4; i++)
        pthread_create(&produtores[i], NULL, produtor, &prod_ids[i]);
    for (int i = 0; i < 2; i++)
        pthread_create(&consumidores[i], NULL, consumidor, &cons_ids[i]);

    for (int i = 0; i < 4; i++)
        pthread_join(produtores[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(consumidores[i], NULL);

    qsort(final_buffer, FINAL_SIZE, sizeof(int), compara);

    // Não mostrar final_buffer em debug, como sugerido
    /*
    for (int i = 0; i < FINAL_SIZE; i++) {
        printf("%d ", final_buffer[i]);
    }
    */

    printf("\n\n--- Contagem de Produções ---\n");
    printf("Produtor A: %d\n", prod_count[0]);
    printf("Produtor B: %d\n", prod_count[1]);
    printf("Produtor C: %d\n", prod_count[2]);
    printf("Produtor D: %d\n", prod_count[3]);

    printf("\n--- Contagem de Consumos ---\n");
    printf("Consumidor A: %d\n", cons_count[0]);
    printf("Consumidor B: %d\n", cons_count[1]);

    return 0;
}
