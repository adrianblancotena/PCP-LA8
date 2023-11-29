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

    // Calculo de prestaciones: tiempoTotal, tiempoPorMensajeEnMicroseg,
    // anchoDeBandaEnMbs.
    // ... (C)

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
