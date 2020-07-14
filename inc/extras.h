#ifndef RTOS_2_CLASE_2_PARTE_2_INC_EXTRAS_H_
#define RTOS_2_CLASE_2_PARTE_2_INC_EXTRAS_H_

#include "driver.h"



typedef char* tMensaje;

void post(tObjeto* objeto,tMensaje* mensaje);
void get(tObjeto* objeto,tMensaje** mensaje);
void imprimir(char* mensaje);
void objeto_crear(tObjeto* objeto);

#endif /* RTOS_2_CLASE_2_PARTE_2_INC_EXTRAS_H_ */
