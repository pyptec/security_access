/*
		FUNCIONES PARA DISPENSADOR																*
*/
#include<verificador.h>
#include <reg51.h>

/*funciones prototipo externas */

extern void EscribirCadenaSoft_buffer(unsigned char *buffer,unsigned char tamano_cadena);
extern void EscribirCadenaSoft(unsigned char tamano_cadena,unsigned char tipo);


extern void send_portERR(unsigned char cod_err);

extern char check_fechaOut(char *buffer);


extern void PantallaLCD(unsigned char cod_msg);
//extern void Trama_pto_Paralelo_C_s(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);
extern void Cmd_LPR_Salida(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);
//extern void Trama_pto_Paralelo(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
//extern void Trama_pto_Paralelo_P(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void analiza_tiempo(char *buffer,unsigned int Val_DctoMinutos);
//extern void Trama_pto_Paralelo_new(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void Block_read_Clock_Hex(unsigned char *datos_clock);

extern void Cmd_Lpr_Int();
extern void Delay_10ms(unsigned int cnt);
/*funciones prototipo string */
extern char  *strcat  (char *s1, const char *s2);
extern char  *strcpy  (char *s1, const char *s2);
extern unsigned int strlen  (const char *);
/*funciones prototipo de EEprom*/
extern unsigned char *Lee_No_Ticket();
void Incremente_Ticket();
extern unsigned char rd_eeprom (unsigned char control,unsigned int Dir); 
/*funciones prototipo del transporte MODULO io_sensores*/
extern unsigned char  ValidaSensoresPaso(void);
extern unsigned char Dir_board();
extern void sel_Pulsa(void);
/*funciones prototipo del transporte MODULO TIBBO*/
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_txt_Tibbo(unsigned char * str);
extern void Debug_chr_Tibbo(unsigned char Dat);
extern void Debug_HexDec(unsigned char xfc);
/*funciones prototipo del transporte MODULO LINTECH*/
extern void Aut_Card_check_Status(void);
extern void Check_Status(unsigned char Detalle);
extern void Dwload_EEprom (void);
extern void Mov_Card(unsigned char Posicion);
extern void Card_Insercion(char Tipo);
extern void RD_MF(unsigned char Sector, unsigned char Bloque);
extern void WR_MF(unsigned char Sector, unsigned char Bloque,unsigned char *buffer);	
extern void LoadVerify_EEprom(void);
/*funcion prototipo pto paralelo*/
void  send_port(unsigned char *buffer_port, unsigned char length_char);
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
extern unsigned char g_cContByteRx;
extern unsigned char xdata Buffer_Rta_Lintech[];
extern int ID_CLIENTE;
extern int COD_PARK;
extern unsigned int T_GRACIA;																				/*tiempo de gracia del parqueo*/
extern unsigned char Timer_wait;
extern unsigned int  SIN_COBRO;
extern unsigned	char 	Tarjeta_on;
extern unsigned char cnt__ask_off;
extern  unsigned char Tipo_Vehiculo;
																										/*variable q define expedicion de tarjetas 1= automatico 0= a boton configurable en eeprom*/
/*externo bit*/

extern bit aSk;
extern bit buffer_ready;
extern unsigned char USE_LPR;

/*----------------------------------------------------------------------------
Definiciones de sequencias de verificador y expedidor
------------------------------------------------------------------------------*/

#define SEQ_INICIO											0X00	
#define SEQ_RESPUESTA_TRANSPORTE				0X01
#define SEQ_CMD_ACEPTADO								0x02
#define SEQ_MOVER_CARD_RF								0x03
#define SEQ_CARD_INSERCION_ON						0x04
#define SEQ_TIPO_CARD										0X05
#define SEQ_LOAD_PASSWORD 							0X06
#define	SEQ_READ_SECTOR_BLOQUE					0X07
#define SEQ_WRITE_SECTOR_BLOQUE					0X08
#define	SEQ_CAPTURE_CARD								0X09
#define SEQ_CARD_INSERCION_OFF					0x0a
#define	SEQ_EXPULSAR_CARD								0x0b
#define	SEQ_LOAD_EEPROM									0x0c

