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
// Declaración e inicialización de la variable n
int n = 0;

// El proceso 0 lee un valor del teclado y lo almacena en n
if (miId == 0) {
  printf("Introduzca un valor para n: ");
  scanf("%d", &n);
}

// Todos los procesos imprimen el contenido de n
printf("Proceso %d con n = %d\n", miId, n);
  // Impresion de un mensaje en el terminal
  printf("Hola, soy el proceso %d de %d\n", miId, numProcs);

  // ------ PARTE CENTRAL DEL CODIGO (FINAL) ----------------------------------
// Declaración e inicialización de la variable n
int n = 0;

// El proceso 0 lee un valor del teclado y lo almacena en n
if (miId == 0) {
  printf("Introduzca un valor para n: ");
  scanf("%d", &n);
}

// El proceso 0 envía el valor de n al resto de procesos usando comunicaciones punto a punto
if (miId == 0) {
  for (int i = 1; i < numProcs; i++) {
    MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Envía n al proceso i con etiqueta 0
  }
}

// El resto de procesos reciben el valor de n del proceso 0 usando comunicaciones punto a punto
if (miId != 0) {
  MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Recibe n del proceso 0 con etiqueta 0
}

// Todos los procesos imprimen el contenido de n
printf("Proceso %d con n = %d\n", miId, n);

  // Finalizacion de MPI
  MPI_Finalize();

  return 0;
}

