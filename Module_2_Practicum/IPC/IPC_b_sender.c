// 4_1_shared_memory_receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    key_t key = ftok("4_1_shared_memory_sender.c", 65);
    if (key == -1) {
        perror("ftok (receiver)");
        exit(1);
    }

    int shmid = shmget(key, 1024, 0666);
    if (shmid == -1) {
        perror("shmget (receiver)");
        exit(1);
    }

    char *str = (char *)shmat(shmid, NULL, 0);
    if (str == (void *)-1) {
        perror("shmat (receiver)");
        exit(1);
    }

    printf("Receiver: Data read from memory: %s\n", str);

    if (shmdt(str) == -1) {
        perror("shmdt (receiver)");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl (receiver)");
        exit(1);
    }
    return 0;
}