#include "uartIRQ.h"
#include "extras.h"
#include "semphr.h"
#include "qmpool.h"
#include "Capa2.h"

void onTxCallback( void* );
void onRxCallback( void* );

//extern char mensaje[];
extern tObjeto obj_1;

tMensaje* Pool_puntero; //puntero al segmento de memoria que albergara el pool
QMPool Pool_memoria; //memory pool (contienen la informacion que necesita la biblioteca qmpool.h


//rxInterruptEnable(driverConfig_t* Uart_driver) Habilita la interrupcion de la UART para recepcion
//indicada por selectedUart
bool_t rxInterruptEnable( driver_t* Uart_driver )
{
	switch( Uart_driver-> uartValue)
	{
		case UART_GPIO:
		case UART_USB:
		case UART_232:
		{
			uartCallbackSet( Uart_driver->uartValue, UART_RECEIVE, onRxCallback, (void*)Uart_driver );
			break;
		}
		default:
		{
			return (FALSE); // No se eligio una UART correcta
		}
	}

	return TRUE;
}

//txInterruptEnable(driverConfig_t* Uart_driver) Habilita la interrupcion de la UART para transmision
//indicada por selectedUart
 bool_t txInterruptEnable( driver_t* Uart_driver )
 {
	 switch( Uart_driver-> uartValue)
	{
		case UART_GPIO:
		case UART_USB:
		case UART_232:
		{
			uartCallbackSet( Uart_driver->uartValue, UART_TRANSMITER_FREE, onTxCallback, (void*)Uart_driver );
			break;
		}
		default:
		{
			return (FALSE); // No se eligio una UART correcta
		}
	}

	return TRUE;

 }
/*=====[Implementations of interrupt functions]==============================*/

 // Callback para la recepcion
 void onRxCallback( void *param )
 {
	 BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 tMensaje* p = param;

	 // Elegir los datos que necesita el cb
	 driver_t* Uart_driver = param;

	 char c;
	 char msn[100];
	 char buffer_1 [100];
	 p = NULL;
	 int j,r;
	 j = 0;
	 r = 0;

	 //abrir seccion critica
	 xSemaphoreTakeFromISR( Uart_driver->mutex, &xHigherPriorityTaskWoken );

	 // Si llegan mas bytes de los que puedo guardar, no los leo.
	 if ( Uart_driver -> rxLen < LARGO )
	 {
		 // Leemos el byte de la UART seleccionado
		 c = uartRxRead( Uart_driver->uartValue );

		 // Asigno un bloque de memoria

		 if(c== '('){
			 j++;
		 }
		 if(c != ')'){
			 if(j == 1){
				 msn[r]= c;
				 r++;
			 }
			 if(j == 0){
				printf("error del paquete");
			 }
		 }
		 if(c == ')'){
			 if(j == 1){
				 j--;
//				 p = (tMensaje *)QMPool_get_fromISR( &Pool_memoria, 0);
				 p = ( tMensaje* ) QMPool_get( &Pool_memoria, 0 );
				 if (p != NULL){

					 buffer_1[100] = Conversion_Paquete(r);
					 memcpy(p,buffer_1, sizeof(buffer_1));

					 post(&obj_1,p);
					 xSemaphoreGiveFromISR( Uart_driver->permiso , &xHigherPriorityTaskWoken);
				 }
			 }
			 if(j == 0){
				 printf("error del paquete");
			 }
		 }
	 }
	 else
	 {
		 Uart_driver -> rxLen = LARGO + 1; // revisar ese +1
	 }

	 // Incrementar el indice
	 Uart_driver->rxLen ++;

	 xSemaphoreGiveFromISR( Uart_driver->mutex , &xHigherPriorityTaskWoken);
	 //cerrar seccion critica

	 portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 }


//	uartSendDataCRC transmite los bloques que llegan por una cola y los envia por la UART seleccionada
//	Todas las callback de interrupcion de UART llaman a esta función para hacer la transmisión.
 void onTxCallback( void*param )
 {

	 driver_t* Uart_driver;
	 UBaseType_t uxSavedInterrumptStatus;
	 BaseType_t xTaskWokenByReceive = pdFALSE;
	 char mensaje[100];
	 Uart_driver = ( driver_t* ) param;
	 tMensaje* p = param;
	 p = NULL;
	 // Armo una seccion critica para proteger tx_counter
	 uxSavedInterrumptStatus = taskENTER_CRITICAL_FROM_ISR();

	 // Si el contador es 0 -> puedo tomar un paquete de la cola
	 if ( Uart_driver -> txCounter == 0)
	 {
		 Uart_driver -> txLen = Uart_driver -> rxLen-1; // "Hola>"

		 //Uart_driver -> txCounter++;
	 }

	 if ( Uart_driver-> txCounter < Uart_driver->txLen)
	 { 	// Transmision
		uartTxWrite(Uart_driver->uartValue, mensaje[Uart_driver->txCounter]);
		Uart_driver -> txCounter ++;
	 }

	 if ( Uart_driver-> txCounter == Uart_driver->txLen)
	 {
		 uartTxWrite(Uart_driver->uartValue, '\r');
		 uartTxWrite(Uart_driver->uartValue, '\n');
		 uartCallbackClr(Uart_driver->uartValue, UART_TRANSMITER_FREE); // Deshabilito la interrupcion de Tx
		 //xTimerStartFromISR(Uart_driver->onTxTimeOut, &xTaskWokenByReceive); // Inicio el timer de timeout Tx
		 //	QMPool_put_fromISR(Pool_memoria, (tMensaje *)p);
		 // Libero el bloque de memoria que ya fue trasmitido
		 QMPool_put( &Pool_memoria, p );
		 Uart_driver -> rxLen = 0;
		 Uart_driver-> txCounter = 0;
	 }

	 taskEXIT_CRITICAL_FROM_ISR( uxSavedInterrumptStatus );

	 // Evaluamos el pedido de cambio de contexto necesario
	 if ( xTaskWokenByReceive != pdFALSE )
	 {
		 taskYIELD();
	 }

 }


