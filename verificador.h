
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
void Check_Status(unsigned char Detalle);
void Aut_Card_check_Status(void);
void Mov_Card(unsigned char Posicion);
unsigned char SecuenciaExpedidorMF(unsigned char Estado_Comunicacion_Secuencia_MF);
void Card_Insercion(char Tipo);
void Dwload_EEprom (void);
unsigned char  Secuencia_inicio_expedidor(void);
char Trama_Validacion_P_N();
void LoadVerify_EEprom(void);
void RD_MF(unsigned char Sector, unsigned char Bloque);
void WR_MF(unsigned char Sector, unsigned char Bloque,unsigned char *buffer);
unsigned char error_rx_pto(void);
unsigned char   rta_cmd_transporte(unsigned char Estado_futuro, unsigned char Estado_Error, unsigned Estado_Actual);
#endif	/* _VERIFICADOR_H_ */

