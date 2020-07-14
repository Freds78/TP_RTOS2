/*==================[inclusiones]============================================*/

// Includes de FreeRTOS
#include "extras.h"
#include "FreeRTOSConfig.h"
#include "qmpool.h"
#include "userTasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "driver.h"

// sAPI header
#include "sapi.h"



tMensaje* Pool_puntero; //puntero al segmento de memoria que albergara el pool
QMPool Pool_memoria; //memory pool (contienen la informacion que necesita la biblioteca qmpool.h

extern tObjeto obj_1;
extern tObjeto obj_2;

char mensaje[100];
char msn[100];
static driver_t Uart_driver; //Variable de la instanciacion del driver que va a usar la tarea principal



/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------
    // Inicializar y configurar la plataforma
    boardConfig();

    // Inicializo UART debug
    uartConfig( UART_USB, 115200 );
    imprimir( "\r\n### Parte 1 TP ###\r\n" );

    //	Reservo memoria para el memory pool
    Pool_puntero = ( tMensaje* ) pvPortMalloc( POOL_SIZE * sizeof( tMensaje ) );

    //	Creo el pool de memoria que va a usarse para la transmision
    QMPool_init( &Pool_memoria, ( tMensaje* ) Pool_puntero,POOL_SIZE*sizeof( tMensaje ),PACKET_SIZE ); //Tamanio del segmento de memoria reservado

    if ( Pool_puntero == NULL )
    {
    	gpioWrite( LEDR, ON );
		imprimir( "Error al crear el pool" );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
    }

    Uart_driver.uartValue = UART_USB;
    Uart_driver.baudRate = 115200;

    if ( driverInit( &Uart_driver ) == FALSE )
    {
    	gpioWrite( LED3, ON ); //Error al inicializar el driver
    	printf( "Error al inicializar el driver" );
    	while( 1 );
    }

    // Creacion Y validacion de tareas

    tarea_crear(led_task,"led_task",2,0,1,0);
    tarea_crear(uart_task,"uart_task",3,( void* )&Uart_driver,2,0);

    // Creacion Y validacion de objetos
    objeto_crear(&obj_1);

    Uart_driver.mutex = xSemaphoreCreateMutex();
    Uart_driver.permiso = xSemaphoreCreateBinary();

    Uart_driver.Cola1 = xQueueCreate( 20, sizeof( driver_t ) );

 	if( Uart_driver.Cola1 == NULL ) {
 		printf("Error al crear la cola muestraLed2\n");
 	}

    // Iniciar scheduler
    vTaskStartScheduler(); // Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
        // Si cae en este while 1 significa que no pudo iniciar el scheduler
    }

    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
    // directamenteno sobre un microcontroladore y no es llamado por ningun
    // Sistema Operativo, como en el caso de un programa para PC.
    return FALSE;
}

