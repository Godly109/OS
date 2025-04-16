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