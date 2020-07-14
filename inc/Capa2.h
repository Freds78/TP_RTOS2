/*
 * Capa2.h
 *
 *  Created on: 13 jul. 2020
 *      Author: freds
 */

#ifndef MYPROGRAM_TP1_P1RTOS2_INC_CAPA2_H_
#define MYPROGRAM_TP1_P1RTOS2_INC_CAPA2_H_

#include "FreeRTOS.h"
#include "sapi.h"



uint8_t serializar_Paquete( int cont);
uint8_t P_crc8(uint8_t val);
uint8_t chanceCrc8(uint8_t i);
bool_t Validacion_Paquete(int cont);
uint8_t Conversion_Paquete(int cont);

#endif /* MYPROGRAM_TP1_P1RTOS2_INC_CAPA2_H_ */
