#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int x = 0; // Biến dùng chung
pthread_mutex_t lock; // Mutex để đồng bộ hóa

void* processA(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock); // Khóa trước khi truy cập
        x = x + 1;
        if (x == 20)
            x = 0;
        printf("Process A: x = %d\n", x);
        pthread_mutex_unlock(&lock); // Mở khóa sau khi truy cập
        usleep(100000); // Dừng 100ms
    }
    return NULL;
}

void* processB(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock); // Khóa trước khi truy cập
        x = x + 1;
        if (x == 20)
            x = 0;
        printf("Process B: x = %d\n", x);
        pthread_mutex_unlock(&lock); // Mở khóa sau khi truy cập
        usleep(100000); // Dừng 100ms
    }
    return NULL;
}

int main() {
    pthread_t threadA, threadB;

    // Khởi tạo mutex
    pthread_mutex_init(&lock, NULL);

    // Tạo thread cho hai tiến trình
    pthread_create(&threadA, NULL, processA, NULL);
    pthread_create(&threadB, NULL, processB, NULL);

    // Chờ hai thread kết thúc (không xảy ra trong chương trình này)
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    // Hủy mutex
    pthread_mutex_destroy(&lock);

    return 0;
}

