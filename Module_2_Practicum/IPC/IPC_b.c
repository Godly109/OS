// 4_2_message_queue.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 256

struct msg_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
};

int main() {
    key_t key = ftok("4_2_message_queue.c", 66);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    struct msg_buffer message;
    message.msg_type = 1;

    printf("Enter message to send: ");
    fgets(message.msg_text, MAX_TEXT, stdin);
    message.msg_text[strcspn(message.msg_text, "\n")] = '\0';

    if (msgsnd(msgid, &message, sizeof(message), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Sent message: %s\n", message.msg_text);

    struct msg_buffer received_message;
    if (msgrcv(msgid, &received_message, sizeof(received_message), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("Received message: %s\n", received_message.msg_text);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    return 0;
}