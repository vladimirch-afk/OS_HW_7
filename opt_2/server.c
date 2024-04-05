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
int client_pid;
int shm_id;
char memory[] = "memory";

// Завершение работы
void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("Shutting down...\n");
        kill(client_pid, SIGINT);
    }

    if(shm_id != -1) {
        shm_unlink(memory);
        close(shm_id);
    }

    exit(0);
}

int main() {
    printf("Server PID: %d\n", getpid());
    printf("Client PID: ");
    scanf("%d", &client_pid);  // Получить PID клиента от пользователя
    // Пайп для сообщения об отключении
    signal(SIGINT, signal_handler);

    // Открыть объект памяти
    if ((shm_id = shm_open(memory, O_RDWR, 0666)) == -1 ) {
        printf("Opening error\n");
        perror("shm_open");
        return 1;
    } else {
        printf("name = %s, id = 0x%x\n", memory, shm_id);
    }

    // Доступ к памяти
    Data *shared_mem = (Data *)mmap(NULL, sizeof(Data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shared_mem == MAP_FAILED) {
        printf("Error getting shared memory\n");
        return 1;
    }

    while (1) {
        sleep(1); // Подождать 1 секунду перед чтением числа
        printf("Received: %d\n", shared_mem->number);
    }
    return 0;
}
