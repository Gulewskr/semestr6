#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> /*deklaracje standardowych funkcji uniksowych (fork(), write() itd.)*/
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define BUFFOR_SIZE 80

char * out_file;

int numOfChildren;
int id;
int n;
int count;

int *tab_range;
double *tab_sum;
double *vector;

int shmid_range;
int shmid_sum;
int shmid_vector;

void generate(int count, int lower, int upper, char * fileName){

    FILE *out = fopen(fileName, "w"); 
    
    int i;
    double sum = 0.0f;

    if(out == NULL)
    {
        fprintf(stderr, "nie mogę pisać do pliku\n");
    }

    srand(time(0));
    fprintf(out, "%d\n", count);

    for (i = 0; i < count; i++) {
        double num = ((double)rand() /  RAND_MAX) * (upper - lower) + lower;
        sum += num;
        fprintf(out, "%f\n", num);
    }

    fclose(out);
}

void putChildToSleep(){
    while(1){
        pause();
    }
}

void on_usr1(int signal) {
    tab_sum[id] = vector[id];
    exit(0);
}

double sum(double* vector, int n) {
	int i;
	double sum = 0.0f;
	for(i=0; i<n; i++) {
		sum += vector[i];
	}
	return sum;
}

void init_memory(){
   key_t key_range = ftok("shm_writer_range.c",1);
    key_t key_sum = ftok("shm_writer_sum.c",2);
    key_t key_vector = ftok("shm_writer_vector.c",3);

    if((shmid_range = shmget(key_range, sizeof(int)*numOfChildren,0666|IPC_CREAT)) == -1)
    {
        printf("shmget range failed\n");
        exit(1);
    } 
    if((shmid_sum = shmget(key_sum, sizeof(double)*numOfChildren,0666|IPC_CREAT)) == -1)
    {
        printf("shmget sum failed\n");
        exit(1);
    } 
    if((shmid_vector = shmget(key_vector, sizeof(double)*count,0666|IPC_CREAT)) == -1)
    {
        printf("shmget vector failed\n");
        exit(1);
    } 
    if((tab_range = (int*) shmat(shmid_range,(void*)0,0)) < 0)
    {
        printf("shmat range failed");
        exit(1);
    }
    if((tab_sum = (double*) shmat(shmid_sum,(void*)0,0)) < 0)
    {
        printf("shmat sum failed");
        exit(1);
    }
    if((vector= (double*) shmat(shmid_vector,(void*)0,0)) < 0)
    {
        printf("shmat vector failed");
        exit(1);
    }
}

void end_memory(){
    if(shmdt(tab_range) < 0)
    {
        fprintf(stderr, "shmdt range failed");
        exit(1);
    }
    if(shmdt(tab_sum) < 0)
    {
        fprintf(stderr, "shmdt sum failed");
        exit(1);
    }
    if(shmdt(vector) < 0)
    {
        fprintf(stderr, "shmdt sum failed");
        exit(1);
    }

    if(shmctl(shmid_range,IPC_RMID,NULL) < 0)
    {
        fprintf(stderr, "smctl range failed");
        exit(1);
    }
    if(shmctl(shmid_sum,IPC_RMID,NULL) < 0)
    {
        fprintf(stderr, "smctl sum failed");
        exit(1);
    }
    if(shmctl(shmid_vector,IPC_RMID,NULL) < 0)
    {
        fprintf(stderr, "smctl vector failed");
        exit(1);
    }
}

void read_vector(){
	FILE* f = fopen(out_file, "r");
        char buffor[BUFFOR_SIZE+1];
	fgets(buffor, BUFFOR_SIZE, f);
 	n = atoi(buffor);
	printf("Vector has %d elements\n", n);
	for(int i=0; i<n; i++) {
		fgets(buffor, BUFFOR_SIZE, f);
		vector[i] = atof(buffor);
        //printf("element i=%d ma wartosc y=%g\n", i, vector[i]);
	}
	fclose(f);
}

int main(int argc, char**argv)
{
    count = argc > 1 ? atoi(argv[1]) : 10;
    double lower = argc > 2 ? atof(argv[2]) : 0;
    double upper = argc > 3 ? atof(argv[3]) : 1;
    numOfChildren = argc > 4 ? atof(argv[4]) : 10;
    out_file = argc > 5 ? argv[5] : "vector.dat";

    generate(count, lower, upper, out_file);

    init_memory();

    pid_t pids[numOfChildren];
    pid_t pid;
    id = -1;
    for(int i = 0; i < numOfChildren; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            printf("Error creating process");
            return 2;
        }
        if(pids[i] == 0){
            id = i;
            break;
        }
    }
    if(id >= 0){
        sigset_t mask;
        struct sigaction usr1;
        sigemptyset(&mask);
        usr1.sa_handler = (&on_usr1);
        usr1.sa_mask = mask;
        usr1.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &usr1, NULL);
        putChildToSleep();
    }

    int i;
    for(i=0; i<numOfChildren; i++){
        tab_range[i] = i;
        tab_sum[i] = 0;
    }

    read_vector();

	printf("Suma elementów w wektorze = %f\n", sum(vector, n));    

    for(int i = 0 ; i < numOfChildren ; i++){
        kill(pids[i], 10);
    }

    while(wait(NULL) > 0);
    double final_sum = 0;
    for(int i = 0; i < numOfChildren; i++){
        final_sum += tab_sum[i];
    }

    end_memory();

    printf("Suma policzona z dzielona pamiecia: %g\n", final_sum);

    return EXIT_SUCCESS;
}