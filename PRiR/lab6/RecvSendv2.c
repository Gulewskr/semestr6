#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

double funcX(double x)
{
    return 2 * x * x + 5 * x + 2.5;
}

double integratePart(double (*func)(double), double *nodes, int nodesAmount)
{
    if (nodesAmount <= 1)
        return 0.0;
    
    double sum = 0.0;
    double dx = nodes[1] - nodes[0];

    for (int i = 0; i < nodesAmount; i++)
    {
        sum += func(nodes[i]);
    }

    return dx * sum;
}

double* generateNodesTable(double begin, double end, int num_points)
{
    double* nodes = (double*)malloc(sizeof(double)*num_points);
    double dx = (end - begin) / (num_points - 1);
    for(int i = 0; i < num_points; i++)
    {
        nodes[i] = begin + i * dx;
    }
    return nodes;
}

double integrate(double (*func)(double), double begin, double end, int num_points)
{
    int world_size, world_rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    if (world_size < 2)
    {
        MPI_Finalize();
        printf("Program wymaga użycia co najmniej 2 procesów. ILOSC: %d\n", world_size);
        return -1;
    }

    double integral_current = 0;
    double integral = 0;
    int workers_number = world_size - 1;

    double* nodes = (double*)malloc(sizeof(double)*num_points);
    double sn = 0;

    if (world_rank == 0)
    {
        nodes = generateNodesTable(begin, end, num_points);
        
        int rest = num_points % workers_number;
        sn = num_points / workers_number;
        //wyliczenie i przeslanie argumentow do procesow liczacych
        for (int i = 1; i < world_size; i++)
        {
            if (i == 1) {
                sn += rest;
            MPI_Send(&sn, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(nodes, sn, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }

        //zliczenie wynikow
        for (int i = 1; i < world_size; i++)
        {
            MPI_Recv(&integral_current, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            integral += integral_current;
            printf("Wynik: %f procesu from process %d\n", integral_current, i);
        }
        printf("Wynik: %f \n", integral);

        //Dla pojedynczego procesu
        //double res = integrate(fun_ptr, begin, end, steps);
        //printf("Wynik sprawdzony dla 1 wątku %f\n\n", res);
    }
    else {
        //otrzymanie parametrow z procesu 0
        MPI_Recv(&sn, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(nodes, sn, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //obliczenie
        integral_current = integratePart(func, nodes, sn);
        //przeslanie wyniku do procesu 0
        MPI_Send(&integral_current, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }


    return integral;
}

int main(int argc, char** argv) {
    char* f = "2x^2 + 5x + 2.5";
    double (*fun_ptr)(double) = &funcX;

    if (argc < 4)
    {
        printf("nie podano argumentow(doble) begin(doble) end(int) step_number\n");
        return -1;
    }
    double begin = atof(argv[1]);
    double end = atof(argv[2]);
    int steps = atoi(argv[3]);

    //MPI INICJALIZACJA
    MPI_Init(NULL, NULL);
    double res = integrate(fun_ptr, begin, end, steps);
    printf("Calkowana funkcja \' %s \' w przedziale < %f , %f> liczba krokow: %d\n", f, begin, end, steps);
    printf("wynik calkowania: %d\n\n", res);
    MPI_Finalize();
    return 0;
}