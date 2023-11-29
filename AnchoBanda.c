#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// ============================================================================
int main( int argc, char * argv[] ) {
  // Declaracion de variables.
  MPI_Status  s;
  int         numProcs, miId, numArgs, vecArgs[ 5 ] = { 0, 0, 0, 0, 0 };
  int         numMensajes, minTam, maxTam, incTam, tam, i, j;
  char        * ptrWorkspace;
  double      t1, t2, tiempoTotal, tiempoPorMensajeEnMicroseg,
              anchoDeBandaEnMbs;
  char        miNombreProc[ MPI_MAX_PROCESSOR_NAME ];
  int         longNombreProc;

  // Inicializacion de MPI.
  MPI_Init( & argc, & argv );
  MPI_Comm_size( MPI_COMM_WORLD, & numProcs );
  MPI_Comm_rank( MPI_COMM_WORLD, & miId );

  // Comprobacion del numero de procesos.
  if( numProcs < 2 ) {
    if ( miId == 0 ) {
      fprintf( stderr, "\nError: Al menos se deben iniciar dos procesos\n\n" );
    }
    MPI_Finalize();
    return( -1 );
  }
  
  // Imprime el nombre de los procesadores.
  MPI_Get_processor_name( miNombreProc, & longNombreProc );
  printf( "Proceso %d  Se ejecuta en: %s\n", miId, miNombreProc );

  // El proceso 0 inicializa las cinco variables.
  if( miId == 0 ) {
    numArgs     = argc;
    numMensajes = ( numArgs > 1 )? atoi( argv[ 1 ] ): -1;
    minTam      = ( numArgs > 2 )? atoi( argv[ 2 ] ): -1;
    if( numArgs == 5 ) {
      maxTam = atoi( argv[ 3 ] );
      incTam = atoi( argv[ 4 ] );
    } else {
      maxTam = minTam;
      incTam = 1;
    }
  }

  // El proceso 0 prepara el vector con las cinco variables.
  if( miId == 0 ) {
    vecArgs[ 0 ] = numArgs;
    vecArgs[ 1 ] = numMensajes;
    vecArgs[ 2 ] = minTam;
    vecArgs[ 3 ] = maxTam;
    vecArgs[ 4 ] = incTam;
  }

  // Difusion del vector vecArgs con operaciones punto a punto.
  // ... (A)
  
    if (miId == 0) {
        // El proceso 0 envia el vector a los demas procesos, uno por uno.
        for (i = 1; i < numProcs; i++) {
        // Envia el vector vecArgs al proceso i, con la etiqueta 0 y el comunicador MPI_COMM_WORLD.
        MPI_Send(vecArgs, 5, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // El resto de procesos reciben el vector del proceso 0, con la etiqueta 0 y el comunicador MPI_COMM_WORLD.
        MPI_Recv(vecArgs, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, &s);
    }
  
  // El resto de procesos inicializan las cinco variables con la 
  // informacion del vector. El proceso 0 no tiene que hacerlo porque
  // ya habia inicializado las variables.
  if( miId != 0 ) {
    numArgs     = vecArgs[ 0 ];
    numMensajes = vecArgs[ 1 ];
    minTam      = vecArgs[ 2 ];
    maxTam      = vecArgs[ 3 ];
    incTam      = vecArgs[ 4 ];
  }

  // Todos los procesos comprueban el numero de argumentos de entrada.
  if( ( numArgs != 3 )&&( numArgs != 5 ) ) {
    if ( miId == 0 ) {
      fprintf( stderr, "\nUso: a.out numMensajes minTam [ maxTam incTam ]\n\n" );
    }
    MPI_Finalize();
    return( -1 );
  }

  // Imprime los parametros de trabajo.
  if( miId == 0 ) {
    printf( "  Numero de procesos:  %5d\n", numProcs );
    printf( "  Numero de mensajes:  %5d\n", numMensajes );
    printf( "  Tamanyo inicial   :  %5d\n", minTam );
    printf( "  Tamanyo final     :  %5d\n", maxTam );
    printf( "  Incremento        :  %5d\n", incTam );
  }

  // Crea un vector capaz de almacenar el espacio maximo.
  if( maxTam != 0 ) {
    ptrWorkspace = ( char * ) malloc( maxTam );
    if( ptrWorkspace == NULL ) {
      if ( miId == 0 ) {
        fprintf( stderr, "\nError en Malloc: Devuelve NULL.\n\n");
      }
      MPI_Finalize();
      return( -1 );
    }
  } else {
    ptrWorkspace = NULL;
  }

  // Imprime cabecera de la tabla.
  if ( miId == 0 ) {
    printf( " Comenzando bucle para envio de informacion\n\n" );
    printf( " Tamanyo(bytes)   tiempoTotal(s.)" );
    printf( "  tiempoPorMsg(microsec.)  AnchoBanda(MB/s)\n");
    printf( " -----------------------------------" );
    printf( "----------------------------------------\n");
  }

  // Sincronizacion de todos los procesos
  MPI_Barrier( MPI_COMM_WORLD );

  // Bucle para pruebas de tamanyos.
  for( tam = minTam; tam <= maxTam; tam += incTam ) {

    // Sincronizacion de todos los procesos
    MPI_Barrier( MPI_COMM_WORLD );

    // Bucle de envio/recepcion de "numMensajes" de tamanyo "tam" y toma de tiempos.
    // ... (B)

        if (miId == 0) {
            // Process 0 sends messages to process 1 and waits for an acknowledgment
            t1 = MPI_Wtime(); // Start the timer
            for (i = 0; i < numMensajes; i++) {
                // Send a message of size tam bytes to process 1, with the tag i and the communicator MPI_COMM_WORLD
                    MPI_Send(ptrWorkspace, tam, MPI_CHAR, 1, i, MPI_COMM_WORLD);
            }
            // Receive a message of size 0 bytes from process 1, with any tag and the communicator MPI_COMM_WORLD
            MPI_Recv(ptrWorkspace, 0, MPI_CHAR, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
            t2 = MPI_Wtime(); // Stop the timer
            tiempoTotal = t2 - t1; // Calculate the total time
        } else if (miId == 1) {
            // Process 1 receives messages from process 0 and sends an acknowledgment when it receives the last one
            for (i = 0; i < numMensajes; i++) {
                // Receive a message of size tam bytes from process 0, with the tag i and the communicator MPI_COMM_WORLD
                MPI_Recv(ptrWorkspace, tam, MPI_CHAR, 0, i, MPI_COMM_WORLD, &s);
                // If it is the last message, send a message of size 0 bytes to process 0, with the tag i and the communicator MPI_COMM_WORLD
                if (i == numMensajes - 1) {
                    MPI_Send(ptrWorkspace, 0, MPI_CHAR, 0, i, MPI_COMM_WORLD);
                }
            }
        }

    // Calculo de prestaciones: tiempoTotal, tiempoPorMensajeEnMicroseg,
    // anchoDeBandaEnMbs.
    // ... (C)

    if (miId == 0) {
        tiempoPorMensajeEnMicroseg = (tiempoTotal / numMensajes) * 1e6; // Convert seconds to microseconds
        anchoDeBandaEnMbs = numMensajes/(tiempoPorMensajeEnMicroseg); // Convert bytes to megabits
    }

    // Escritura de resultados.
    if ( miId == 0 ) {
      printf("   %8d", tam );
      if( tiempoTotal >= 0.0) {
        printf("   %15.6f", tiempoTotal );
        printf("   %15.3f", tiempoPorMensajeEnMicroseg );
        printf("   %21.2f", anchoDeBandaEnMbs );
        printf("\n");
      } else {
        printf(": No se han realizado los calculos.\n" );
      }
    }
  }

  // Imprime final de la tabla.
  if ( miId == 0 ) {
    printf( " -----------------------------------" );
    printf( "----------------------------------------\n");
  }

  // Liberacion del espacio.
  if( maxTam != 0 ) {
    free( ptrWorkspace );
  }

  // Cierre de MPI.
  MPI_Finalize();

  if ( miId == 0 ) {
    printf( "Fin del programa\n" );
  }
  return 0;
}
