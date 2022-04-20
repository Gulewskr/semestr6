include <mpi.h>
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
       
    double (*fun_ptr)(double) = &funcX;
    
    if (argc < 4)
    {
        printf("nie podano argumentów(doble) begin(doble) end(int) step_number\n");
        return -1;
    }

    double begin = atof(argv[1]);
    double end = atof(argv[2]);
    int steps = atoi(argv[3]);

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    //printf("Hello world from processor %s, rank %d out of %d processors\n",
    //    processor_name, world_rank, world_size);

    if (world_rank == 0) 
    {
        int rest = steps % world_size;
        double dx = (end - begin) / steps;
        for (int i = 1; i < world_size; i++)
        {
            double b, e;
            int sn = steps / world_size;
            if (i == 1) {
                b = begin;
                sn += rest;
            } else {
                b = begin + dx * (i * sn + rest);
            }
            e = b + dx * sn;

            MPI_Send(&b, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&e, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(&sn, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        /*
        MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",
            world_rank, token, world_rank - 1);
        */
    }
    else {
        // Set the token's value if you are process 0
        /*
        token = -1;
        */
    }
    

    /*
// Now process 0 can receive from the last process.
    if (world_rank == 0) {
        MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",
            world_rank, token, world_size - 1);
    }
    */

    double res = integrate(fun_ptr, begin, end, steps);
    printf("%f", res);

    // Finalize the MPI environment.
    MPI_Finalize();
}