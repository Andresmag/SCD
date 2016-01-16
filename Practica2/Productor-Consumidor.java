import monitor.*;
// ****************************************************************************

class Buffer extends AbstractMonitor
{
	private int tam_buffer = 0;
	private int ocupado_buffer = 0;
	private double buffer[] = null;
	
	private Condition puede_producir = makeCondition();
	private Condition puede_consumir = makeCondition();
	
	//Constructor
	public Buffer(int tamanio){
		tam_buffer = tamanio;
		buffer = new double[tam_buffer];
	}
	
	public void producir(double valor)
	{
		enter();
			if(ocupado_buffer == tam_buffer)
				puede_producir.await();
			
			buffer[ocupado_buffer] = valor;
			ocupado_buffer++;
			puede_consumir.signal();
		leave();
	}
	
	public double consumir()
	{
		enter();
			if(ocupado_buffer == 0)
				puede_consumir.await();
			
			double valor;
			ocupado_buffer--;
			valor = buffer[ocupado_buffer];
			puede_producir.signal();
		leave();
		return valor;
	}
}
		
// ****************************************************************************

class Productor implements Runnable
{
	private Buffer buf;
	private int cantidad_producir;	
	public Thread thr;	
	
	
	//Constructor
	public Productor(Buffer new_buf, int cantidad){
		buf = new_buf;
		cantidad_producir = cantidad;
		thr = new Thread(this, "Productor");
	}
	
	public void run(){
		for(int i=0; i<cantidad_producir; i++){
			double elemento = i+1;
			System.out.println(thr.getName()+", produciendo " + elemento);
			buf.producir(elemento);
		}
	}
}
	
// ****************************************************************************

class Consumidor implements Runnable
{
	private Buffer buf;
	private int cantidad_consumir;
	public Thread thr;
	
	//Constructor
	public Consumidor(Buffer new_buf, int cantidad){
		buf = new_buf;
		cantidad_consumir = cantidad;
		thr = new Thread(this, "Consumidor");
	}
	
	public void run(){
		for(int i=0; i<cantidad_consumir; i++){
			double valor;
			valor = buf.consumir();
			System.out.println(thr.getName()+", consumiendo " + valor);
		}
	}
}

// ****************************************************************************

class MainProdCons
{
	public static void main( String[] args )
	{
		//Creamos monitor
		Buffer buffer = new Buffer(10);
		
		//Creamos hebras
		Productor productor = new Productor(buffer, 40);
		Consumidor consumidor = new Consumidor(buffer, 40);
		
		//Ponemos en marcha las hebras
		productor.thr.start();
		consumidor.thr.start();
	}
}
