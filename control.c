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
        return 0;
    }
    int semid;
    int semkey = ftok("Makefile" , 22);
    int shmkey = ftok("control.c", 12);
    int sc;
    if (strncmp(argv[1], "-c", strlen(argv[1])) == 0){
        int md = shmget(shmkey, sizeof(int), IPC_CREAT | IPC_EXCL | 0644);
        if(md == -1){
            printf("error : %s\n",strerror(errno));
        }

        int fd = open("story", O_CREAT | O_EXCL | O_TRUNC, 0644);
        semid = semget(semkey, 1, IPC_CREAT | IPC_EXCL| 0644);
        printf("semaphore created: %d\n", semid);
        union semun message;
        message.val = 1;
        semctl(semkey, 0, IPC_SET, message);
        close(fd);
    }
    else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0){
        semctl(semkey, 0, IPC_RMID);
        semid = semget(semkey, 1, 0);
        sc = semctl(semid, 0, IPC_RMID);
        printf("semaphore removed: %d\n", sc);

        int md = shmget(shmkey,sizeof(int), 0);
        struct shmid_ds buf;
        shmctl(md, IPC_RMID, &buf);
        print_file("story");
    } else if (strncmp(argv[1], "-v", strlen(argv[1])) == 0) {
        print_file("story");
    }
    return 0;

}