/*----------------------------------------------------------------------------
Definiciones de sequencias de tareas del verificador y expedidor
------------------------------------------------------------------------------*/
#define TAREA_PRESENCIA_VEHICULAR							0X00	
#define TAREA_TIPO_TARJETA										0X01
#define TAREA_LECTURA_TARJETA_SECTOR_BLOQUE 	0x02
#define TAREA_WRITE_TARJETA_SECTOR_BLOQUE			0X03
#define TAREA_OPEN_BARRERA										0X04

/*NO USADOS EN EL MOMENTO*/

//#define SEQ_RTA_CARD_POS1			0x19	/*no usada temporal tomar decision de borrar*/
//#define SEQ_CHECK_STATUS			0X13

//#define SEQ_REQUEST						0x15
//#define SEQ_RD_S1B0 					0x16
//#define SEQ_RD_S1B0_EJECT			0x17
//#define SEQ_CARD_INSERCION		0X18

//#define SEQ_EXPULSAR_TARJ			0x20
//#define SEQ_EXPULSAR_CHECK 		0x21
//#define SEQ_EXPULSAR 					0x22
//#define SEQ_EXPULSAR_FROM			0x23

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

//#define 	ETX								03
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
(2) mensualidad
------------------------------------------------------------------------------*/
enum Tipos_MF_TIPO_TARJETA{
	INACTIVA,					
	ROTACION, 					
	MENSUALIDAD				
};
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
enum Estados_Expedidor{
 EstadoActual,
 EstadoPasado,
 EstadoFuturo,
 TareadelCmd	
};
enum expedidor {
 Sector,				
 Bloque,				
 Tipo_Tarjeta,	
 Apb,
 
};
/*tipos de APB antipassback*/
enum Tipos_MF_APB{
	
	APB_INICIADO,				
	APB_INT,							
	APB_OUT,							
	APB_NO							
};
/*tipo de vehiculo*/
/*Sectores y bloques de Mf*/
enum MF_Sector_Bloque{
	Sector_0,
	Sector_1,
	Bloque_0 = 0,
	Bloque_1 = 1,
	Bloque_2 = 2
};
/*comandos pto paralelo*/
enum CMD_Trama_Pto_Paralelo{
	STX=02,
	CMD_PTO_PARALELO_EXPEDIDOR='a',
	ETX= 03,
	NULL=0
};
enum EE_AntiPassBack{
	APB_INHABILITADO_SOFT
};	

#define AUTOMOVIL						0X00
#define MOTO								0X01

/*DATOS DE CONFIGURACION EEPROM*/
#define EE_USE_LPR							0x000A
#define EE_CARD_AUTOMATIC_BOTON	0x000f
#define EE_HABILITA_APB					0x0010
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
				}																										 																	/*SEQ_RTA_CARD_POS;*/
				else 																																									//(temp=error_rx_pto()==REENVIA_TRAMA)
				{
					EstadoComSeqMF=Estado_Error;																										/*SEQ_INICIO*/
				}																									
			}				
		}
		else
	  {
			
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);
			EstadoComSeqMF=Estado_futuro;
		}
	return EstadoComSeqMF;
}

