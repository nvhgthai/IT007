#include <stdio.h>
#include <pthread.h>

// Biến toàn cục
int x1 = 2, x2 = 3, x3 = 4, x4 = 5, x5 = 6, x6 = 7;
int w, v, y, z, ans;

// Mutex và condition variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_v = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_yz = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_ans = PTHREAD_COND_INITIALIZER;

// Cờ trạng thái
int is_v_ready = 0;
int is_y_ready = 0, is_z_ready = 0;

// Hàm mô phỏng từng lệnh
void* calculate_w(void* arg) {
    pthread_mutex_lock(&lock);
    w = x1 * x2;
    printf("Calculated w = %d\n", w);
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* calculate_v(void* arg) {
    pthread_mutex_lock(&lock);
    v = x3 * x4;
    printf("Calculated v = %d\n", v);
    is_v_ready = 1;
    pthread_cond_broadcast(&cond_v); // Báo hiệu rằng v đã sẵn sàng
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* calculate_y(void* arg) {
    pthread_mutex_lock(&lock);
    while (!is_v_ready) {
        pthread_cond_wait(&cond_v, &lock); // Chờ v sẵn sàng
    }
    y = v * x5;
    printf("Calculated y (step c) = %d\n", y);
    is_y_ready = 1;
    pthread_cond_broadcast(&cond_yz); // Báo hiệu rằng y đã sẵn sàng
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* calculate_z(void* arg) {
    pthread_mutex_lock(&lock);
    while (!is_v_ready) {
        pthread_cond_wait(&cond_v, &lock); // Chờ v sẵn sàng
    }
    z = v * x6;
    printf("Calculated z (step d) = %d\n", z);
    is_z_ready = 1;
    pthread_cond_broadcast(&cond_yz); // Báo hiệu rằng z đã sẵn sàng
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* calculate_y_step_e(void* arg) {
    pthread_mutex_lock(&lock);
    while (!is_y_ready || w == 0) {
        pthread_cond_wait(&cond_yz, &lock); // Chờ y và w sẵn sàng
    }
    y = w * y;
    printf("Updated y (step e) = %d\n", y);
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* calculate_z_step_f(void* arg) {
    pthread_mutex_lock(&lock);
    while (!is_z_ready || w == 0) {
        pthread_cond_wait(&cond_yz, &lock); // Chờ z và w sẵn sàng
    }
    z = w * z;
    printf("Updated z (step f) = %d\n", z);
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* calculate_ans(void* arg) {
    pthread_mutex_lock(&lock);
    while (!is_y_ready || !is_z_ready) {
        pthread_cond_wait(&cond_ans, &lock); // Chờ y và z sẵn sàng
    }
    ans = y + z;
    printf("Final ans = %d\n", ans);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t threads[7];

    // Tạo các thread
    pthread_create(&threads[0], NULL, calculate_w, NULL);
    pthread_create(&threads[1], NULL, calculate_v, NULL);
    pthread_create(&threads[2], NULL, calculate_y, NULL);
    pthread_create(&threads[3], NULL, calculate_z, NULL);
    pthread_create(&threads[4], NULL, calculate_y_step_e, NULL);
    pthread_create(&threads[5], NULL, calculate_z_step_f, NULL);
    pthread_create(&threads[6], NULL, calculate_ans, NULL);

    // Chờ các thread kết thúc
    for (int i = 0; i < 7; i++) {
        pthread_join(threads[i], NULL);
    }

    // Hủy mutex và condition variables
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond_v);
    pthread_cond_destroy(&cond_yz);
    pthread_cond_destroy(&cond_ans);

    return 0;
}
