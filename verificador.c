/*
		FUNCIONES PARA DISPENSADOR																*
*/
#include<verificador.h>
#include <reg51.h>

/*funciones prototipo externas */

extern void EscribirCadenaSoft_buffer(unsigned char *buffer,unsigned char tamano_cadena);
extern void EscribirCadenaSoft(unsigned char tamano_cadena,unsigned char tipo);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_txt_Tibbo(unsigned char * str);
extern unsigned char  ValidaSensoresPaso(void);
extern void send_portERR(unsigned char cod_err);
extern void Debug_chr_Tibbo(unsigned char Dat);
extern void Debug_HexDec(unsigned char xfc);
extern char check_fechaOut(char *buffer);
extern unsigned char Dir_board();
extern void PantallaLCD(unsigned char cod_msg);
extern void Trama_pto_Paralelo_C_s(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);
extern void Cmd_LPR_Salida(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);
extern void Trama_pto_Paralelo(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void Trama_pto_Paralelo_P(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void analiza_tiempo(char *buffer,unsigned int Val_DctoMinutos);
extern void Trama_pto_Paralelo_new(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void Block_read_Clock_Hex(unsigned char *datos_clock);
extern void sel_Pulsa(void);
extern void Cmd_Lpr_Int();
extern void Delay_10ms(unsigned int cnt);
/*io sensores */

sbit DataIn = P1^1;					//	dato de las entradas		
sbit sel_A = P3^5;					//Pulsador												*
sbit sel_B = P3^6;					//Entrada Sensor 2										*
sbit sel_C = P3^7;					//Entrada Sensor 1										*

sbit lock = P1^7;						//Relevo 	
sbit Rele_Atasco = P0^3;				//Rele de on/off del verificador o transporte
sbit led_err_imp = P0^2;			//Error 	
/*variables externas*/

extern unsigned char g_cEstadoComSoft;
extern unsigned char ValTimeOutCom;
//extern unsigned char g_cEstadoComSeqMF;
extern unsigned char g_cContByteRx;
extern unsigned char xdata Buffer_Rta_Lintech[];
extern int ID_CLIENTE;
extern int COD_PARK;
extern unsigned int T_GRACIA;																				/*tiempo de gracia del parqueo*/
extern unsigned char Timer_wait;
extern unsigned int  SIN_COBRO;
extern unsigned	char 	Tarjeta_on;
extern unsigned char  CardAutomatic;	
extern unsigned char cnt__ask_off;

																										/*variable q define expedicion de tarjetas 1= automatico 0= a boton configurable en eeprom*/
/*externo bit*/

extern bit aSk;
extern bit buffer_ready;
extern unsigned char USE_LPR;

/*----------------------------------------------------------------------------
Definiciones de sequencias de verificador y expedidor
------------------------------------------------------------------------------*/

#define SEQ_INICIO											0X00	
#define SEQ_RESPUESTA_TRASPORTE					0X01
#define SEQ_RTA_CARD_SENSOR							0x02
#define SEQ_MOVER_CARD_RF								0x03

#define SEQ_RTA_SEL_STACKER		0X02
#define SEQ_MF_CHECK_STATUS		0X03
#define SEQ_MF_LINTECH				0x04
#define SEQ_MF_VERIFY					0x05
#define SEQ_RD_S1B1		 				0x06
#define SEQ_RTA_S1B1		 			0x07
#define SEQ_RD_S1B2 					0x08
#define SEQ_WR_S1B2						0x09
#define SEQ_WR_S1B0						0x0A

#define SEQ_MF_S2							0x0B
#define SEQ_WR_PLATE					0x0C
#define SEQ_RTA_S2B0					0x0D


#define SEQ_MF_FINALIZA				0x0E
#define SEQ_MF_FINAL					0X0F
#define SEQ_MF_FINAL_ENTREGA	0X10
#define SEQ_VACIO							0x11
#define SEQ_FINALIZADO				0x12

#define SEQ_RTA_CAPTURE				0X14
/*NO USADOS EN EL MOMENTO*/
#define SEQ_RTA_CARD_POS1			0x19	/*no usada temporal tomar decision de borrar*/
#define SEQ_CHECK_STATUS			0X13

#define SEQ_REQUEST						0x15
#define SEQ_RD_S1B0 					0x16
#define SEQ_RD_S1B0_EJECT			0x17
#define SEQ_CARD_INSERCION		0X18

#define SEQ_EXPULSAR_TARJ			0x20
#define SEQ_EXPULSAR_CHECK 		0x21
#define SEQ_EXPULSAR 					0x22
#define SEQ_EXPULSAR_FROM			0x23

/*----------------------------------------------------------------------------
 definiciones de lintech en la inicializacion de expedidor o verificador
------------------------------------------------------------------------------*/

#define INICIA_LINTECH					0x30
#define	SEQ_CAPTURE_DATOS_INI		0x31
#define GRABA_EEPROM						0x32
#define SEQ_CAPTURA_OK_EEPROM		0X33
#define FIN_OK									0x00


#define True										0x01
#define False										0x00
/*----------------------------------------------------------------------------
 Definicion de datos asociados al buffer de resetcion del dato del trnasporte 

------------------------------------------------------------------------------*/


#define ON		1
#define OFF		0
/*----------------------------------------------------------------------------
 ERRORES pto serie 

------------------------------------------------------------------------------*/
#define REENVIA_TRAMA			0
#define ESPERA_MAS_TIEMPO	1
/*----------------------------------------------------------------------------
 definiciones de lintech en la inicializacion de expedidor o verificador
 funcion usada en lintech que me debuelve la respuesta de comunicacion del expedidor
 o verificador Trama_Validacion_P_N()
RSPT_TRP_OK							(0) respuesta_transporte_oksignifica que la trama es valida y continua en el proceso
NO_RSPD_TRP_PTO_COM			(1) no_responde_trasnporte_pto_com se cumple el tiempo de espera
ESPR_RSPT_TRP_TRAMA			(2)Espera_respuesta_transporte_trama
ERROR_TRP_TRAMA					(3) error_transpote_trama
------------------------------------------------------------------------------*/
#define 	RSPT_TRP_OK						0
#define 	NO_RSPD_TRP_PTO_COM		1
#define 	ESPR_RSPT_TRP_TRAMA		2
#define 	ERROR_TRP_TRAMA				3


/*----------------------------------------------------------------------------
 definiciones de lintech en el comando Check_Status
------------------------------------------------------------------------------*/

#define	SENSOR_DETAIL				0x31
#define SENSOR_NORMAL				0x30

/*------------------------------------------------------------------------------
 definiciones de lintech en el comando Card_Insercion
------------------------------------------------------------------------------*/

#define Habilita			 	0x30
#define Inhabilita			0x31

/*------------------------------------------------------------------------------
Definicion de Lintech en el comando Inicializa
------------------------------------------------------------------------------*/

#define	TO_FRONT				'0'
#define	CAPTURE_BOX			'1'
#define	SIN_MOVIMIENTO	'3'

/*------------------------------------------------------------------------------
Definicion de Lintech en el comando mover tarjeta (Mov_Card)
MovPos_Front				'0'		-> mueve  tarjeta al frente pero no la suelta
MovPos_IC						'1'		->mueve la tarjeta en posicion de read/write IC
MovPos_RF						'2'		->mueve tarjeta a RF card read/write
MovPos_Capture			'3'		->captura la tarjeta
MovPos_EjectFront		'9'		->lanza la tarjeta del mecanismo

------------------------------------------------------------------------------*/

#define 	MovPos_Front				'0'		
#define 	MovPos_IC						'1'
#define  	MovPos_RF						'2'
#define  	MovPos_Capture			'3'
#define 	MovPos_EjectFront		'9'

/*------------------------------------------------------------------------------
Definicion de la trama Lintech de las respuestas de los cmd
------------------------------------------------------------------------------*/

#define Pos_Length					3
#define Pos_TipoResp				4
#define Pos_St0							7
#define Pos_St1							8
#define Pos_St2							9
#define Pos_IniDatMF				0x0a
#define	Card_type_H					0x0a
#define	Card_type_L					0x0b
/*------------------------------------------------------------------------------
Definicion del estado de st0,st1,st2 de la trama Lintech 
NO_CARDS_IN_MCNSM	-> No tiene tarjetas en el mecanismo 			sto=0
CARD_IN_MOUTH			-> hay una tarjeta dentro del bessel 			st0=1
CARD_OK_READ_RF		-> Tarjeta en posicion para leer/escribir dentro del mecanismo st0=2
NO_HAVE_CARDS			-> no tiene tarjetas en el deposito st1=0
LOW_NIVEL_CARDS		-> nivel bajo de tarjetas en el deposito	st1=1
FULL_CARD					-> Deposito de tarjetas esta lleno				st1=2
------------------------------------------------------------------------------*/


#define	NO_CARDS_IN_MCNSM		'0'
#define	CARD_IN_MOUTH				'1'
#define	CARD_OK_READ_RF			'2'
#define	NO_HAVE_CARDS				'0'
#define	LOW_NIVEL_CARDS			'1'
#define	FULL_CARD						'2'
/*----------------------------------------------------------------------------
Comprobacion automatica del tipo de tarjeta 
checking RF card type
Card_type_H		Card_type_L	 explicacion
    0							0						tarjeta desconocida
		1							0						MF50
		1							1						MF70
		1							2						MF_UL
		2							0						TYPE_A_CPU
------------------------------------------------------------------------------*/
#define MF50_HIGH							'1'
#define MF50_LOW							'0'

/*----------------------------------------------------------------------------
definicion de recepcion serial 
------------------------------------------------------------------------------*/

#define  ESPERA_RX 					0  					//espera el primer cmd de recepcion del verificado 

/*----------------------------------------------------------------------------
tiempo de delay entre funciones
------------------------------------------------------------------------------*/

#define 	TIME_CARD					100		//50


/*----------------------------------------------------------------------------
definicion de datos de trama lintech
------------------------------------------------------------------------------*/

#define 	ETX								03
#define 	STX_LINTECH				0xf2

/*----------------------------------------------------------------------------
msj de lcd tarjeta y lcd serie
------------------------------------------------------------------------------*/

#define ERROR_LOOP							0XE0
#define TARJETA_INVALIDA				0XE1
#define TARJETA_SIN_FORMATO	    0xDF
#define ERROR_COD_PARK					0XE5
#define SIN_INGRESO							0XE6
#define SIN_PAGO								0XE7
#define EXCEDE_GRACIA						0XE8
#define GRACIAS									0XFF

#define NO_CARD									0xfa	
#define LOW_CARD								0x01
#define ATASCADO								0x02
#define AUDIO_ENTRADA			0XA0
#define AUDIO_CAJA				0XA1
#define AUDIO_GRACIAS			0XA2
/*----------------------------------------------------------------------------
definiciones para, el debuger. saber si la trama es enviada, o la trama es de respuesta
------------------------------------------------------------------------------*/

#define 	ENVIADOS					0X0
#define		RESPUESTA					0X01

/*----------------------------------------------------------------------------
definiciones de la tarjeta MF tipo de cliente esto esta en la posicion (0) de la memoria MF
(0) si el dato es cero esta inactiva
(1) activa o ROTACION

------------------------------------------------------------------------------*/

#define INACTIVA					0x00
#define ROTACION 					0x01

/*----------------------------------------------------------------------------
posicion de  MF  bloque 1 sector 1
(0) tipo de tarjeta 
(01) el id del cliente
(03)codigo del parqueadero	
------------------------------------------------------------------------------*/
#define 	MF_TIPO_TARJETA		0X00
#define 	MF_ID_CLIENTE			0x01
#define		MF_COD_PARK				0x03

/*----------------------------------------------------------------------------
posicion de  MF bloque 2 sector 1
(00) donde esta grabado la fecha de entrada (año,mes,dia,hora,minutos) estan en hex  
(0b) donde esta grabado la fecha de salida (año,mes,dia,hora,minutos) estan en hex 
------------------------------------------------------------------------------*/

#define 	MF_FECHA_INT			0X00				/*año,mes,dia,hora,minutos*/


#define		MF_DCTO						0x05				/*Tipo de descuento (00)sin descuento, (01xx xxxx) 0x40 fija fecha de salida,
																					10xx xxxx dcto por porcentaje xx xxxx= valor del porcentaje, 11xx xxxx dcto por dinero */
#define		MF_LSB						0x06

#define   MF_TIPO_VEHICULO	0x08							/*tipo vehiculo 00 carro, 01 moto, 02 bicicleta, 04 tractomula*/

#define		MF_IN_PAGO				0x09
#define 	MF_APB						0x0A						/*antipassback 00 inicializado, 01 IN, 02 OUT, 03 NO USA*/

#define		MF_FECHA_OUT			0X0B				/*año,mes,dia,hora,minutos*/

/*tipo de vehiculo*/


#define AUTOMOVIL						0X00
#define MOTO								0X01
/*----------------------------------------------------------------------------
Definicion de varaibles globales del objeto
------------------------------------------------------------------------------*/

static unsigned char Estado=INICIA_LINTECH;



/*------------------------------------------------------------------------------
funcion que valida la trama del verificador o transporte lintech
RSPT_TRP_OK						(0) significa que la trama es valida y sigue en el proceso
NO_RSPD_TRP_PTO_COM		(1)	falla en la respuesta por pto serie o trama invalida	
ESPR_RSPT_TRP_TRAMA		(2)	no ha recibido la trama del verificador o transporte	
ERROR_TRP_TRAMA				(3) ERROR DE TRAMA CMD (N)


------------------------------------------------------------------------------*/

char Trama_Validacion_P_N()
{
	char Trama_Validacion_P_N=ESPR_RSPT_TRP_TRAMA;																/*espera respuesta del transporte*/
	
			if ((ValTimeOutCom==1)||(buffer_ready==1))
			{
				if (buffer_ready==1)
				{
					buffer_ready=0;
						
				
					if (Buffer_Rta_Lintech[Pos_TipoResp]=='P')
					{
										
						Trama_Validacion_P_N=RSPT_TRP_OK;																				/*trama ok*/
							
					}
					else if (Buffer_Rta_Lintech[Pos_TipoResp]=='N')
					{
 						
						Trama_Validacion_P_N=ERROR_TRP_TRAMA	;																	/*error de trama*/
						
					}
					else
					{
					 	Debug_txt_Tibbo((unsigned char *) "Respuesta  DESCONOCIDA \r\n");				/*la respuesta es desconocida*/
						Trama_Validacion_P_N=ERROR_TRP_TRAMA	;		
						
					}
				}
				else
				{
			 							
					Trama_Validacion_P_N=NO_RSPD_TRP_PTO_COM;																	/*pto serie no responde */
																						
 				}

			}
			
	return Trama_Validacion_P_N;
}
/*------------------------------------------------------------------------------
funcion de error de respuesta pto serie

variable en uart. cnt__ask_off=cuenta los numeros de error pto serie y reset de transporte 
se limpia cuando llega la respuesta del transporte. 
Rele_Atasco = es un I/O del Mc que activa / inhabilita el rele
ON		(1) esta activo 
OFF		(0) inhactivo 
REENVIA_TRAMA		1
ESPERA_MAS_TIEMPO	0
cnt_espera_ask_on= cuenta el tiempo de ASK que esta activo,sin que le llegue
									 la trama completa, al  5 tiempos borra el ask y retrasmite el cmd

error_rx_pto= (0)= ESPERA_MAS_TIEMPO nos da mas tiempo para esperar la trama,
						= (1)= REENVIA_TRAMA reenvia la trama al transporte

cnt__ask_off= varible global esta definida en pto serie y es un contador , que 
se limpia cada vez que la trama a llegado completa y validada

aSk= significa que llego al pto serie el 06 = ask donde el transporte dice que a recibido el cmd
esta ejecutando y nos enviara la respuesta,necesita tiempo esta bit esta definino en el modulo uart es global

------------------------------------------------------------------------------*/
unsigned char error_rx_pto(void)
{

static unsigned cnt_espera_ask_on=0;
unsigned char error_rx_pto=ESPERA_MAS_TIEMPO;

		
					
			Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");
			Debug_chr_Tibbo	(cnt__ask_off);	
			Debug_chr_Tibbo	(cnt_espera_ask_on);
			Debug_chr_Tibbo	(error_rx_pto);
			Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");
			if (aSk==OFF)
			
			{	
					cnt__ask_off++;																																		/*cuento el error*/																																
				if(cnt__ask_off>=10)
				{																																										/*no contesta debe reset el transporte*/
					Rele_Atasco=ON;																																		/*off el rele de reset del verificador*/		
					Delay_10ms(110);
					cnt__ask_off=0;																																		/*limpio ls errores*/
					cnt_espera_ask_on=0;
					error_rx_pto=ESPERA_MAS_TIEMPO;																										/**/
					Rele_Atasco=OFF;	
					Delay_10ms(110);																																	/*On el rele de reset del verificador*/		
					ValTimeOutCom=TIME_CARD;
				}
				else;
				{
					error_rx_pto=REENVIA_TRAMA;																												/*1 reenvia trama*/
					ValTimeOutCom=TIME_CARD;
				}
			}																																											/*aSk esta activo */
			else
			{
					cnt_espera_ask_on++;																															/*cuento n tiempos de ask para recibir el total de la trama*/
				if(cnt_espera_ask_on>=3)
				{
					cnt__ask_off=0;																																		/*paso tiempo de espera y no se completo la trama 
																																														limpio los reg y reenvio la trama y ask=off*/
					cnt_espera_ask_on=0;
					error_rx_pto=REENVIA_TRAMA;
					aSk=OFF;
					ValTimeOutCom=TIME_CARD;
				}
				else
				{
					cnt__ask_off=0;																																		/*damos tiempo de espera de la trama del transporte*/
					error_rx_pto=ESPERA_MAS_TIEMPO;;
					ValTimeOutCom=TIME_CARD;
				}
			}
				return (error_rx_pto);
}
/*------------------------------------------------------------------------------
Funcion de respuesta a los cmd del transporte
Retorna rta_cmd_transporte el estado en que queda
Recibe tres datos unos es el 
Estado_futuro= significa que la respuesta fue ok
Estado_Error= en caso de que aya un error a que estado envia el codigo
Estado_Actua= No ha llegado la respuesta y sigue en el mismo estado
RSPT_TRP_OK						(0) significa que la trama es valida y sigue en el proceso
NO_RSPD_TRP_PTO_COM		(1)	falla en la respuesta por pto serie o trama invalida	
ESPR_RSPT_TRP_TRAMA		(2)	no ha recibido la trama del verificador o transporte	
ERROR_TRP_TRAMA				(3) ERROR DE TRAMA CMD (N)

(0)= ESPERA_MAS_TIEMPO nos da mas tiempo para esperar la trama,
(1)= REENVIA_TRAMA reenvia la trama al transporte
------------------------------------------------------------------------------*/
unsigned char   rta_cmd_transporte(unsigned char Estado_futuro, unsigned char Estado_Error, unsigned Estado_Actual)
{
	unsigned char temp;
	unsigned char EstadoComSeqMF;
	
			
if((temp=Trama_Validacion_P_N())!=RSPT_TRP_OK	)
		{
			if(temp==ESPR_RSPT_TRP_TRAMA)																													/*no he recibido respuesta espero*/
			{
			EstadoComSeqMF=Estado_Actual;																											/*SEQ_RTA_CARD_POSno ha respondido*/
			}	
			else if (temp==ERROR_TRP_TRAMA)
			{
			Debug_txt_Tibbo((unsigned char *) "RTA_CMD_ERROR\r\n\r\n");											/* trama no valida respuesta incorrecta falla en la escritura */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/	
			EstadoComSeqMF=Estado_Error;																												/*SEQ_INICIO (3) Trama invalida cmd (N)reenvio cmd*/	
			}			
			else
			{
			/*Dispensador No Responde PTO SERIE ...*/
		
				if(temp=error_rx_pto()==ESPERA_MAS_TIEMPO)
				{
					EstadoComSeqMF=Estado_Actual;
				}																																											/*SEQ_RTA_CARD_POS;*/
				else 																																									//(temp=error_rx_pto()==REENVIA_TRAMA)
				{
					EstadoComSeqMF=Estado_Error;																										/*SEQ_INICIO*/
				}																									
			}				
		}
		else
	  {
			EstadoComSeqMF=Estado_futuro;
		}
	return EstadoComSeqMF;
}

unsigned char Ingreso_Vehiculo(void)
	{
	if((ValidaSensoresPaso())!=False)	 																							// valido los sensor de piso
		{
			if (CardAutomatic==True)																										//se pregunta si expide la tarjeta automatica o por presionar el boton
				{
					Debug_txt_Tibbo((unsigned char *) "Tarjeta: Automatico");								//expide tarjetas automatico con presencia
					Mov_Card(MovPos_RF);																										// muevo la tarjeta hasta el lector de rf
					Estado=SEQ_RTA_SEL_STACKER;	  																					// valido el cmd enviado al verificador
				 }
				 else
						{
							sel_Pulsa();																													//se valida el pulsador en hardware
						 	if (DataIn!=True)  		  
								{
									Debug_txt_Tibbo((unsigned char *) "Pulsador Activo");							//el pulsador fue presionado
									Mov_Card(MovPos_RF);																							//muevo tarjeta hasta el lector de RF
									Estado=SEQ_RTA_SEL_STACKER;																				//valido el cmd enviado al verificador
								}
							else
									{
										Estado=SEQ_RTA_CARD_SENSOR;
									}
						
					  }	
		}
		else
				{	
					/*no hay vehiculo en los sensores se hace el loop otra vez */
					Estado=SEQ_INICIO;	
				}
	return Estado;
}
 
unsigned char	Responde_Estado_Sensores_Transporte()
{
			Debug_txt_Tibbo((unsigned char *) "RTA_CARD_POS OK\r\n\r\n");													// trama valida Habilitado 
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														//imprimo la trama recibida
			if (Buffer_Rta_Lintech[Pos_St0]==NO_CARDS_IN_MCNSM)																	// CANAL LIBRE	  no tiene tarjetas en el mecanismo
			{
					
				if	((Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS)||(Buffer_Rta_Lintech[Pos_St1]==FULL_CARD	)) 	//  se detecta la tarjeta en la boca TARJETA EN BEZZEL
				{
					if (Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS	) 																				// nivel de tarjetas
						{
							Debug_txt_Tibbo((unsigned char *) "Nivel bajo de tarjetas\r\n\r\n");				// nivel bajo de tarjetas 
							send_portERR(LOW_CARD);																											//envio msj al primario
							PantallaLCD('a');																														//envio msj por la raspberry nivel bajo de tarjetas
								//msj						
						 }
						 else
								{
								 Estado=Ingreso_Vehiculo();
								/*respuesta ok*/
								}
				}	
				else 
						{
							/*dispensador no posee tarjetas*/
							Debug_txt_Tibbo((unsigned char *) "Dispensador SIN TARJETAS...");	
							//msj a tibbo de no card	msj por diseñar	
							send_portERR(NO_CARD);																											// se envia msj al uC principal, visualiza en el lcd que no hay tarjetas
							Estado=SEQ_INICIO;																								//inicio el loop
						 }
			}
			else if (Buffer_Rta_Lintech[Pos_St0]==CARD_IN_MOUTH)	 																		//  se detecta la tarjeta en la boca TARJETA EN BEZZEL
						{
							/*hay una tarjeta en la boca del verificador */
							Debug_txt_Tibbo((unsigned char *) "Tarjeta en la boca");											//se envia msj al debuger q hay tarjeta en la boca
							Card_Insercion(Habilita);																											//se habilita recepcion de tarjetas por boca
							Estado=SEQ_RTA_CAPTURE;																						//se trabaja mensual				
						}
						else
								{
									Estado=SEQ_INICIO;	
								}
	return Estado;	
}
/*------------------------------------------------------------------------------
Secuencia de los cmd de inicio (reset dispositivo y graba eeprom)

funcion  de los cmd de inicio (reset dispositivo y graba eeprom) retorna un (00) cuando a terminado exitoso
------------------------------------------------------------------------------*/

unsigned char  Secuencia_inicio_expedidor(void)
{
	char temp;
	switch (Estado)
	{
		case INICIA_LINTECH:

					
				Inicializa(SIN_MOVIMIENTO);	   																		//Inicio el transporte sin movimiento
				Estado=SEQ_CAPTURE_DATOS_INI;																			// entra a validar la respuesta del transporte
			  
 			
			return (Estado);
		break;


/*------------------------------------------------------------------------------
Envia la primera trama de inicializacion del transporte sin movimiento
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/
			
		case SEQ_CAPTURE_DATOS_INI:

			if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			Estado=SEQ_CAPTURE_DATOS_INI;																													/*(2)no ha respondido*/
			}	
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "INICIALIZA SIN MOVIMIENTO ERROR\r\n\r\n");					/* trama no valida*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/	
			Estado=INICIA_LINTECH;																																/* (3) Trama invalida cmd (N)*/	
			}
			else
			{
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");
				Estado=INICIA_LINTECH;																															/*(1) no responde el pto*/	
			}
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "INICIALIZA SIN MOVIMIENTO OK\r\n\r\n");						/* trama valida Habilitado */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/
			Estado=GRABA_EEPROM;																																	/*(0) respuesta ok*/
		}			
		return (Estado);
 		break;
			
