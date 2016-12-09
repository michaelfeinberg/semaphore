#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

void print_file(char *file) {
    FILE *f = fopen(file, "r");
    if (!f) {
        return;
    }
    char input[256];
    while (fgets(input, sizeof(input), f)) {
        printf("%s", input);
    }
    fclose(f);
}

union semun{
    int val;
    struct semid_ds *buff;
    unsigned long *buffer;
    struct seminfo *_buf;
};


int main(int argc, char *argv[]){
    if (argc < 2) {
        return 1;
    }
    int semkey = ftok("Makefile" , 22);
    int shmkey = ftok("control.c", 12);

    int semid = semget(semkey, 1, IPC_CREAT | 0644);
    if (semid == -1) {
        printf("Error: %s\n",strerror(errno));
        return 1;
    }

    if (strncmp(argv[1], "-c", strlen(argv[1])) == 0) {
        int md = shmget(shmkey, sizeof(int), IPC_CREAT | IPC_EXCL | 0644);
        if (md == -1) {
            printf("Error: %s\n",strerror(errno));
            return 1;
        }

        int fd = open("story", O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            printf("Error: %s\n",strerror(errno));
            return 1;
        }

        union semun message;
        message.val = 1;
        if (semctl(semid, 0, SETVAL, message) == -1) {
            printf("Error: %s\n",strerror(errno));
            return 1;
        }

        close(fd);
        printf("[*] Story created\n");
    } else if (strncmp(argv[1], "-r", strlen(argv[1])) == 0) {
        if (semctl(semid, 0, IPC_RMID) == -1) {
            printf("Error: %s\n",strerror(errno));
            return 1;
        }

        int md = shmget(shmkey,sizeof(int), 0);
        if (md == -1) {
            printf("Error: %s\n",strerror(errno));
            return 1;
        }

        struct shmid_ds buf;
        if (shmctl(md, IPC_RMID, &buf) == -1) {
            printf("Error: %s\n",strerror(errno));
            return 1;
        }
        print_file("story");
        printf("[*] Story removed\n");
    } else if (strncmp(argv[1], "-v", strlen(argv[1])) == 0) {
        print_file("story");
    }
    return 0;
}
