#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

typedef struct {
    int number;
} Data;

int serv_pid;
int shm_id;
char memory[] = "memory"; // Объект памяти

// Отключение
void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("Shutting down...\n");
        kill(serv_pid, SIGINT);
    }

    if(shm_id != -1) {
        shm_unlink(memory);
        close(shm_id);
    }

    exit(0);
}

int main() {
    printf("Client (generation) PID: %d\n", getpid());
    printf("Server (printing) PID: ");
    scanf("%d", &serv_pid);  // Получить PID сервера от пользователя
    // Пайп для сообщения об отключении
    signal(SIGINT, signal_handler);

    // Открываем объект памяти
    if ((shm_id = shm_open(memory, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open");
        return 1;
    } else {
        printf("name = %s, id = 0x%x\n", memory, shm_id);
    }

    // Задание размера объекта памяти
    if (ftruncate(shm_id, sizeof(Data)) == -1) {
        perror("ftruncate");
        return 1;
    } else {
        printf("Memory size = %lu\n", sizeof(Data));
    }
    Data *shared_mem = (Data *)mmap(NULL, sizeof(Data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shared_mem == MAP_FAILED) {
        printf("Error getting shared memory\n");
        return 1;
    }
    // Генератор случайных чисел
    srand(getpid());
    while (1) {
        sleep(1); // Подождать 1 секунду перед генерацией числа
        shared_mem->number = rand() % 100; // Генерация числа от 0 до 99
        printf("Sent: %d\n", shared_mem->number);
    }
    return 0;
}
