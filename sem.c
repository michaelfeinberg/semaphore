#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>

union semun{
  int val;
  struct semid_ds *buff;
  unsigned long *buffer;
  struct seminfo *_buf;
};
  
  
int main(int argc, char *argv[]){
        int semid;
        int semkey = ftok("makefile" , 22);
        int shmkey = ftok("sem.c", 12);
	int sc;
        if (strncmp(argv[1], "-c", strlen(argv[1])) == 0){
          int md = shmget(shmkey, sizeof(int), IPC_CREAT | IPC_EXCL, 0644);
	  if(md == -1){
	    printf("error : %s\n",strerror(errno));
	  }
	  
	  int fd = open("story", O_CREAT | O_EXCL | O_TRUNC, 0644);
	  semid = semget(key, 1, IPC_CREAT | IPC_EXCL| 0644);
          printf("semaphore created: %d\n", semid);
          union semun su;
          su.val = atoi(argv[2]);
	  sc = semctl(semid, 0, SETVAL, su);
          printf("value set: %d\n", sc);
        }

        else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0){
          semid = semget(key, 1, 0);
	  sc = semctl(semid, 0, IPC_RMID);
	  printf("semaphore removed: %d\n", sc);
	
	  int md = shmget(shmkey,sizeof(int), 0);
	  struct shmid_ds buf;
	  shmctl(md, IPC_RMID, buf);
	  int fd = open("story",O_RDONLY);
	  char *buff;
	  read(fd,buff,1024);
	  printf("%s\n",buff);

	}
        return 0;

}