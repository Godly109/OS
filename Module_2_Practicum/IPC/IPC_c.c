// 5_pipe_fork.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t child_pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        close(pipefd[1]);
        char buffer[256];
        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("read");
            exit(1);
        }
        buffer[bytes_read] = '\0';
        printf("Child: Received message: %s\n", buffer);
        close(pipefd[0]);
    } else {
        close(pipefd[0]);
        char message[] = "hai, anak sisop 24";
        write(pipefd[1], message, strlen(message));
        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
