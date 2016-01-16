#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std;
//-----------------------------------------------------------------------------
//Variables globales y semaforos
int producto;
sem_t
	estanquero,
	fumador1,
	fumador2,
	fumador3;

// ----------------------------------------------------------------------------
// función que simula la acción de fumar  como un retardo aleatorio de la hebra

void fumar() 
{ 
   //  inicializa la semilla aleatoria  (solo la primera vez)
   static bool primera_vez = true ;
   if ( primera_vez )
   {   primera_vez = false ;
      srand( time(NULL) );    
   }
   
   // calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
   const unsigned miliseg = 100U + (rand() % 1900U) ; 

   // retraso bloqueado durante 'miliseg' milisegundos
   usleep( 1000U*miliseg ); 
}

// ----------------------------------------------------------------------------
// Produce de manera aleatoria un numero entre 0 y 2 
// que se asocia a uno de los 3 productos

int produce (void)
{
	int numero = rand() % 3;
	return numero;
}
// ----------------------------------------------------------------------------
// Estanquero

void * Estanquero(void *)
{
	while(true){
		sem_wait( &estanquero);
	
		//Produce
		producto = produce();
	
		cout << "El estanquero pone a la venta: ";
	
		//Llama y espera
		switch (producto)
		{
			case 0:
				cout << " CERILLAS\n";
				sem_post( &fumador1);
			
				break;
			
			case 1:
				cout << " TABACO\n";
				sem_post( &fumador2);
			
				break;
		
			case 2:
				cout << " PAPEL\n";
				sem_post( &fumador3);
			
				break;
		}
	}
	
	return NULL;
			
}

// ----------------------------------------------------------------------------
// Fumador 1

void * Fumador1(void *)
{
	while(true){
		sem_wait( &fumador1);
	
		//Comprobacion de si le vale el producto
		bool puede_fumar = false;
	
		if(producto == 0)
			puede_fumar = true;
	
		//Recoge y desbloquea al estanquero
		sem_post( &estanquero);
	
		//fuma
		if(puede_fumar){
			cout << "\n\tEl fumador 1 comienza a fumar\n";
			fumar();
			cout << "\n\tEL fumador 1 termina de fumar\n";
		}
	}
	
	return NULL;
}

// ----------------------------------------------------------------------------
// Fumador 2

void * Fumador2(void *)
{
	while(true){
		sem_wait( &fumador2);
	
		//Comprobacion de si le vale el producto
		bool puede_fumar = false;
	
		if(producto == 1)
			puede_fumar = true;
	
		//Recoge y desbloquea al estanquero
		sem_post( &estanquero);
	
		//fuma
		if(puede_fumar){
			cout << "\n\tEl fumador 2 comienza a fumar\n";
			fumar();
			cout << "\n\tEL fumador 2 termina de fumar\n";
		}
	}
	
	return NULL;
}

// ----------------------------------------------------------------------------
//Fumador 3

void * Fumador3(void *)
{
	while(true){
		sem_wait( &fumador3);
	
		//Comprobacion de si le vale el producto
		bool puede_fumar = false;
	
		if(producto == 2)
			puede_fumar = true;
	
		//Recoge y desbloquea al estanquero
		sem_post( &estanquero);
	
		//fuma
		if(puede_fumar){
			cout << "\n\tEl fumador 3 comienza a fumar\n";
			fumar();
			cout << "\n\tEL fumador 3 termina de fumar\n";
		}
	}
	
	return NULL;
}

// ----------------------------------------------------------------------------

int main()
{
	pthread_t hebra_estanquero;
	int num_fumadores = 3;
	pthread_t fumadores[num_fumadores];
   
   cout << "El fumador 1 tiene PAPEL y TABACO por lo que necesita CERILLAS";
   cout << "\nEl fumador 2 tiene PAPEL y CERILLAS por lo que necesita TABACO";
   cout << "\nEl fumador 3 tiene TABACO y CERILLAS por lo que neceista PAPEL";
   cout << endl << endl;
   
   sem_init( &estanquero, 0, 1);
   sem_init( &fumador1, 0, 0);
   sem_init( &fumador2, 0, 0);
	sem_init( &fumador3, 0, 0);

	pthread_create( &hebra_estanquero, NULL, Estanquero, NULL);
   pthread_create (&(fumadores[0]), NULL, Fumador1, NULL);
   pthread_create (&(fumadores[1]), NULL, Fumador2, NULL);
   pthread_create (&(fumadores[2]), NULL, Fumador3, NULL);
	
	pthread_join( hebra_estanquero, NULL);
	for (unsigned i=0; i<num_fumadores; i++)
   	pthread_join (fumadores[i], NULL);

	sem_destroy( &estanquero);
	sem_destroy( &fumador1);
	sem_destroy( &fumador2);
	sem_destroy( &fumador3);
   
  return 0 ;
}
