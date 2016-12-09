#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

union semun{
    int val;
    struct semid_ds *buff;
    unsigned long *buffer;
    struct seminfo *_buf;
};

int main(){
    int semkey = ftok("Makefile" , 22);
    int shmkey = ftok("control.c", 12);

    int semid = semget(semkey, 1, IPC_CREAT | 0644);
    if (semid == -1) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_flg = SEM_UNDO;
    sb.sem_op = -1;
    if (semop(semid, &sb, 1) == -1) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    int md = shmget(shmkey, sizeof(int), 0644);
    if (md == -1) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    int *len = shmat(md, 0, 0);
    if (*len == -1) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    int fd = open("story", O_CREAT | O_RDWR | O_APPEND, 0644);
    if (fd == -1) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    lseek(fd, -(*len), SEEK_END);
    char *buf = (char *) malloc(*len + 1);

    read(fd, buf, *len);
    printf("%s", buf);

    printf("What is the next line? ");
    char input[256];
    fgets(input, 256, stdin);

    write(fd, input, strlen(input));
    close(fd);
    *len = strlen(input);
    shmdt(len);

    sb.sem_op = 1;
    semop(semid, &sb, 1);
}
