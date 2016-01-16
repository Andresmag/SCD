#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>

using namespace std ;

// ---------------------------------------------------------------------
// constantes 
const unsigned 
  num_items  = 40 ,
  tam_vector = 10 ;     
  
//Vector que guarda los numeros producidos y su controlador
int vector_datos[tam_vector];
int primera_libre = 0;  

//Semaforos de control
sem_t
	puede_producir,
	puede_consumir,
	mutex;       

// ---------------------------------------------------------------------

unsigned producir_dato()
{
  static int contador = 0 ;
  sem_wait( &mutex);
  cout << "producido: " << contador << endl << flush ;
  sem_post( &mutex);
  return ++contador;
}
// ---------------------------------------------------------------------

void consumir_dato( int dato )
{
    cout << "dato recibido: " << dato << endl ;
}
// ---------------------------------------------------------------------

void * productor( void * )
{  
	int dato;
	for( unsigned i = 0 ; i < num_items ; i=i+10 ){ 
	
		sem_wait( &puede_producir);	//Da la señal de que se puede producir
		
		for( int i=0; i < tam_vector;i++){
    		dato = producir_dato() ;
    		vector_datos[primera_libre] = dato;
   		primera_libre++;
   	}
   	
   	//Una vez relleno el vector se da la señal de que se puede consumir
    	sem_post( &puede_consumir);
  }
  return NULL ;
}
// ---------------------------------------------------------------------

void * consumidor( void * )
{  
	int dato;
	for( unsigned i = 0 ; i < num_items ; i=i+10 ){   
    
    sem_wait( &puede_consumir);	//Da la señal de que se puede consumir
    for( int i=0; i < tam_vector; i++){
    	primera_libre--;
    	dato = vector_datos[primera_libre];
    	sem_wait( &mutex);
    	consumir_dato( dato );
    	sem_post( &mutex);
    }
    
    //Una vez vacio el vector se puede seguir produciendo y guardando
    sem_post( &puede_producir);
    
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{
   pthread_t hebra_productora, hebra_consumidora;
   
   sem_init( &mutex, 0, 1);
   sem_init( &puede_producir, 0, 1);
   sem_init( &puede_consumir, 0, 0);

	pthread_create( &hebra_productora, NULL, productor, NULL);
	pthread_create( &hebra_consumidora, NULL, consumidor, NULL);
	
	pthread_join( hebra_productora, NULL);
	pthread_join( hebra_consumidora, NULL);

	sem_destroy( &puede_producir);
	sem_destroy( &puede_consumir);
	sem_destroy( &mutex);
	
	cout << "\nFIN!\n\n";
	 
   return 0 ; 
}
