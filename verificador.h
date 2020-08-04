
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
unsigned char   rta_cmd_transporte(unsigned char *Estados_Expedidor);
unsigned char Responde_Lectura_Tarjeta_Sector1_Bloque1 (unsigned char *Atributos_Expedidor);
unsigned char Responde_Lectura_Tarjeta_Sector1_Bloque2 (unsigned char *Atributos_Expedidor);
unsigned char Responde_Write_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF);
unsigned char Responde_Write_Tarjeta_Sector1_Bloque0(unsigned char *Buffer_Write_MF);
void Armar_Trama_Tarjeta_Sector1_Bloque0(unsigned char *Buffer_Write_MF);
void Armar_Trama_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF);
unsigned char *Armar_Trama_Pto_Paralelo_Expedidor();
unsigned char Load_Secuencia_Expedidor(unsigned char *Secuencia_Expedidor,unsigned const  estadoactivo,unsigned const estadoactual,unsigned const estadofuturo);
unsigned char Disparo_Lock_Entrada_Vehiculo(unsigned char *Nombre_Mensual);
unsigned char *Armar_Trama_Monitor(unsigned char *Atributos_Expedidor);
unsigned char Entrega_Card_Captura();
unsigned char Send_Pto_Paralelo(unsigned char *Atributos_Expedidor);
unsigned char  Respuesta_Segunda_clave(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF);
void Armar_Trama_Placa(unsigned char *Buffer_Write_MF);
unsigned char Responde_Write_Tarjeta_Sector2_Bloque0(unsigned char *Buffer_Write_MF);
unsigned char Wait_Placa(unsigned char *secuencia_expedidor, unsigned char estadoactivo);
unsigned char Analiza_Presencia_Mensual();
unsigned char Captura_Expulsa();
unsigned char Valida_Vehiculo_Card_Mensual(unsigned char *Atributos_Expedidor);
unsigned char *Armar_Trama_Pto_Paralelo_Expedidor_Mensual(unsigned char *Atributos_Expedidor);
unsigned char Valida_Tipo_Tarjeta(unsigned char *Atributos_Expedidor,unsigned  char *Buffer_Write_MF );
unsigned char Dia_Pico_Placa(unsigned char * Atributos_Expedidor);
unsigned char Horarios(unsigned char * Atributos_Expedidor);
unsigned Bloque_Horario(unsigned int addr);
unsigned int Hora_Maxima(unsigned int addr);
unsigned char En_Horario(unsigned int HoraNow, unsigned int Hora_Prog,unsigned int addr);
unsigned char Tarjeta_Mensual(unsigned char *Atributos_Expedidor,unsigned  char *Buffer_Write_MF );
unsigned char Tarjeta_Rotacion(unsigned char *Atributos_Expedidor,unsigned  char *Buffer_Write_MF );


#endif	/* */

