/*********************************************
Diseñado Ing. Jaime Pedraza
Marzo 05 de 2019
*********************************************/
/*
@file		prog.h
@brief	header file for delay.c
*/

#ifndef _PROG_H_
#define _PROG_H_


/*funcion prototipo*/
void menu(void);
void Show();
void DisplayCursor(void);
void IngresaDato(unsigned char  *buffer, unsigned char modo);
unsigned char  ValidarClave(unsigned char *buffer);
unsigned char ListaComandos(unsigned char *cmd);
void prog_id_cliente();
void prog_cod_park();
void Prog_tiempo_gracia();
void Prog_Sin_Cobro();
void Prog_debug();
void Prog_Use_Lpr();
void Prog_Comparacion_Activa();
void tipo_pantalla();
unsigned char *hash_id(unsigned char *clave);
void Prog_tarjeta_automatica_o_boton();
void Prog_AntiPassBack();
void Prog_Placa();
void Prog_Address_High_Board();
void Prog_Horarios();
void Prog_Validar_Tipo_Vehiculo_Mensual();
void Prog_Apb_Mensual();
unsigned char *Addr_Horarios();
unsigned char Prog_Horarios_on_off(unsigned int addr);
void Prog_Horario_Desde_Hasta(unsigned int addr);
void  First_Clave();

#endif	/* _PROG_H_ */