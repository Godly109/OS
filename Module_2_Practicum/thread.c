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
    FILE *threads_log = fopen("threads_log.txt", "a");
    if (threads_log == NULL) {
        perror("fopen (threads_log.txt)");
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
     // Determine the order and write to threads_log.txt
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
             fprintf(threads_log, "1.  Thread %d\n", i+1);
             break;
        }
    }
    fclose(threads_log);
}

int main() {
    for (int i = 0; i < 3; ++i) {
        printf("Run %d:\n", i + 1);
        run_threads_and_log();
    }
    printf("Threads executed and logged.\n");
    return 0;
}