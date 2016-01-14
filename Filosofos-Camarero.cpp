#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

#define camarero 10
#define ProcesosSinCamarero 10

using namespace std;



void Filosofo( int id, int nprocesos);
void Tenedor ( int id, int nprocesos);
void Camarero();

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   if( size!=11)
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 11" << endl << flush ;
         
      MPI_Finalize( ); 
      return 0; 
   }
   
   if ((rank%2) == 0){
   	if(rank == 10)
   		Camarero();	// El 10 aunque es par es el camarero
   	else
      	Filosofo(rank,size); // Los pares son Filosofos
   }
   else 
      Tenedor(rank,size);  // Los impares son Tenedores 
   
   MPI_Finalize( );
   return 0;
}  
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
   int izq = (id+1) % ProcesosSinCamarero;
   int der = ((id+ProcesosSinCamarero)-1) % ProcesosSinCamarero; 
   int peticion, llamar_camarero = 1;
   
   while(1)
   {
   	peticion = 1;
   	
 		// Solicita sentarse
 		cout << "Filosofo " << id << " solicita al Camarero sentarse" << endl << flush;
 		MPI_Ssend(&llamar_camarero, 1, MPI_INT, camarero, 1, MPI_COMM_WORLD);
 		cout << "Filosofo " << id << " se SIENTA" << endl << flush;
 		
      // Solicita tenedor izquierdo
		cout << "Filosofo "<<id<< " solicita tenedor izq ..." << izq << endl << flush;
		MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
     	

      // Solicita tenedor derecho
      // El Filosofo 0 ahora va a solicitar el de izquierda 
      cout <<"Filosofo "<<id<< " solicita tenedor der ..." << der << endl << flush;
      MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
      
  
      cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
      sleep((rand() % 3)+1);  //comiendo
      peticion = 0;
      cout << "Filosofo " << id << " TERMINA DE COMER" << endl << flush;
      
      // Suelta el tenedor izquierdo
      cout <<"Filosofo "<<id<< " suelta tenedor izq ..." << izq << endl << flush;
      MPI_Ssend(&peticion, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
      
      // Suelta el tenedor derecho
      cout <<"Filosofo "<<id<< " suelta tenedor der ..." << der << endl << flush;
      MPI_Ssend(&peticion, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
      
      // Le dice al Camarero que se levanta para que anote el sitio libre
      cout << "Filosofo " << id << " le dice al Camarero que se levanta" << endl << flush;
      MPI_Ssend(&llamar_camarero, 1, MPI_INT, camarero, 2, MPI_COMM_WORLD);
      cout << "Filosofo " << id << " se LEVANTA" << endl << flush;
      
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
    // Espera una peticion desde cualquier filosofo vecino ...
    // Como los filosofos mandan a rank+1 o rank-1 el tenedor solo con id = X
    // solo va a recibir mensajes de los filosofos X-1 y X+1
    // Vamos a comprar de cual de los dos es
    MPI_Probe( MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status );
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

void Camarero( )
{
	int buf, Filo, Tag, num_filosofos_mesa = 0;
	MPI_Status status;
	int flag;
	
	while(true)
	{
		if(num_filosofos_mesa < 4)
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		else
			MPI_Probe(MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
		
		
		Filo = status.MPI_SOURCE;
		Tag = status.MPI_TAG;
		cout << "\n\n TAG RECIBIDO -> " << Tag << endl << "DEL FILOSOFO -> " << Filo << endl << endl << flush;
		
		if(Tag == 1){
			MPI_Recv(&buf, 1, MPI_INT, Filo, 1, MPI_COMM_WORLD, &status);
			cout << "Camarero recibe peticion de Filosofo " << Filo << endl << flush;
    		num_filosofos_mesa++;
    	}
    	else if(Tag == 2){
    		MPI_Recv(&buf, 1, MPI_INT, Filo, 2, MPI_COMM_WORLD, &status);
    		cout << "Camarero recibe de Filosofo " << Filo << " la seÃ±al de que se levanta" << endl << flush;
    		num_filosofos_mesa--;
    	}
	}
}
