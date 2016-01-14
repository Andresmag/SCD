#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

using namespace std;

void Filosofo( int id, int nprocesos);
void Tenedor ( int id, int nprocesos);

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   if( size!=10)
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 10" << endl << flush ;
         
      MPI_Finalize( ); 
      return 0; 
   }
   
   if ((rank%2) == 0)  
      Filosofo(rank,size); // Los pares son Filosofos 
   else 
      Tenedor(rank,size);  // Los impares son Tenedores 
   
   MPI_Finalize( );
   return 0;
}  
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
   int izq = (id+1) % nprocesos;
   int der = ((id+nprocesos)-1) % nprocesos; 
   int peticion;  
   
   while(1)
   {
   	peticion = 1;
      // Solicita tenedor izquierdo
      // El Filosofo 0 lo hace al reves
      if(id == 0){
      	//Solicita primero el derecho
      	cout << "Filosofo "<<id<< " solicita tenedor der ..." << der << endl << flush;
      	MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
      }
      else{
		   cout << "Filosofo "<<id<< " solicita tenedor izq ..." << izq << endl << flush;
		   MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
     	}

      // Solicita tenedor derecho
      // El Filosofo 0 ahora va a solicitar el de izquierda 
      if(id == 0){
      	cout <<"Filosofo "<<id<< " solicita tenedor izq ..." << izq << endl << flush;
      	MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
      }
      else{
      	cout <<"Filosofo "<<id<< " solicita tenedor der ..." << der << endl << flush;
      	MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
      }
  
      cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
      sleep((rand() % 3)+1);  //comiendo
      peticion = 0;
      cout << "Filosofo " << id << " TERMINA DE COMER" << endl << flush;
      
      // Suelta el tenedor izquierdo
      // El Filosofo 0 comienza soltando el de la derecha
      if(id == 0){
      	cout <<"Filosofo "<<id<< " suelta tenedor der ..." << der << endl << flush;
      	MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
      }
      else{
      	cout <<"Filosofo "<<id<< " suelta tenedor izq ..." << izq << endl << flush;
      	MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
      }
      
      // Suelta el tenedor derecho
      // El Filosofo 0 suelta el tenedor izquierdo
      if(id == 0){
      	cout <<"Filosofo "<<id<< " suelta tenedor izq ..." << izq << endl << flush;
      	MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
      }
      else{
      	cout <<"Filosofo "<<id<< " suelta tenedor der ..." << der << endl << flush;
      	MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
      }
      
      // Piensa (espera bloqueada aleatorio del proceso)
      cout << "Filosofo " << id << " PENSANDO" << endl << flush;
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una dÃ©cima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
      cout << "Filosofo " << id << " TERMINA DE PENSAR" << endl << flush;
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  int buf; 
  MPI_Status status; 
  int Filo;
  
  while( true )
  {
    // Espera un peticion desde cualquier filosofo vecino ...
    // Como los filosofos mandan a rank+1 o rank-1 el tenedor solo con id = X
    // solo va a recibir mensajes de los filosofos X-1 y X+1
    // Vamos a comprar de cual de los dos es
    MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
    Filo = status.MPI_SOURCE;
    
    // Recibe la peticion del filosofo ...
    MPI_Recv(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD, &status);
    cout << "Ten. " << id << " recibe petic. de " << Filo << endl << flush;
    
    // Espera a que el filosofo suelte el tenedor...
    MPI_Recv(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD, &status);
    cout << "Ten. " << id << " recibe liberac. de " << Filo << endl << flush; 
  }
}
// ---------------------------------------------------------------------
