#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void Read_n(int* n_p, int* count, int** blocklengths, int** displacements, MPI_Datatype* rtype, int my_rank, MPI_Comm comm);
void Read_matrix(double** a, int n, int my_rank, MPI_Comm comm);

int main(void) {
   int comm_sz, my_rank;
   MPI_Comm comm;

   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);

   int n, count, i;
   int* displacements;
   int* blocklengths;
   double* source;
   double* target;
   MPI_Datatype rtype;

   // Read_n
   Read_n(&n, &count, &blocklengths, &displacements, &rtype, my_rank, comm);

   // Allocate arrays
   source = malloc(n * n * sizeof(double));
   target = malloc(n * n * sizeof(double));

   // Read_m
   Read_matrix(&source, n, my_rank, comm);

   // Print
   if (my_rank == 0)
   {
      for(i = 0; i < n * n; ++i)
      {
         printf("%lf ", source[i]);
      }
      MPI_Send(source, 1, rtype, 1, 0, comm);
   }
   else
   {
      MPI_Recv(target, 1, rtype, 0, 0, comm, MPI_STATUS_IGNORE);
      printf("\n");
      for (i = 0; i < n * n; ++i)
      {
         printf("%lf ", target[i]);
      }
   }

   free(blocklengths);
   free(displacements);
   free(source);
   free(target);
   MPI_Finalize();

   return 0;
}  /* main */

void Read_n(
      int*          n_p           /* out */, 
      int*          count         /* out */, 
      int**         blocklengths  /* out */,
      int**         displacements /* out */,
      MPI_Datatype* rtype         /* out */,
      int           my_rank       /* in  */,
      MPI_Comm	    comm          /* in  */) {
   int i;

   if (my_rank == 0) {
      printf("What's the size of the matrix?\n");
      scanf("%d", n_p);
   }
   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

   *blocklengths = malloc(*n_p * sizeof(int));
   *displacements = malloc(*n_p * sizeof(int));

   if (my_rank == 0) {
      *count = 0;
      for (i = 0; i < *n_p; ++i)
      {
         (*blocklengths)[i] = *n_p - i;
         (*displacements)[i] = *n_p * i + i;
         *count += (*blocklengths)[i];
      }
   }
   MPI_Bcast(*blocklengths, *n_p, MPI_INT, 0, comm);
   MPI_Bcast(*displacements, *n_p, MPI_INT, 0, comm);
   MPI_Bcast(count, 1, MPI_INT, 0, comm);
   MPI_Barrier(comm);
   MPI_Type_indexed(*n_p, *blocklengths, *displacements, MPI_DOUBLE, rtype);
   MPI_Type_commit(rtype);
}  /* Read_n */

void Read_matrix(
      double**  a               /* out */, 
      int       n               /* in  */,
      int       my_rank         /* in  */, 
      MPI_Comm  comm            /* in  */) {

   int i;

   if (my_rank == 0) {
      printf("Enter the matrix:\n");
      for (i = 0; i < n * n; i++)
         scanf("%lf", &(*a)[i]);
   }
}  /* Read_matrix */  

