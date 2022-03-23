#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int *tab_range;
int thread_index = 0;
int thread_count;
double **A;
double **B;
double **C;
int ma, mb, na, nb;
int count;
double global_sum = 0.0;
double frobenius_sum = 0.0;

void free_memory(double **X, int nsize){
    for(int i=0; i<nsize; i++)
    {
        free(X[i]);
    }
    free(X);
}

void print_matrix(double**A, int m, int n){
    int i, j;
    printf("[");
    for(i =0; i< m; i++)
    {
        for(j=0; j<n; j++)
        {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    printf("]\n");
}

void multiply_row(int row_num, int col_start, int col_end){
    int i, j, k;
    double s;
        for(j=col_start; j<col_end; j++)
        {
            s = 0;
            for(k=0; k<na; k++)
            {
                s+=A[row_num][k]*B[k][j];
            }
            C[row_num][j] = s;
            pthread_mutex_lock(&mut);
            global_sum += s;
            frobenius_sum += s*s;
            pthread_mutex_unlock(&mut);
        }
}

void multiply_matrix(int from, int to){
    int row_num = floor((from)/nb);
    int col_start = from - (row_num*nb);
    int col_end = col_start + (to - from) + 1;
    int remaining = to - from + 1;
    if(col_end > nb)
        col_end = nb;
    while(remaining > 0){
        multiply_row(row_num, col_start, col_end);
        row_num++;
        remaining = remaining -  (col_end - col_start);
        col_start=0;
        col_end=to - remaining + 1;
        if(col_end > nb)
            col_end = nb;
        if(remaining < nb){
            col_end--;
        }
    }
}



void * thread_multiply(void * thread_id){
    
    int id = *((int *)thread_id);
    int from = tab_range[id];
    int to = 0;
    if(id == thread_count - 1){
        to = count - 1;
    } else {
        to = tab_range[id+1] - 1;
    }
    multiply_matrix(from, to);
    return NULL;
}



int main(int argc, char**argv)
{
    FILE *fpa;
    FILE *fpb;
    int i, j;
    double x;
    thread_count = argc > 1 ? atoi(argv[1]) : 5;

    fpa = fopen("A.txt", "r");
    fpb = fopen("B.txt", "r");
    if( fpa == NULL || fpb == NULL )
    {
        perror("błąd otwarcia pliku");
        exit(-10);
    }

    fscanf (fpa, "%d", &ma);
    fscanf (fpa, "%d", &na);


    fscanf (fpb, "%d", &mb);
    fscanf (fpb, "%d", &nb);

    if(na != mb)
    {
        printf("Złe wymiary macierzy!\n");
        return EXIT_FAILURE;
    }
    
    A = malloc(ma*sizeof(double));
    for(i=0; i< ma; i++)
    {
        A[i] = malloc(na*sizeof(double));
    }

    B = malloc(mb*sizeof(double));
    for(i=0; i< mb; i++)
    {
        B[i] = malloc(nb*sizeof(double));
    }

    C = malloc(ma*sizeof(double));
    for(i=0; i< ma; i++){
        C[i] = malloc(nb*sizeof(double));
    }

    count = ma*nb; 
    tab_range = malloc(thread_count*sizeof(int));
    for(i = 0; i < thread_count; i++){
        int dist = floor( i * count/thread_count);
        tab_range[i] = dist;
    }

    for(i =0; i< ma; i++){
        for(j = 0; j<na; j++)
        {
            fscanf( fpa, "%lf", &x );
            A[i][j] = x;
        }
    }

    for(i =0; i< mb; i++){
        for(j = 0; j<nb; j++)
        {
            fscanf( fpb, "%lf", &x );
            B[i][j] = x;
        }
    }
    
    pthread_t thread[thread_count];

    int ids[thread_count];
    for(i = 0; i < thread_count; i++){
        ids[i] = i;
        pthread_create( &thread[i], NULL, thread_multiply, &ids[i]);
    }
    for (int i=0; i<thread_count; i++) pthread_join(thread[i], NULL);

    printf("Suma:%g\n", global_sum);
    printf("C:\n");
    print_matrix(C, ma, nb);
    printf("Norma Frobeniusa: %g\n", sqrt(frobenius_sum));
    pthread_mutex_destroy(&mut); 
    free_memory(A, na);
    free_memory(B, nb);
    free_memory(C, nb);
    free(tab_range);
    fclose(fpa);
    fclose(fpb);
}