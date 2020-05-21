
/*********************************************
Diseñada por Ing. Jaime Pedraza 
Marzo 01/2019
*********************************************/
/*
@file		verificador.h
@brief	header file for verificador.c
*/

#ifndef _VERIFICADOR_H_
#define _VERIFICADOR_H_

/*funciones prototipo  */

void Inicializa(unsigned char TipoMovimiento);
unsigned char SecuenciaExpedidorMF(unsigned char Estado_Comunicacion_Secuencia_MF);
unsigned char  Secuencia_inicio_expedidor(void);
char Trama_Validacion_P_N();
unsigned char error_rx_pto(void);
unsigned char   rta_cmd_transporte(unsigned char Estado_futuro, unsigned char Estado_Error, unsigned Estado_Actual);
unsigned char Responde_Lectura_Tarjeta_Sector1_Bloque1 (unsigned char *Atributos_Expedidor);
unsigned char Responde_Lectura_Tarjeta_Sector1_Bloque2 (unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF);
unsigned char Responde_Write_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF);
unsigned char Responde_Write_Tarjeta_Sector1_Bloque0(unsigned char *Buffer_Write_MF);
void Armar_Trama_Tarjeta_Sector1_Bloque0(unsigned char *Buffer_Write_MF);
void Armar_Trama_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF);
unsigned char *Armar_Trama_Pto_Paralelo_Expedidor();



#endif	/* _VERIFICADOR_H_ */

