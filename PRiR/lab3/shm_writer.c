#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    // ftok to generate unique key
    key_t key = ftok("shm_writer.c",65);
    
    // shmget returns an identifier in shmid
    int shmid;
    if((shmid = shmget(key, sizeof(int)*10,0666|IPC_CREAT)) == -1)
    {
        fprintf(stderr, "shmget failed");
        exit(1);
    } 
  
    // shmat to attach to shared memory
    int *tab;
    if((tab = (int*) shmat(shmid,(void*)0,0)) < 0)
    {
        fprintf(stderr, "shmat failed");
        exit(1);
    }
  
    int i;
    for(i=0; i<10; i++)
        tab[i] = i;

    printf("Data written in memory are in range: [%d .. %d]\n",tab[0], tab[9]);
      
    //detach from shared memory 
    if(shmdt(tab) < 0)
    {
        fprintf(stderr, "shmdt failed");
        exit(1);
    }
  
    return 0;
}