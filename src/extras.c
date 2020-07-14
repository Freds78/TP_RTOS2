#include "extras.h"
#include "driver.h"

#include "FreeRTOS.h"
#include "task.h"



void post(tObjeto* objeto,tMensaje* mensaje)
{
	xQueueSend( objeto->cola, &mensaje, portMAX_DELAY );
}

void get(tObjeto* objeto,tMensaje** mensaje)
{
	xQueueReceive( objeto->cola, mensaje ,portMAX_DELAY );
}

void objeto_crear(tObjeto* objeto)
{
	objeto->cola  = xQueueCreate(POOL_TOTAL_BLOCKS,sizeof(tMensaje));

	if( objeto->cola == NULL)
	{
		gpioWrite( LEDR, ON );
		imprimir( "Error al crear la cola." );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

}

void imprimir(char* mensaje)
{
	uartWriteString( UART_USB, mensaje );
	uartWriteString( UART_USB, "\r\n" );
}
