#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>

union semun{
  int val;
  struct semid_ds *buff;
  unsigned long *buffer;
  struct seminfo *_buf;
};

int main(){
  int semkey = ftok("Makefile" , 22);
  int shmkey = ftok("control.c", 12);

  int semid = semget(semkey, 1, 0);
  union semun data;
  semctl(semid, 0, -1, data);

  int md = shmget(shmkey, sizeof(int), 0);
  int p;
  shmat(md, &p, 0);

  int fd = open("story", O_RDWR | O_APPEND);
  char line[256];
  read(fd, line, p);
  printf("%s\n", line);

  printf("What is the next line? ");
  char *input;
  fgets(input, 256, stdin);

  write(fd, input, sizeof(input));
  close(fd);
  p = sizeof(input);
  shmdt(&p);

}