/*------------------------------------------------------------------------------
Envia el cmd de grabar claves a la eeprom del transporte 
------------------------------------------------------------------------------*/
			
			case GRABA_EEPROM:
				
				Dwload_EEprom();	   																																//envio el cmd de grabar la eeprom
				Estado=SEQ_CAPTURA_OK_EEPROM;																												// entra a validar la respuesta del transporte
			
 			

			return (Estado);
				break;
	
/*------------------------------------------------------------------------------
se analiza la respuesta 
			(0) LA RESPUESTA ES OK
			(1) NO RESPONDE EL PTO SERIE
			(2) NO RECIBIDO LA TRAMA 
			(3) ERROR DE TRAMA CMD (N)
------------------------------------------------------------------------------*/
			
		case SEQ_CAPTURA_OK_EEPROM:
		
		if((temp=Trama_Validacion_P_N())!=0)
		{
			if(temp==2)
			{
			Estado=SEQ_CAPTURA_OK_EEPROM;																													/*no ha respondido*/
			}
			else if (temp==3)
			{
			Debug_txt_Tibbo((unsigned char *) "DWLOAP EEPROM ERROR\r\n\r\n");											/* trama no valida respuesta incorrecta falla en la escritura de la clave*/
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/	
			Estado=INICIA_LINTECH;																																/* (3) Trama invalida cmd (N)*/	
			}			
			else
			{
				Debug_txt_Tibbo((unsigned char *) "Dispensador No Responde PTO SERIE ...\r\n\r\n");
				Estado=INICIA_LINTECH;																															/*NO RESPONDE PTO SERIE */
			}				
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "DWLOAP EEPROM OK\r\n\r\n");												/* trama valida Habilitado */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/
			Estado=FIN_OK;																																				/*respuesta ok clave grabada con exito*/
		}			

		return (Estado);
 		break;