unsigned char Ingreso_Vehiculo(void)
	{
		unsigned char CardAutomatic;	
	if((ValidaSensoresPaso())!=False)	 																							// valido los sensor de piso
		{
			CardAutomatic=rd_eeprom(0xa8,EE_CARD_AUTOMATIC_BOTON);	
			
			if (CardAutomatic==True)																										//se pregunta si expide la tarjeta automatica o por presionar el boton
				{
					Debug_txt_Tibbo((unsigned char *) "Tarjeta: Automatico\r\n");								//expide tarjetas automatico con presencia
					//Mov_Card(MovPos_RF);																										// muevo la tarjeta hasta el lector de rf
					Estado=SEQ_MOVER_CARD_RF;//SEQ_RTA_SEL_STACKER;	  																					// valido el cmd enviado al verificador
				 }
				 else
						{
							sel_Pulsa();																													//se valida el pulsador en hardware
						 	if (DataIn!=True)  		  
								{
									Debug_txt_Tibbo((unsigned char *) "Pulsador Activo\r\n");							//el pulsador fue presionado
									//Mov_Card(MovPos_RF);																							//muevo tarjeta hasta el lector de RF
									Estado=SEQ_MOVER_CARD_RF;//SEQ_RTA_SEL_STACKER;																				//valido el cmd enviado al verificador
								}
							else
									{
										Estado=SEQ_RESPUESTA_TRANSPORTE;
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
	unsigned char Estado_expedidor;
			Debug_txt_Tibbo((unsigned char *) "TAREA_PRESENCIA_VEHICULAR\r\n");													// trama valida Habilitado 
			//Debug_txt_Tibbo((unsigned char *) "Respuesta comando Check_Status \r\n");	
			//DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														//imprimo la trama recibida
			if (Buffer_Rta_Lintech[Pos_St0]==NO_CARDS_IN_MCNSM)																	  // CANAL LIBRE	  no tiene tarjetas en el mecanismo
			{
					
				if	((Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS)||(Buffer_Rta_Lintech[Pos_St1]==FULL_CARD	)) 	//  se detecta la tarjeta en la boca TARJETA EN BEZZEL
				{
					if (Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS	) 																				// nivel de tarjetas
						{
							Debug_txt_Tibbo((unsigned char *) "Nivel bajo de tarjetas\r\n");				// nivel bajo de tarjetas 
							send_portERR(LOW_CARD);																											//envio msj al primario
							PantallaLCD('a');																														//envio msj por la raspberry nivel bajo de tarjetas
								Estado_expedidor=Ingreso_Vehiculo();				
						 }
						 else
								{
									/*respuesta ok*/
								 Estado_expedidor=Ingreso_Vehiculo();
								
								}
				}	
				else 
						{
							/*dispensador no posee tarjetas*/
							Debug_txt_Tibbo((unsigned char *) "Dispensador SIN TARJETAS...\r\n");	
							//msj a tibbo de no card	msj por diseñar	
							send_portERR(NO_CARD);																											// se envia msj al uC principal, visualiza en el lcd que no hay tarjetas
							Estado_expedidor=SEQ_INICIO;																								//inicio el loop
						 }
			}
			else if (Buffer_Rta_Lintech[Pos_St0]==CARD_IN_MOUTH)	 																		//  se detecta la tarjeta en la boca TARJETA EN BEZZEL
						{
							/*hay una tarjeta en la boca del verificador */
							Debug_txt_Tibbo((unsigned char *) "Tarjeta en la boca\r\n");											//se envia msj al debuger q hay tarjeta en la boca
							//Card_Insercion(Habilita);																											//se habilita recepcion de tarjetas por boca
							Estado_expedidor=SEQ_CARD_INSERCION_ON;																						//se trabaja mensual				
						}
						else
								{
									Estado_expedidor=SEQ_INICIO;	
								}
	return Estado_expedidor;	
}

/*------------------------------------------------------------------------------
Funcion que nos dice el tipo de tarjeta que tiene en el mecanismo
Comprobacion automatica del tipo de tarjeta 
checking RF card type
Card_type_H		Card_type_L	 explicacion
    0							0						tarjeta desconocida
		1							0						MF50
		1							1						MF70
		1							2						MF_UL
		2							0						TYPE_A_CPU

#define MF50_HIGH							'1'
#define MF50_LOW							'0'
------------------------------------------------------------------------------*/
unsigned char Responde_Tipo_Tarjeta()
	{
		unsigned char Estado_expedidor;
			Debug_txt_Tibbo((unsigned char *) "TAREA_TIPO DE TARJETA\r\n");													// trama valida Habilitado 
			
	if (Buffer_Rta_Lintech[Card_type_H]==MF50_HIGH)																									/* pregunto si la tarjeta en el transporte es MF 50 */
		{
		if (Buffer_Rta_Lintech[Card_type_L]==MF50_LOW)	
			{
				Debug_txt_Tibbo((unsigned char *) "Tarjeta valida MF50\r\n");							/* trama valida son MF50*/
				Estado_expedidor=SEQ_LOAD_PASSWORD;	
																																								
			}
			else
				{
					Debug_txt_Tibbo((unsigned char *) "Tarjeta invalida no es MF50\r\n");					/* trama no valida */
					send_portERR(0xA2);																															/*error audio*/	
					send_portERR(0XE1);																															/*envio msj principal tarjeta invalidad*/
					PantallaLCD(TARJETA_INVALIDA);																									/*envio el msj por la pantalla lcd o la raspberry*/
					Estado_expedidor=SEQ_CAPTURE_CARD;																				
																											
							     
				}
		}
		else 
			{
				Debug_txt_Tibbo((unsigned char *) "Tarjeta invalida no es MF50\r\n");						/* trama no valida */
				send_portERR(0xA2);																															/*error audio*/	
				send_portERR(0xe1);	   																									//la tarjeta no es valida 
				PantallaLCD(TARJETA_INVALIDA);																											/*envio el msj por la pantalla lcd o la raspberry*/
				Estado_expedidor=SEQ_CAPTURE_CARD;
				//g_cEstadoComSeqMF=SEQ_EXPULSAR_TARJ;																								/* capturo la respuesta y regreso a chequear verificador*/			 
			}
							
		return Estado_expedidor;																																																	/*respuesta ok inicia revisando sensores*/
	}	

unsigned char  Responde_Lectura_Tarjeta_Sector1_Bloque1 (unsigned char *Atributos_Expedidor)
{
	unsigned char temp;
	unsigned char Estado_expedidor;
	unsigned char buffer_S1_B1[17];
	
			Debug_txt_Tibbo((unsigned char *) "TAREA_LECTURA_TARJETA_SECTOR1_BLOQUE1\r\n");		
																	
																
			
			if (Buffer_Rta_Lintech[Pos_Length] >=0x18)
			{
				
					for (temp=0; temp<16; ++temp)
					{
						buffer_S1_B1 [temp]=Buffer_Rta_Lintech[Pos_IniDatMF+temp];														/*almaceno la informacion de MF en un arreglo*/
					 
					}
					
					Debug_txt_Tibbo((unsigned char *) "Buffer_s1_b1\r\n");
					DebugBufferMF(buffer_S1_B1,16,RESPUESTA);
					/*Compara  MF_ID_CLIENTE y MF_COD_PARK con el codigo del parqueadero ID_CLIENTE  COD_PARK*/
					if (((buffer_S1_B1 [ MF_ID_CLIENTE])==ID_CLIENTE)&&((buffer_S1_B1 [ MF_COD_PARK] )==COD_PARK)||((ID_CLIENTE==0)&&(COD_PARK==0)))		
					{
							
						Debug_txt_Tibbo((unsigned char *) "ID_CLIENTE: ");								// posicion 1
						Debug_HexDec((buffer_S1_B1 [ MF_ID_CLIENTE]));
						Debug_txt_Tibbo((unsigned char *) "\r\n");
						
						Debug_txt_Tibbo((unsigned char *) "COD_PARK:");										// posicion 3
						Debug_HexDec(buffer_S1_B1 [ MF_COD_PARK]);
						Debug_txt_Tibbo((unsigned char *) "\r\n"); 

						Debug_txt_Tibbo((unsigned char *) "TIPO DE TARJETA: ");
						Debug_chr_Tibbo((buffer_S1_B1 [MF_TIPO_TARJETA]));
						Debug_txt_Tibbo((unsigned char *) "\r\n");
						
						*(Atributos_Expedidor + Tipo_Tarjeta)=buffer_S1_B1 [MF_TIPO_TARJETA];
						
						if 	(buffer_S1_B1 [ MF_TIPO_TARJETA]==ROTACION)					
						{
							Debug_txt_Tibbo((unsigned char *) "TIPO DE TARJETA ROTACION\r\n ");
							*(Atributos_Expedidor + Sector) = Sector_1;
							*(Atributos_Expedidor + Bloque) = Bloque_2;
																																								
							Estado_expedidor=SEQ_READ_SECTOR_BLOQUE;
							
						}
						else 
						{
							Debug_txt_Tibbo((unsigned char *) "ERROR TARJETA INVALIDA\r\n");
							send_portERR(0xe1);
							PantallaLCD(TARJETA_INVALIDA);																											/*envio el msj por la pantalla lcd o la raspberry*/
							Estado_expedidor=SEQ_CAPTURE_CARD;																							/* expulso la tarjeta */		
						}
					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "ERROR COD PARK\r\n");
						send_portERR(0XE5);
						PantallaLCD(ERROR_COD_PARK);																												/*envio el msj por la pantalla lcd o la raspberry*/
						Estado_expedidor=SEQ_CAPTURE_CARD;																								/* codigo de parqueo erro expulso la tarjeta */		
					}
			}
			else
			{
				Debug_txt_Tibbo((unsigned char *) "TARJETA SIN FORMATO\r\n");
				send_portERR(0XDF);
				PantallaLCD(TARJETA_SIN_FORMATO);																												/*envio el msj por la pantalla lcd o la raspberry*/
				Estado_expedidor=SEQ_LOAD_EEPROM;
			}
			
	return Estado_expedidor;
}	
unsigned char Responde_Lectura_Tarjeta_Sector1_Bloque2 (unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF)
{
	unsigned char temp;
	unsigned char Estado_expedidor;
	unsigned char buffer_S1_B2[17];	
			Debug_txt_Tibbo((unsigned char *) "TAREA_LECTURA_TARJETA_SECTOR1_BLOQUE2\r\n");		
			
			Estado_expedidor=SEQ_CAPTURE_CARD;													
			
			if (Buffer_Rta_Lintech[Pos_Length] >=0x18)
			{
				
					for (temp=0; temp<16; ++temp)
					{
						buffer_S1_B2 [temp] =Buffer_Rta_Lintech[Pos_IniDatMF+temp];														/*almaceno la informacion de MF en un arreglo*/
					}
					Debug_txt_Tibbo((unsigned char *) "Buffer_s1_b2\r\n");
					DebugBufferMF(buffer_S1_B2,16,RESPUESTA);
			
					Debug_txt_Tibbo((unsigned char *) "ANTIPASSBACK:");
					Debug_chr_Tibbo(buffer_S1_B2 [MF_APB]);
					Debug_txt_Tibbo((unsigned char *) "\r\n");
				
					/*antipassback 00 inicializado, 01 IN, 02 OUT, 03 NO USA*/
					*(Atributos_Expedidor + Apb)=		buffer_S1_B2 [MF_APB];
					
				
					if(((buffer_S1_B2 [ MF_APB ])==APB_OUT)||((buffer_S1_B2 [ MF_APB])==APB_INICIADO) || (rd_eeprom(0xa8,EE_HABILITA_APB)==APB_INHABILITADO_SOFT) )															
					{	
					*(Atributos_Expedidor + Sector) = Sector_1;
					*(Atributos_Expedidor + Bloque) = Bloque_2;
					Armar_Trama_Tarjeta_Sector1_Bloque2(Atributos_Expedidor,Buffer_Write_MF);
					Estado_expedidor=SEQ_WRITE_SECTOR_BLOQUE;
				
					}
					else if((buffer_S1_B2 [ MF_APB ])==APB_INT)
					{	
					Debug_txt_Tibbo((unsigned char *) "ERROR: INT ANTIPASSBACK \r\n");
					
					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "ERROR: NO ANTIPASSBACK \r\n");
					}
	
			}
		return Estado_expedidor;	
}
unsigned char Responde_Write_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF)
{
	
	Debug_txt_Tibbo((unsigned char *) "TAREA_WRITE_TARJETA_SECTOR1_BLOQUE2\r\n");	
	Debug_txt_Tibbo((unsigned char *) "Write_s1_b2\r\n");
	DebugBufferMF(Buffer_Write_MF,16,RESPUESTA);
	
	*(Atributos_Expedidor + Sector) = Sector_1;
	*(Atributos_Expedidor + Bloque) = Bloque_0;
	Armar_Trama_Tarjeta_Sector1_Bloque0(Buffer_Write_MF);
	return SEQ_WRITE_SECTOR_BLOQUE;
}
unsigned char Responde_Write_Tarjeta_Sector1_Bloque0(unsigned char *Buffer_Write_MF)
{
	unsigned char *Trama_Expedidor;
	
	Debug_txt_Tibbo((unsigned char *) "TAREA_WRITE_TARJETA_SECTOR1_BLOQUE0\r\n");	
	Debug_txt_Tibbo((unsigned char *) "Write_s1_b0\r\n");
	DebugBufferMF(Buffer_Write_MF,16,ENVIADOS);
	Trama_Expedidor=Armar_Trama_Pto_Paralelo_Expedidor();
	
	send_port(Trama_Expedidor,strlen(Trama_Expedidor));	
	Incremente_Ticket();
	Debug_txt_Tibbo((unsigned char *) "Trama_pto_paralelo\r\n");
	DebugBufferMF(Trama_Expedidor,strlen(Trama_Expedidor),ENVIADOS);
	return SEQ_EXPULSAR_CARD;
}
void  Armar_Trama_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF)
{
	
	/*fecha de ingreso se lee año,mes,dia,hora y minutos*/
	Block_read_Clock_Hex(Buffer_Write_MF);
	/*descuentos los borro*/
	*(Buffer_Write_MF +5)=0;
	*(Buffer_Write_MF +6)=0;
	*(Buffer_Write_MF +7)=0;
	if (*(Atributos_Expedidor + Tipo_Tarjeta)==ROTACION)
	{
		*(Buffer_Write_MF +8)  =Tipo_Vehiculo;//20
		*(Buffer_Write_MF +9)  =0x0f & Dir_board();
		*(Buffer_Write_MF +10) =APB_INT;
	}
	/*mensual*/
	else
	{
		if (*(Atributos_Expedidor + Apb)== APB_NO)
		 {
				*(Buffer_Write_MF +10) =APB_NO;
			}
		else 
		{
			*(Buffer_Write_MF +10) =APB_INT;
		}
	}
	/*fecha de salida maxima*/
	*(Buffer_Write_MF +11)=0;
	*(Buffer_Write_MF +12)=0;
	*(Buffer_Write_MF +13)=0;
	*(Buffer_Write_MF +14)=0;
	*(Buffer_Write_MF +15)=0;
}
void Armar_Trama_Tarjeta_Sector1_Bloque0(unsigned char *Buffer_Write_MF)
{
	/*graba serie de Ticket*/
	strcpy(Buffer_Write_MF, Lee_No_Ticket());
}
unsigned char *Armar_Trama_Pto_Paralelo_Expedidor()
{
	static unsigned char buffer[24];

	/*la trama esta compuesta de
	STX,CMD,-,NoTICKET,-,FECHAINT,-,placa,ETX*/

	buffer[0]=STX;
	buffer[1]=CMD_PTO_PARALELO_EXPEDIDOR;
	buffer[2]=NULL;
	strcat(buffer , Lee_No_Ticket());
	buffer[12]= '-';
	Block_read_Clock_Hex(buffer+13);					//leo la fecha de entrada
	buffer[13]=buffer[14]+0x030;					/*año de entrada*/
	buffer[14]=buffer[15]+0x030;					/*mes de entrada*/
	buffer[15]=buffer[16]+0x030;					/*dia de entrada*/
	buffer[16]=buffer[17]+0x030;					/*hora de entrada*/
	buffer[17]=buffer[18]+0x030;					/*minutos de entrada*/
	buffer[18]= '-';
	/*placa*/
	if (rd_eeprom(0xa8,EE_USE_LPR)!=0)
	{		
	
	buffer[19]= ' ';
	
	}
	else 
	{
		buffer[19]= ' ';
	}
	buffer[20]= ETX;
	buffer[21]= NULL;
	return buffer;
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

unsigned char SecuenciaExpedidorMF(unsigned char EstadoActivo)
{
	static unsigned char Buffer_Write_MF[17];
	static unsigned char Atributos_Expedidor[4];
	//static unsigned char Estados_Expedidor[5];

	static unsigned char EstadoActual;
	static unsigned char EstadoPasado;
	static unsigned char EstadoFuturo;
	static unsigned char TareadelCmd;

	switch (EstadoActivo)
	{
//***********************************************************************************************
		
		case SEQ_INICIO:

			if (ValTimeOutCom==1)
			{
				Rele_Atasco=OFF;																																		/*activo el rele de reset del verificador logica negativa*/		
				Check_Status(SENSOR_NORMAL);																															/* se pregunta al transporte en q estado estan las TI*/
			//	Atributos_Expedidor [ EstadoPasado ] =Atributos_Expedidor [ EstadoActivo ];
				EstadoPasado=EstadoActivo;
			//	Atributos_Expedidor [ EstadoActual ]=SEQ_CMD_ACEPTADO;
				EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;	
				// entra a validar la respuesta del transporte
				//Atributos_Expedidor [ EstadoFuturo ]=SEQ_RESPUESTA_TRANSPORTE;
			  EstadoFuturo=SEQ_RESPUESTA_TRANSPORTE;
			//	Atributos_Expedidor [ TareadelCmd ]=TAREA_PRESENCIA_VEHICULAR;
				TareadelCmd=TAREA_PRESENCIA_VEHICULAR;
 			}
 
 		break;
		
		case	SEQ_CMD_ACEPTADO:
			EstadoActivo=rta_cmd_transporte(EstadoFuturo,EstadoPasado,EstadoActual);
			
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
		
			
		case SEQ_RESPUESTA_TRANSPORTE:
			
			if (TareadelCmd==TAREA_PRESENCIA_VEHICULAR)
			{
			EstadoActivo=Responde_Estado_Sensores_Transporte();	
			
			}
			else if (TareadelCmd==TAREA_TIPO_TARJETA)
					{
						
						EstadoActivo=Responde_Tipo_Tarjeta();
					}
			else if (TareadelCmd==TAREA_LECTURA_TARJETA_SECTOR_BLOQUE)
					{
						if ((Atributos_Expedidor [ Sector ]== Sector_1)&& (Atributos_Expedidor [ Bloque ]==Bloque_1))
						{
							EstadoActivo=Responde_Lectura_Tarjeta_Sector1_Bloque1 (Atributos_Expedidor);
						}
						else
						{
							EstadoActivo=Responde_Lectura_Tarjeta_Sector1_Bloque2(Atributos_Expedidor,Buffer_Write_MF);
						}
					}
			else if (TareadelCmd==TAREA_WRITE_TARJETA_SECTOR_BLOQUE)
					{	
						if ((Atributos_Expedidor [ Sector ]== Sector_1)&& (Atributos_Expedidor [ Bloque ]==Bloque_2))
						{	
							EstadoActivo=Responde_Write_Tarjeta_Sector1_Bloque2 (Atributos_Expedidor,Buffer_Write_MF);
						}
						else
						{
							EstadoActivo=Responde_Write_Tarjeta_Sector1_Bloque0 (Buffer_Write_MF);
						}
					}
				else if (TareadelCmd==TAREA_OPEN_BARRERA)
					{		
					}
				else
					{
						Debug_txt_Tibbo((unsigned char *) "TAREA_3\r\n");
					}
			break;
		case SEQ_MOVER_CARD_RF:
			Mov_Card(MovPos_RF);
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_TIPO_CARD;
			
		
		break;
		case SEQ_CARD_INSERCION_ON:
			Card_Insercion(Habilita);	
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_RESPUESTA_TRANSPORTE;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_TIPO_CARD;
			TareadelCmd=TAREA_LECTURA_TARJETA_SECTOR_BLOQUE;
		break;
		case SEQ_TIPO_CARD:
			Aut_Card_check_Status();
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_RESPUESTA_TRANSPORTE;;
			TareadelCmd=TAREA_TIPO_TARJETA;
		break;
		
		case SEQ_LOAD_PASSWORD:
			LoadVerify_EEprom();
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			Atributos_Expedidor [ Sector ] = Sector_1;
			Atributos_Expedidor [ Bloque ] = Bloque_1;
		
			EstadoFuturo=SEQ_READ_SECTOR_BLOQUE;								//nueva secuencia
		break;
		case SEQ_READ_SECTOR_BLOQUE:
			RD_MF(Atributos_Expedidor [ Sector ],Atributos_Expedidor [ Bloque ]);
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_RESPUESTA_TRANSPORTE;;
			TareadelCmd=TAREA_LECTURA_TARJETA_SECTOR_BLOQUE;
		break;
		case SEQ_WRITE_SECTOR_BLOQUE:
			WR_MF(Atributos_Expedidor [ Sector ],Atributos_Expedidor [ Bloque ],Buffer_Write_MF);
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_RESPUESTA_TRANSPORTE;
			TareadelCmd=TAREA_WRITE_TARJETA_SECTOR_BLOQUE;
		break;
		case SEQ_CAPTURE_CARD:
			Mov_Card(MovPos_Capture);	
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_INICIO;
		break;
		case SEQ_CARD_INSERCION_OFF:
			Card_Insercion(Inhabilita);
		break;
		case SEQ_EXPULSAR_CARD:
			Mov_Card(MovPos_EjectFront);
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_RESPUESTA_TRANSPORTE;;
			TareadelCmd=TAREA_LECTURA_TARJETA_SECTOR_BLOQUE;
		break;
		case SEQ_LOAD_EEPROM:
			Dwload_EEprom();
			EstadoPasado=EstadoActivo;
			EstadoActual=EstadoActivo=SEQ_CMD_ACEPTADO;																									// entra a validar la respuesta del transporte
			EstadoFuturo=SEQ_READ_SECTOR_BLOQUE;
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
				EstadoActivo=SEQ_INICIO;	
				break;	
			
	}	
	return EstadoActivo;
}


	
