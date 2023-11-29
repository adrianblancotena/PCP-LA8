#include <stdio.h> // Definicion de rutinas para E/S
#include <mpi.h>   // Definicion de rutinas de MPI

// Programa principal
int main(int argc, char *argv[])
{
  // Declaracion de variables
  int miId, numProcs;

  // Inicializacion de MPI
  MPI_Init(&argc, &argv);

  // Obtiene el numero de procesos en ejecucion
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs); // Obtiene el numero de procesos en ejecucion
  // Obtiene el identificador del proceso
  MPI_Comm_rank(MPI_COMM_WORLD, &miId); // Obtiene el identificador del proceso

  // ------ PARTE CENTRAL DEL CODIGO (INICIO) ---------------------------------

  // Impresion de un mensaje en el terminal
  printf("Hola, soy el proceso %d de %d\n", miId, numProcs);

  // ------ PARTE CENTRAL DEL CODIGO (FINAL) ----------------------------------

  // Finalizacion de MPI
  MPI_Finalize();

  return 0;
}

