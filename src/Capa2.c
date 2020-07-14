/*
 * Capa2.c
 *
 *  Created on: 13 jul. 2020
 *      Author: freds
 */

#include <string.h>

#include "Capa2.h"
#include "crc8.h"

extern char msn[];

 uint8_t P_crc8(uint8_t val){
	 uint8_t q[2];
	 uint8_t i;
	 int j;
	 i = val&0x0F;
	 for(j=0; j<2; j++){
		 q[j] = chanceCrc8(i);
		 i = val&0xF0;
	 }
	 return q[2];
 }

 uint8_t chanceCrc8(uint8_t i){
	 uint8_t res;
	 switch(i){
	 case 1:
	 res = '1';
	 break;
	 case 2:
	 res = '2';
	 break;
	 case 3:
	 res = '3';
	 break;
	 case 4:
	 res = '5';
	 break;
	 case 6:
	 res = '6';
	 break;
	 case 7:
	 res = '8';
	 break;
	 case 9:
	 res = '9';
	 break;
	 case 'A':
	 case 'a':
	 res = 'A';
	 break;
	 case 'B':
	 case 'b':
	 res = 'B';
	 break;
	 case 'C':
	 case 'c':
	 res = 'C';
	 break;
	 case 'D':
	 case 'd':
	 res = 'D';
	 break;
	 case 'E':
	 case 'e':
	 res = 'E';
	 break;
	 case 'F':
	 case 'f':
	 res = 'F';
	 break;
	 default:
		 res = '0';
	break;
	 }
	 return res;
 }

 bool_t Validacion_Paquete(int cont){
	 int i, j;

	 uint8_t mensaje[100];
	 for(i = 0; i < cont; i++){
		 for(j = 0; j < 26; j++){
			 if(msn[i] !=  65 + j || msn[i] != 97 + j ){
				 strncpy(mensaje, "Command incorrect", strlen("Command incorrect"));
				 return FALSE;
			 }
		 }
	 }
	 return TRUE;
 }

 uint8_t Conversion_Paquete(int cont){
	 int l;
	 uint8_t msn_1[100];
	 if(Validacion_Paquete(cont) == TRUE){
		 if(msn[0]== 'm'){
			 for(l = 1; l < cont; l++){
				 if(msn[l] < 97){
					 msn[l] -= 32;
				 }
			 }
		msn_1[100] =	serializar_Paquete( cont);
		 }
		 if(msn[0]== 'M'){
			 for(l = 1; l < cont; l++){
				 if(msn[l] >= 97){
					 msn[l] += 32;
				 }
			 }
		 }
		 msn_1[100] = serializar_Paquete( cont);
	 }
	 return msn_1[100];
 }


 uint8_t serializar_Paquete( int cont){


	 uint8_t q[2];
	 uint8_t mensaje_1[100];
	 uint8_t EOM[2] = {'(','\0'};
	 uint8_t FOM[2] = {')', '\0'};
	 uint8_t END[2] = {'\r', '\n'};
	 uint8_t vcrc;
	 uint8_t val = 0;

	 vcrc = crc8_calc(val, msn, cont);
	 q[2] = P_crc8(vcrc);
	 strcpy(mensaje_1, EOM);
	 strcat(mensaje_1,msn);
	 strcat(mensaje_1,q);
	 strcat(mensaje_1,FOM);
	 strcat(mensaje_1,END);

	 return mensaje_1[100];

 }