/*------------------------------------------------------------------------------
Fin de la secuencia de comandos con exito 
------------------------------------------------------------------------------*/		
			
				case FIN_OK:

				return (Estado);
		break;
			
					
				default:
				return Estado=INICIA_LINTECH;	
				break;
	}
}

/*------------------------------------------------------------------------------
Prosedimiento  q hace paso a paso la secuencia de la MF
atributos
ValTimeOutCom = tiempo que  espera para recibir datos pto serie
Tarjeta_on = detecto vehiculo en loop y tiene tarjeta en boca
g_cEstadoComSeqMF = nos dice en el estado que nos encontramos dentro del seguimiento
------------------------------------------------------------------------------*/

unsigned char SecuenciaExpedidorMF(unsigned char Estado_Comunicacion_Secuencia_MF)
{
//	unsigned char temp;
//	unsigned char sen_on_off;
//	static unsigned char buffer_S1_B0[17];
//	static unsigned char buffer_S1_B1[17];
//	static unsigned char buffer_S1_B2[17];

//	unsigned char clock_temp[6];
	

	switch (Estado_Comunicacion_Secuencia_MF)
	{
//***********************************************************************************************
		
		case SEQ_INICIO:

			if (ValTimeOutCom==1)
			{
				//Tarjeta_on=0;
				Rele_Atasco=OFF;																																		/*activo el rele de reset del verificador logica negativa*/		
				Check_Status(SENSOR_NORMAL);																															/* se pregunta al transporte en q estado estan las TI*/
				Estado_Comunicacion_Secuencia_MF=SEQ_RESPUESTA_TRASPORTE;																									// entra a validar la respuesta del transporte
			  
 			}
 
 		break;
		case	SEQ_RESPUESTA_TRASPORTE:
			Estado_Comunicacion_Secuencia_MF=rta_cmd_transporte(SEQ_RTA_CARD_SENSOR,SEQ_INICIO,SEQ_RESPUESTA_TRASPORTE);
			Debug_txt_Tibbo((unsigned char *) "Estado_Comunicacion_Secuencia_MF=");											/* trama no valida respuesta incorrecta falla en la escritura */
			Debug_chr_Tibbo(Estado_Comunicacion_Secuencia_MF);
			Debug_txt_Tibbo((unsigned char *) "\r\n\r\n");	
			
		break;
/*------------------------------------------------------------------------------
vengo de seq_inicio donde preguntamos en el estado q se encuentra las tarjetas
			en esta caso:
			Analizo la trama y valido
			
RSPT_TRP_OK						(0) significa que la trama es valida y sigue en el proceso
NO_RSPD_TRP_PTO_COM		(1)	falla en la respuesta por pto serie o trama invalida	
ESPR_RSPT_TRP_TRAMA		(2)	no ha recibido la trama del verificador o transporte	
ERROR_TRP_TRAMA				(3) ERROR DE TRAMA CMD (N)

si la trama es correcta validamos si hay tarjetas en el expedidor
Buffer_Rta_Lintech[Pos_St0]=='0'	el canal esta libre no tiene tarjeta	
(Buffer_Rta_Lintech[Pos_St1]=='1') (1)nivel bajo de tarjetas (x)tarjetas ok			
(0)= ESPERA_MAS_TIEMPO nos da mas tiempo para esperar la trama,
(1)= REENVIA_TRAMA reenvia la trama al transporte
	
			

------------------------------------------------------------------------------*/			
		
			
		case SEQ_RTA_CARD_SENSOR:
			Debug_txt_Tibbo((unsigned char *) "RTA_CARD_POS OK\r\n\r\n");													// trama valida Habilitado 
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);	
			Estado_Comunicacion_Secuencia_MF=Responde_Estado_Sensores_Transporte();	
			ValTimeOutCom=TIME_CARD;
		break;
		case SEQ_MOVER_CARD_RF:
			Mov_Card(MovPos_RF);
		break;
	
/*-----------------------------------------------------
respuesta ok hacemos la validacion de los niveles de tarjetas 
NO_CARDS_IN_MCNSM	-> No tiene tarjetas en el mecanismo 			sto=0
CARD_IN_MOUTH			-> hay una tarjeta dentro del bessel 			st0=1
CARD_OK_READ_RF		-> Tarjeta en posicion para leer/escribir dentro del mecanismo st0=2
NO_HAVE_CARDS			-> no tiene tarjetas en el deposito st1=0
LOW_NIVEL_CARDS		-> nivel bajo de tarjetas en el deposito	st1=1
FULL_CARD					-> Deposito de tarjetas esta lleno				st1=2
-------------------------------------------------------------		*/
	/*	else
		{
		Responde_Estado_Sensores_Transporte()
		{
			Debug_txt_Tibbo((unsigned char *) "RTA_CARD_POS OK\r\n\r\n");													// trama valida Habilitado 
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														//imprimo la trama recibida
				if (Buffer_Rta_Lintech[Pos_St0]==NO_CARDS_IN_MCNSM)																	// CANAL LIBRE	  no tiene tarjetas en el mecanismo
				{
					
						if	((Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS)||(Buffer_Rta_Lintech[Pos_St1]==FULL_CARD	)) 	/*  se detecta la tarjeta en la boca TARJETA EN BEZZEL
						{
						  if (Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS	) 																				// nivel de tarjetas
							{
								Debug_txt_Tibbo((unsigned char *) "Nivel bajo de tarjetas\r\n\r\n");				// nivel bajo de tarjetas 
								send_portERR(LOW_CARD);																											//envio msj al primario
								PantallaLCD('a');																														//envio msj por la raspberry nivel bajo de tarjetas
								//msj						
							 }
							 else
							 {
								/*respuesta ok*/
/*funcion q valide los sensores y verifique si expide tarjetas automatica o por boton 
																																																*/								
		/*						if((sen_on_off=ValidaSensoresPaso())!=0)	 																	// valido los sensores
								{
										if (CardAutomatic==1)																										//se pregunta si expide la tarjeta automatica o por presionar el boton
										{

											Debug_txt_Tibbo((unsigned char *) "Tarjeta: Automatico");							//expide tarjetas automatico con presencia
											Mov_Card(MovPos_RF);																									// muevo la tarjeta hasta el lector de rf
											g_cEstadoComSeqMF=SEQ_RTA_SEL_STACKER;	  														// valido el cmd enviado al verificador
						  					
							 			}
										else
										{
										 	sel_Pulsa();																													//se valida el pulsador en hardware
						 						if ((DataIn==0))  		  
												{
											   											
													Debug_txt_Tibbo((unsigned char *) "Pulsador Activo");							//el pulsador fue presionado
													Mov_Card(MovPos_RF);																							//muevo tarjeta hasta el lector de RF
													g_cEstadoComSeqMF=SEQ_RTA_SEL_STACKER;														//valido el cmd enviado al verificador
												}
												else
												{
													g_cEstadoComSeqMF=SEQ_RTA_CARD_SENSOR;
												}
						
										}	
								}
							
								else
								{	
									/*no hay vehiculo en los sensores se hace el loop otra vez */
	/*								g_cEstadoComSeqMF=SEQ_INICIO;	
								}
							}
						 }	
						 else 
						 {
							 /*dispensador no posee tarjetas*/
	/*						Debug_txt_Tibbo((unsigned char *) "Dispensador SIN TARJETAS...");	
							//msj a tibbo de no card	msj por diseñar	
							send_portERR(NO_CARD);																											// se envia msj al uC principal, visualiza en el lcd que no hay tarjetas
							g_cEstadoComSeqMF=SEQ_INICIO;																								//inicio el loop
						 }
				}
				else if (Buffer_Rta_Lintech[Pos_St0]==CARD_IN_MOUTH)	 																		//  se detecta la tarjeta en la boca TARJETA EN BEZZEL
				{
					/*hay una tarjeta en la boca del verificador */
/*					Debug_txt_Tibbo((unsigned char *) "Tarjeta en la boca");											//se envia msj al debuger q hay tarjeta en la boca
					Card_Insercion(Habilita);																											//se habilita recepcion de tarjetas por boca
					g_cEstadoComSeqMF=SEQ_RTA_CAPTURE;																						//se trabaja mensual				
				}
		}*/	
	//	break;

	
/*------------------------------------------------------------------------------
expulsa la tarjeta por que no pertenece a MF50
------------------------------------------------------------------------------*/			
		
				default:
				Estado_Comunicacion_Secuencia_MF=SEQ_INICIO;	
				break;	
			
	}	
	return Estado_Comunicacion_Secuencia_MF;
}


	
