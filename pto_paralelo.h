/*********************************************
jp
*********************************************/
/*
@file		pto_paralelo.h
@brief	header file for pto_paralelo.c
*/

#ifndef _PTO_PARALELO_H_
#define _PTO_PARALELO_H_

/*funciones prototipo*/


unsigned char recibe_port(unsigned char *buffer_port);
void  send_port(unsigned char *buffer_port, unsigned char length_char);
void Pulso_Bus(void);
void send_portERR(unsigned char cod_err);
void cond_ini_pto();
void Trama_pto_Paralelo(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
void load_and_send_info_reloj();
void load_and_send_id_cod();


#endif	/*_PTO_PARALELO_H_ */