// 4_1_shared_memory_sender.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main() {
    key_t key = ftok("4_1_shared_memory_sender.c", 65);
    if (key == -1) {
        perror("ftok (sender)");
        exit(1);
    }

    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget (sender)");
        exit(1);
    }

    char *str = (char *)shmat(shmid, NULL, 0);
    if (str == (void *)-1) {
        perror("shmat (sender)");
        exit(1);
    }

    strcpy(str, "aku lagi belajar ipc");
    printf("Sender: Data written to memory: %s\n", str);

    if (shmdt(str) == -1) {
        perror("shmdt (sender)");
        exit(1);
    }

    return 0;
}