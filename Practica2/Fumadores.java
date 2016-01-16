import java.util.Random;
import monitor.* ;

// ****************************************************************************

class Estanco extends AbstractMonitor
{
	private int ingrediente_comun;
	private boolean hay_ingrediente;
	
	private Condition producir = makeCondition();
	private Condition fumador[] = new Condition[3];
	
	public Estanco(){
		ingrediente_comun = -1;
		hay_ingrediente = false;
		for(int i=0; i<3;i++)
			fumador[i] = makeCondition();
	}
	
	public void obtenerIngrediente(int miIngrediente)
	{
		enter();
			if(ingrediente_comun != miIngrediente)
				fumador[miIngrediente].await();
			
			hay_ingrediente = false;	//Se recoge el ingrediente
			System.out.println("Fumador " + miIngrediente + " fumando");
			producir.signal();	//Se avisa de que ya puede volver a producir
		leave();
				
	}
	
	public void ponerIngrediente(int ingrediente)
	{
		enter();
			//Colocamos el ingrediente y decimos que ya si hay
			ingrediente_comun = ingrediente;
			hay_ingrediente = true;
			String genera = " =";
			switch(ingrediente_comun){
				case 0:
					genera += " CERILLAS";
					break;
				
				case 1:
					genera += " TABACO";
					break;
				
				case 2:
					genera += " PAPEL";
					break;
			}
			
			System.out.println("Estanquero, pone en el mostrador " + ingrediente_comun + genera);
			//Llamamos al fumador que necesita ese ingrediente
			fumador[ingrediente_comun].signal();
		leave();
	}
	
	public void esperarRecogidaIngrediente()
	{
		enter();
			if(hay_ingrediente)	//Si ya hay un ingrediente no podemos colocar otro hasta que
				producir.await();	//se quite, por lo tanto, el estanquero espera 
		leave();
	}
}
// ****************************************************************************

class Estanquero implements Runnable
{
	private Estanco estanco;
	public Thread thr;
	
	//Constructor
	public Estanquero(Estanco p_estanco){
		estanco = p_estanco;
		thr = new Thread(this,"Estanquero");
	}
	
	//Metodo para ejecutarlo
	public void run(){
		int ingrediente;
		while (true)
		{
			ingrediente = (int) (Math.random()*3.0); //Genera numero entre 0,1 y 2
			estanco.ponerIngrediente(ingrediente);
			estanco.esperarRecogidaIngrediente();
		}
	}
}

// ****************************************************************************

class Fumador implements Runnable
{
	private int miIngrediente;
	private Estanco estanco;
	public Thread thr;
	
	//Constructor
	public Fumador(int p_miIngrediente, Estanco p_estanco){
		estanco = p_estanco;
		miIngrediente = p_miIngrediente;
		thr = new Thread(this,"Fumandor "+p_miIngrediente);
	}
	
	//Metodo para ejecutarlo
	public void run(){
		while (true){
			estanco.obtenerIngrediente( miIngrediente );
			aux.dormir_max(2000);
			System.out.println(thr.getName() + " termina de fumar");
		}
	}
}

// ****************************************************************************

class aux
{
	static Random genAlea = new Random() ;
	static void dormir_max( int milisecsMax ){
		try{
			Thread.sleep( genAlea.nextInt( milisecsMax ) ) ;
		}
		
		catch( InterruptedException e ){
			System.err.println("sleep interumpido en ’aux.dormir_max()’");
		}
	}
}

// ****************************************************************************

class MainFumadores 
{ 
	public static void main(String[] args) 
	{ 

		// crear monitor intermedio
		Estanco estanco = new Estanco();
		
		// crear hebras
		Estanquero estanquero = new Estanquero(estanco);
		Fumador[] fumador = new Fumador[3];	//Vector con los fumadores
		for(int i = 0; i < fumador.length; i++){
			fumador[i] = new Fumador(i, estanco);
			String ingredientes_iniciales = " tiene";
			switch (i){
				case 0:
					ingredientes_iniciales += " TABACO y PAPEL";
					break;
				
				case 1:
					ingredientes_iniciales += " PAPEL y CERILLAS";
					break;
					
				case 2:
					ingredientes_iniciales += " TABACO y CERILLAS";
					break;
			}
			
			System.out.println("Fumador " + i + ingredientes_iniciales);
		}

		// poner en marcha las hebras
		estanquero.thr.start();
		for(int i = 0; i < fumador.length; i++) 
			fumador[i].thr.start();
	}
}
