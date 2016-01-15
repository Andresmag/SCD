#include <mpi.h>
#include <iostream>
#include <math.h>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"


#define Productores    4
#define Buffer         5
#define Consumidores   9
#define ITERS          20
#define TAM            5

using namespace std;

// ---------------------------------------------------------------------

void productor()
{
   int value, rank;
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   
   for ( unsigned int i=0; i < (ITERS / 5) ; i++ )
   { 
      value =  rank * Productores + i;

      cout << "Productor " << rank << " produce valor " << value << endl << flush ;
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una dÃ©cima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
      
      // enviar 'value' al Buffer, con etiqueta 1
      MPI_Ssend( &value, 1, MPI_INT, Buffer, 1, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void buffer()
{
   int        value[TAM] , 
              peticion , 
              pos  =  0,
              rama ;
   MPI_Status status ;
   
   for( unsigned int i=0 ; i < ITERS*2 ; i++ )
   {  
      int num;

      if (pos == 0)          // el consumidor no puede consumir
      {
         rama = 0 ;
         MPI_Probe( MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status );
         num = status.MPI_SOURCE;
      }

      else if (pos == TAM)   // el productor no puede producir
      {
         rama = 1 ;
         MPI_Probe( MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status );
         num = status.MPI_SOURCE;
      }
      else                   // ambas guardas son ciertas
      {
         // leer 'status' del siguiente mensaje (esperando si no hay)
         MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
         
         // calcular la rama en funciÃ³n del origen del mensaje
         num = status.MPI_SOURCE;

         if (num <= Productores) 
            rama = 0 ; 
         else 
            rama = 1 ;
      }
      switch(rama)
      {
         case 0:
            MPI_Recv( &value[pos], 1, MPI_INT, num, 1, MPI_COMM_WORLD, &status );
            cout << "Buffer recibe " << value[pos] << " de Productor " << num << endl << flush;  
            pos++;
            break;

         case 1:
            MPI_Recv( &peticion, 1, MPI_INT, num, 0, MPI_COMM_WORLD, &status );
            MPI_Ssend( &value[pos-1], 1, MPI_INT, num, 2, MPI_COMM_WORLD);
            cout << "Buffer envÃ­a " << value[pos-1] << " a Consumidor " << Consumidores - num << endl << flush;  
            pos--;
            break;
      }     
   }
}   
   
// ---------------------------------------------------------------------

void consumidor()
{
   int         value,
               peticion = 1 ; 
   float       raiz ;
   MPI_Status  status ;
 	int rank;
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   
   for (unsigned int i=0; i < (ITERS / 4); i++)
   {
      MPI_Ssend( &peticion, 1, MPI_INT, Buffer, 0, MPI_COMM_WORLD ); 
      MPI_Recv ( &value, 1,    MPI_INT, Buffer, 2, MPI_COMM_WORLD,&status );
      
      usleep(1000U);	//Para que no salte antes este mensaje que el de que el buffer se lo envia al consumidor
      cout << "Consumidor " << Consumidores - rank << " recibe valor " << value << " de Buffer " << endl << flush ;
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una dÃ©cima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
      
      raiz = sqrt(value) ;
   }
}
// ---------------------------------------------------------------------

int main(int argc, char *argv[]) 
{
   int rank,size; 
   
   // inicializar MPI, leer identif. de proceso y nÃºmero de procesos
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   // inicializa la semilla aleatoria:
   srand ( time(NULL) );
   
   // comprobar el nÃºmero de procesos con el que el programa 
   // ha sido puesto en marcha (debe ser 3)
   if ( size != 10 ) 
   {
      cout<< "El numero de procesos debe ser 10 "<<endl;
      return 0;
   } 
   
   // verificar el identificador de proceso (rank), y ejecutar la
   // operaciÃ³n apropiada a dicho identificador
   if ( rank <= Productores ) 
      productor();
   else if ( rank == Buffer ) 
      buffer();
   else 
      consumidor();
   
   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}
