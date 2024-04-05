#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

typedef struct {
    int number;
    int flag;
} Package;

Package *shared_d;
int shm_id;
char memory[] = "memory";

// Для завершения работы
void signal_handler(int signal) {
    if (signal == SIGINT) {
        shared_d->flag = 1;
    }

    if(shm_id != -1) {
        shm_unlink(memory);
        close(shm_id);
    }
}

int main() {
    signal(SIGINT, signal_handler);

    // Открыть объект памяти
    if ((shm_id = shm_open(memory, O_RDWR, 0666)) == -1 ) {
        printf("Opening error\n");
        perror("shm_open");
        return 1;
    } else {
        printf("name = %s, id = 0x%x\n", memory, shm_id);
    }

    // Получить доступ к памяти
    shared_d = (Package *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shared_d->number == (int)-1 ) {
        printf("Error getting shared memory\n");
        return 1;
    }

    // Получаем сообщения
    while (1) {
        sleep(1); // Подождать 1 секунду перед чтением числа
        printf("Received: %d\n", shared_d->number);

        // Завершить работу, если флаг равен 1
        if (shared_d->flag == 1) {
            if(shm_id != -1) {
                shm_unlink(memory);
                close(shm_id);
            }
            exit(0);
        }
    }
    return 0;
}
