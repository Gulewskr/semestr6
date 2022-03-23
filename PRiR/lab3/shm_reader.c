#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // ftok to generate unique key
    key_t key = ftok("shm_writer.c", 65);
  
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
  
    printf("Data read from memory:\n");
    int i;
    for(i=0; i<10; i++)
        printf("%d\n", tab[i]); 
      
    //detach from shared memory 
    if(shmdt(tab) < 0)
    {
        fprintf(stderr, "shmdt failed");
        exit(1);
    }
    
    // destroy the shared memory
    if(shmctl(shmid,IPC_RMID,NULL) < 0)
    {
        fprintf(stderr, "smctl failed");
        exit(1);
    }
     
    return 0;
}