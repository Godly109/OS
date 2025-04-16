// 3_threads_mutexes.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t log_mutex;

void *counting_thread(void *arg) {
    int thread_id = (int)(long)arg;
    FILE *log_file = fopen("mutex_log.txt", "a");
    if (log_file == NULL) {
        perror("fopen (mutex_log.txt)");
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