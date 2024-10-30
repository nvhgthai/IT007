/*######################################
# University of Information Technology
# IT007 Operating System
#
# Nguyen Van Hong Thai, 23521418
# File: collatz.c
######################################*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 4096 // Kích thước của shared memory

void collatz_sequence(int n, char* buffer) {
    int pos = 0;
    while (n != 1) {
        pos += sprintf(buffer + pos, "%d, ", n);
        if (n % 2 == 0)
            n /= 2;
        else
            n = 3 * n + 1;
    }
    sprintf(buffer + pos, "1");  // Kết thúc chuỗi bằng 1
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Sử dụng: %s <số nguyên dương>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Lỗi: Đầu vào phải là một số nguyên dương.\n");
        return 1;
    }

    const char *name = "collatz_shared_memory";
    int fd;
    char *buffer;

    // Tạo và ánh xạ shared memory
    fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    buffer = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, fd, 0);

    pid_t pid = fork();

    if (pid < 0) {
        perror("Lỗi: Không thể tạo tiến trình con.\n");
        return 1;
    }
    else if (pid == 0) {
        // Tiến trình con tính toán chuỗi Collatz và ghi vào buffer
        collatz_sequence(n, buffer);
        munmap(buffer, SIZE);
        close(fd);
        exit(0);
    }
    else {
        // Tiến trình cha chờ tiến trình con hoàn tất
        wait(NULL);
        // Đọc và in chuỗi Collatz từ buffer
        printf("Chuỗi Collatz: %s\n", buffer);

        // Đóng và hủy shared memory
        munmap(buffer, SIZE);
        close(fd);
        shm_unlink(name);
    }

    return 0;
}
