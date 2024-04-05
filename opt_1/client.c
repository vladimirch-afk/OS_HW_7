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

// для завершения работы
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

    if ((shm_id = shm_open(memory, O_CREAT|O_RDWR, 0666)) == -1) {
        perror("shm_open");
        return 1;
    } else {
        printf("name = %s, id = 0x%x\n", memory, shm_id);
    }

    // Задание размера объекта памяти
    if (ftruncate(shm_id, sizeof(Package)) == -1) {
        perror("ftruncate");
        return 1;
    } else {
        printf("Memory size set and = %lu\n", sizeof(Package));
    }
    shared_d = (Package *)mmap(NULL, sizeof(Package), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shared_d == MAP_FAILED) {
        printf("Error getting shared memory\n");
        return 1;
    }

    // Генератора случайных чисел
    srand(getpid());

    while (1) {
        sleep(1); // Подождать 1 секунду перед генерацией числа
        shared_d->number = rand() % 100; // Генерация числа от 0 до 99
        printf("Sent: %d\n", shared_d->number);
        // Завершаем работу, если флаг равен 1
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
