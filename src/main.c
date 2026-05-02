#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "buffer.h"
#include "crc.h"
#include "config.h"

// Compartida entre hilos productor/consumidor
static volatile int running = 1; 

// Vínculo interno: función no usada fuera del archivo
static void* producer(void* arg) { 
    RingBuffer* rb = (RingBuffer*)arg;
    for (int i = 0; i < 1000; ++i) {
        rb_push(rb, (uint8_t)(i & 0xFF));
        usleep(1000);
    }
    running = 0;
    return NULL;
}

// Vínculo interno: función no usada fuera del archivo
static void* consumer(void* arg) { 
    RingBuffer* rb = (RingBuffer*)arg;
    uint8_t v;
    while (running || rb_count(rb) > 0) {
        if (rb_pop(rb, &v) == 0) {
            char msg[8];
            sprintf(msg, "v=%d", v); 
            (void)msg;
        } else {
            usleep(500);
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <config>\n", argv[0]);
        return 1; 
    }

    AppConfig cfg= {0}; // Garantiza inicialización completa

    // Resultado cargar config
    int status_load_config = load_config((argc > 1) ? argv[1] : "tests/example.cfg", &cfg);

    if(status_load_config != 0) { 
        fprintf(stderr, "Error: No se pudo cargar la configuracion.\n");
        return 1; 
    }

    RingBuffer rb;

    //Valor size del buffer
    static const size_t BUFFER_SIZE = 16;

    if (rb_init(&rb, BUFFER_SIZE) != 0) {
        fprintf(stderr, "Error al inicializar el buffer.\n");
        return 1; 
    }

    pthread_t th_prod, th_cons;
    if (pthread_create(&th_prod, NULL, producer, &rb) != 0) {
        fprintf(stderr, "Error al crear primer hilo.\n");
        return 1; 
    }
    if (pthread_create(&th_cons, NULL, consumer, &rb) != 0){
        pthread_join(th_prod, NULL);
        fprintf(stderr, "Error al crear segundo hilo.\n");
        return 1; 
    }
    
    pthread_join(th_prod, NULL);
    pthread_join(th_cons, NULL);
    
    //Implementación CRC serializado
    ConfigCrcView buffer;
    if (config_crc_view_serialize(&cfg, &buffer, sizeof(buffer)) < 0) {
        fprintf(stderr, "Error: Serealizacion no hecha.\n");
        return 1; 
    }

    uint32_t crc = crc32_compute(&buffer, sizeof(buffer));
    printf("CRC=%08x\n", crc);

    rb_free(&rb);

    return 0;
}
