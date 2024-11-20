#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

int products = 0;
int sells = 0;

sem_t sem;
pthread_mutex_t mutex;

void* producer(void* arg) {
    while (true) {
        sem_wait(&sem);
        pthread_mutex_lock(&mutex);
        if (sells < products) {
            sells++;
        }
        pthread_mutex_unlock(&mutex);
    }
}

void* consumer(void* arg) {
    while (true) {
        pthread_mutex_lock(&mutex);
        if (products < sells + 1418)
        { //MSSV 23521418
            products++;
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&sem);
    }
}

int main() {
    pthread_t producer_thread, consumer_thread;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 0);

    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    for (int i = 0; i < 5; i++) {
        printf("products is %d and sells is %d\n", products, sells);
        sleep(0.5);
    }
    sem_destroy(&sem);
    pthread_mutex_destroy(&mutex);

    return 0;
}
