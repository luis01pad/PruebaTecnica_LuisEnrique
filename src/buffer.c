#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

int rb_init(RingBuffer* rb, size_t size) {
    if (rb == NULL || size == 0) {
        return -1;
    }
    rb->data = (uint8_t*)malloc(size);
    rb->size = (unsigned)size;
    rb->head = 0U;
    rb->tail = 0U; 
    if (rb->data == NULL){
        return -1;    
    }
    pthread_mutex_init(&rb->mutex,NULL);
    memset(rb->data, 0, size);
    return 0;
}

void rb_free(RingBuffer* rb) {
    if (rb == NULL){
        return;
    }
    if (rb->data != NULL) {
        free(rb->data);
        rb->data = NULL;
    }
    pthread_mutex_destroy(&rb->mutex);
}

static unsigned next(unsigned v, unsigned mod) {
    return (unsigned)((v + 1) % mod);
}

int rb_push(RingBuffer* rb, uint8_t value) {
    if (rb == NULL) {
        return -1;
    }
    if (rb->data == NULL) {
        return -1;
    }
    pthread_mutex_lock(&rb->mutex); 
    unsigned nhead = next(rb->head, (unsigned)rb->size);
    if (nhead == rb->tail) {
        pthread_mutex_unlock(&rb->mutex);
        return -1;
    }
    rb->data[rb->head] = value;
    rb->head = nhead;
    pthread_mutex_unlock(&rb->mutex);
    return 0;
}

int rb_pop(RingBuffer* rb, uint8_t* out) {
    if (rb == NULL) {
        return -1;
    }
    if (out == NULL) {
        return -1;
    }
    pthread_mutex_lock(&rb->mutex); 
    if (rb->head == rb->tail) {
        pthread_mutex_unlock(&rb->mutex);
        return -1;
    }
    *out = rb->data[rb->tail];
    rb->tail = next(rb->tail, (unsigned)rb->size);
    pthread_mutex_unlock(&rb->mutex);
    return 0;
}

int rb_count(const RingBuffer* rb) {
    if (rb == NULL) {
        return 0;
    }
    pthread_mutex_lock(&rb->mutex); 
    int diff = (int)rb->head - (int)rb->tail;
    if (diff < 0) {
        diff += (int)rb->size;
    }
    pthread_mutex_unlock(&rb->mutex);
    return diff;
}
