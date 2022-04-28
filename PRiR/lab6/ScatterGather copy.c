#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double funcX(double x)
{
    return 2 * x * x + 5 * x + 2.5;
}

double integrate(double (*func)(double), double begin, double end, int num_points)
{
    double dx = (end - begin) / num_points;
    double sum = 0;

    for (int i = 0; i < num_points; i++)
    {
        sum += func(begin + i * dx);
    }

    return dx * sum;
}

int main(int argc, char** argv)
{
    double b = 0, e = 0;
    int sn = 0, world_size, world_rank;
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
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_size < 2)
    {
        MPI_Finalize();
        printf("Program wymaga użycia co najmniej 2 procesów. ILOSC: %d\n", world_size);
        return -1;
    }

    if (steps % (world_size - 1) != 0)
    {
        if (world_rank == 0)
            puts("Wymagana jest liczba punktow podzielnych przez liczbe procesow");
        MPI_Finalize();
        return(EXIT_FAILURE);
    }
    
    double* points = (double*)malloc(steps * sizeof(double));
    int singlePackageSize = steps / world_size;
    double* processR = (double*)malloc(singlePackageSize * sizeof(double));
    const double step = (end - begin) / (steps - 1);

    double x = begin;
    for (int i = 0; i < steps; i++)
    {
        points[i] = x;
        x += step;
    }


    // Spread points
    MPI_Scatter(points, singlePackageSize, MPI_DOUBLE, processR, singlePackageSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Each process calculates partial result
    double partialResult = integrate(fun_ptr, processR[0], processR[singlePackageSize-1], singlePackageSize);

    double result;
    MPI_Reduce(&partialResult, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    free(points);
    if (world_rank == 0)
        printf("Result: %lf\n", result);

    MPI_Finalize();
    return EXIT_SUCCESS;
}