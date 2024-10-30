/*######################################
# University of Information Technology
# IT007 Operating System
#
# Nguyen Van Hong Thai, 23521418
# File: producer_consumer.c
######################################*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#define SIZE 10  // Kích thước của bounded-buffer

typedef struct {
    int buffer[SIZE];  // Bounded-buffer có độ lớn 10 bytes
    int in;            // Vị trí ghi của Producer
    int out;           // Vị trí đọc của Consumer
    int total;         // Tổng của các số trong buffer
    int flag;          // Cờ để đồng bộ hóa giữa Producer và Consumer
} SharedMemory;

int main() {
    const char *name = "OS_Buffer";
    int fd;
    SharedMemory *shm;

    // Tạo shared memory
    fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(SharedMemory));
    shm = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Khởi tạo giá trị ban đầu cho shared memory
    shm->in = 0;
    shm->out = 0;
    shm->total = 0;
    shm->flag = 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }
    else if (pid == 0) {
        // Consumer process
        while (1) {
            // Kiểm tra cờ để đảm bảo có dữ liệu để đọc
            if (shm->flag == 1) {
                int data = shm->buffer[shm->out];  // Đọc dữ liệu từ buffer
                shm->out = (shm->out + 1) % SIZE;
                shm->total += data;

                printf("Consumer read: %d, Total: %d\n", data, shm->total);

                // Kiểm tra nếu tổng đã vượt quá 100
                if (shm->total > 100) {
                    break;
                }

                shm->flag = 0;  // Đánh dấu buffer đã được đọc
            }
        }
        // Đóng và huỷ shared memory
        munmap(shm, sizeof(SharedMemory));
        close(fd);
        shm_unlink(name);
    }
    else {
        // Producer process
        srand(time(NULL));
        while (1) {
            // Kiểm tra cờ để đảm bảo buffer trống
            if (shm->flag == 0) {
                int num = (rand() % 11) + 10;  // Sinh số ngẫu nhiên trong khoảng [10, 20]
                shm->buffer[shm->in] = num;  // Ghi số vào buffer
                shm->in = (shm->in + 1) % SIZE;

                printf("Producer wrote: %d\n", num);

                shm->flag = 1;  // Đánh dấu buffer đã có dữ liệu

                // Kiểm tra nếu tổng vượt quá 100 để dừng
                if (shm->total > 100) {
                    break;
                }
            }
        }
        // Chờ Consumer kết thúc
        wait(NULL);
        // Đóng và huỷ shared memory
        munmap(shm, sizeof(SharedMemory));
        close(fd);
        shm_unlink(name);
    }

    return 0;
}
