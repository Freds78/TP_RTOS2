/*=============================================================================
 * Copyright (c) 2019, Agustin Curcio Berardi <acurber91@gmail.com>
 * All rights reserved.
 * License: mit (see LICENSE.txt)
 * Date: 2019/09/22
 * Version: v1.0
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"

#include "semphr.h"

// Includes de FreeRTOS


#include "extras.h"

#include "FreeRTOSConfig.h"
#include "qmpool.h"

#include "FreeRTOS.h"
#include "task.h"
#include "driver.h"
#include "uartIRQ.h"

// sAPI header
#include "sapi.h"


tObjeto obj_1;
tObjeto obj_2;

extern tMensaje Pool_puntero;
extern QMPool Pool_memoria;

extern char mensaje[];
//extern char msn[];

void tarea_crear(TaskFunction_t tarea,const char * const nombre,uint8_t stack,void * const parametros,uint8_t prioridad,TaskHandle_t * const puntero)
{
	// Crear tarea en freeRTOS
	BaseType_t res = xTaskCreate(tarea,nombre,configMINIMAL_STACK_SIZE*stack,parametros,tskIDLE_PRIORITY+prioridad,puntero);                         		// Puntero a la tarea creada en el sistema

	// Gestion de errores
	if(res == pdFAIL)
	{
		gpioWrite( LEDR, ON );
		imprimir( "Error al crear las tareas." );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

}

void led_task( void* pvParameters )
{
	while( TRUE )
	{
		gpioToggle( LEDB );
		vTaskDelay(500/portTICK_RATE_MS);
	}
}
//checkTask recibe como parametro los datos de una UART con el driver inicializado
//se queda esperando a que llegue un paquete, lo procesa y lo envia por la UART

void uart_task( void* pvParameters )
{

	driver_t *Uart_driver;
	Uart_driver = ( driver_t * )pvParameters;

	char buffer2 [PACKET_SIZE];
	tMensaje* p = NULL;

	while( TRUE )
	{

		if (p != NULL){
			gpioToggle( LED3 );

			xSemaphoreTake( Uart_driver->permiso  , portMAX_DELAY );

			get(&obj_1,&p);

			memcpy(buffer2,p, sizeof(p));

			packetTX( Uart_driver , buffer2 );

			vTaskDelay(100/portTICK_RATE_MS);
		}

	}

}

void packetTX( driver_t* Uart_driver, char *buffer2 )
{

    //xQueueSend( Uart_driver->onTxQueue, &mensaje, portMAX_DELAY ); //Envio a la cola de transmision el blocke a transmitir
    taskENTER_CRITICAL();  //no permito que se modifique txcounter
    if ( Uart_driver->txCounter == 0 ) //si se esta enviando algo no llamo a la interrupcion para no interrumpir el delay
    {
        txInterruptEnable( Uart_driver );
    }
    taskEXIT_CRITICAL();
    uartSetPendingInterrupt( Uart_driver->uartValue );
}




/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

