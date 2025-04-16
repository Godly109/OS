// 1_processes.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

void create_and_copy_files() {
    // 1. Create directory "halo"
    if (mkdir("halo", 0777) == -1) {
        if (errno != EEXIST) {
            perror("mkdir");
            exit(1);
        }
    }

    // 2. Create empty file "hai.txt" inside "halo"
    char halo_hai_path[256];
    snprintf(halo_hai_path, sizeof(halo_hai_path), "halo/hai.txt");
    int hai_fd = open(halo_hai_path, O_CREAT | O_WRONLY, 0666);
    if (hai_fd == -1) {
        perror("open (create)");
        exit(1);
    }
    close(hai_fd);

    // 3. Copy "hai.txt" to the parent directory
    char hai_copy_path[256];
    snprintf(hai_copy_path, sizeof(hai_copy_path), "hai.txt");
    FILE *src_file = fopen(halo_hai_path, "r");
    if (src_file == NULL) {
        perror("fopen (src)");
        exit(1);
    }
    FILE *dest_file = fopen(hai_copy_path, "w");
    if (dest_file == NULL) {
        perror("fopen (dest)");
        exit(1);
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dest_file);
    }
    fclose(src_file);
    fclose(dest_file);
}

int main() {
    create_and_copy_files();
    printf("Files created and copied.\n");
    return 0;
}
```c
// 2_threads.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *count_thread(void *arg) {
    (void)arg;
    FILE *fp = fopen("count.txt", "w");
    if (fp == NULL) {
        perror("fopen (count.txt)");
        exit(1);
    }
    for (int i = 1; i <= 100; ++i) {
        fprintf(fp, "%d\n", i);
    }
    fclose(fp);
    return (void *)1;
}

void *print_thread(void *arg) {
    (void)arg;
    FILE *fp = fopen("print.txt", "w");
    if (fp == NULL) {
        perror("fopen (print.txt)");
        exit(1);
    }
    fprintf(fp, "Saya pintar mengerjakan thread\n");
    fclose(fp);
    return (void *)2;
}

void *even_thread(void *arg) {
    (void)arg;
    FILE *fp = fopen("count_2.txt", "w");
    if (fp == NULL) {
        perror("fopen (count_2.txt)");
        exit(1);
    }
    for (int i = 2; i <= 100; i += 2) {
        fprintf(fp, "%d\n", i);
    }
    fclose(fp);
    return (void *)3;
}

void run_threads_and_log() {
    pthread_t threads[3];
    void *thread_results[3];
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("fopen (log.txt)");
        exit(1);
    }

    // Create threads
    if (pthread_create(&threads[0], NULL, count_thread, NULL) != 0) {
        perror("pthread_create (count_thread)");
        exit(1);
    }
    if (pthread_create(&threads[1], NULL, print_thread, NULL) != 0) {
        perror("pthread_create (print_thread)");
        exit(1);
    }
    if (pthread_create(&threads[2], NULL, even_thread, NULL) != 0) {
        perror("pthread_create (even_thread)");
        exit(1);
    }

    // Wait for threads to finish and get results
    for (int i = 0; i < 3; ++i) {
        if (pthread_join(threads[i], &thread_results[i]) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }
     // Determine the order and write to log.txt
    int order[3] = {0};
    for (int i = 0; i < 3; i++) {
        if ((long)thread_results[i] == 1) {
            order[0] = 1;
        } else if ((long)thread_results[i] == 2) {
            order[1] = 1;
        } else if ((long)thread_results[i] == 3) {
            order[2] = 1;
        }
    }
    for(int i=0; i<3; i++){
        if(order[i] == 1){
             fprintf(log_file, "1.  Thread %d\n", i+1);
             break;
        }
    }
    fclose(log_file);
}

int main() {
    for (int i = 0; i < 3; ++i) {
        printf("Run %d:\n", i + 1);
        run_threads_and_log();
    }
    printf("Threads executed and logged.\n");
    return 0;
}
```c
// 3_threads_mutexes.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t log_mutex;

void *counting_thread(void *arg) {
    int thread_id = (int)(long)arg;
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("fopen (log.txt)");
        exit(1);
    }

    for (int i = 1; i <= 3; ++i) {
        pthread_mutex_lock(&log_mutex);
        fprintf(log_file, "thread %d count %d\n", thread_id, i);
        pthread_mutex_unlock(&log_mutex);
    }
    fclose(log_file);
    return NULL;
}

void run_threads_with_mutexes() {
    pthread_t threads[5];
    pthread_mutex_init(&log_mutex, NULL);

    // Create threads, passing thread ID as argument
    for (int i = 0; i < 5; ++i) {
        if (pthread_create(&threads[i], NULL, counting_thread, (void *)(long)(i + 1)) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < 5; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    pthread_mutex_destroy(&log_mutex);
}

int main() {
    run_threads_with_mutexes();
    printf("Threads with mutexes executed and logged.\n");
    return 0;
}
```c
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

```c
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
```c
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
```c
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
