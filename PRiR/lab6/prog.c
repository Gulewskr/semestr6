#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char** argv) {

    double b = 0, e = 0;
    int sn = 0, world_size, world_rank;
    double (*fun_ptr)(double) = &funcX;
    
    if (argc < 4)
    {
        printf("nie podano argumentów(doble) begin(doble) end(int) step_number\n");
        return -1;
    }
    double begin = atof(argv[1]);
    double end = atof(argv[2]);
    int steps = atoi(argv[3]);

    //MPI INICJALIZACJA
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if(world_size < 2)
    {
        MPI_Finalize();
        printf("Program wymaga użycia co najmniej 2 procesów. ILOSC: %d\n", world_size);
        return -1;
    }

    double integral_current = 0;
    double integral = 0;                                                
    int workers_number = world_size - 1;

    if (world_rank == 0) 
    {
        int rest = steps % workers_number;
        double dx = (end - begin) / steps;
        //wyliczenie i przeslanie argumentow do procesow liczacych
        for (int i = 1; i < world_size; i++)
        {
            sn = steps / workers_number;
            if (i == 1) {
                b = begin;
                sn += rest;
            } else {
                b = begin + dx * ((i-1) * sn + rest);
            }
            e = b + dx * sn;

            MPI_Send(&b, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&e, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&sn, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        //zliczenie wynikow
        for(int i = 1; i < world_size; i++)
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
        MPI_Recv(&b, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&e, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&sn, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //obliczenie
        integral_current = integrate(fun_ptr, b, e, sn);
        //przeslanie wyniku do procesu 0
        MPI_Send(&integral_current, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}
