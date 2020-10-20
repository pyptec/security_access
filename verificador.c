/*
		FUNCIONES PARA DISPENSADOR																*
*/
#include<verificador.h>
#include <reg51.h>

/*funciones prototipo externas */

extern void EscribirCadenaSoft_buffer(unsigned char *buffer,unsigned char tamano_cadena);
extern void EscribirCadenaSoft(unsigned char tamano_cadena,unsigned char tipo);


extern void send_portERR(unsigned char cod_err);




extern void PantallaLCD(unsigned char cod_msg);
extern void PantallaLCD_LINEA_2(unsigned char cod_msg, unsigned char *buffer);

//extern void Cmd_LPR_Salida(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);


/*funciones prototipo de clock*/

extern void analiza_tiempo(char *buffer,unsigned int Val_DctoMinutos);
extern void Block_read_Clock_Hex(unsigned char *datos_clock);
void Block_read_clock_ascii(unsigned char *datos_clock);
void ByteHex_Decimal(unsigned char *buffer,unsigned char valorhex);
void Two_ByteHex_Decimal(unsigned char *buffer,unsigned char id_h,unsigned char id_l);
extern char check_fechaOut(char *buffer);
extern char lee_clk (unsigned char dir_clk);
extern unsigned char bcd_hex (unsigned char l_data);
extern void hex_ascii(unsigned char * datos,unsigned char * fecha_asii);

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
unsigned char Valida_Sensor1_Auto();
extern unsigned char Dir_board();
extern void sel_Pulsa(void);
void sel_Sensor2(void);
char ValidaSensor(void);
unsigned char Dir_Board_Monitor();

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
extern void Clave_Seguridad_S2(void);
extern void Unique_Identifier_UID(void);
extern void Power_off(void);

/*funcion prototipo monitor*/

extern void clear_placa();
extern void Rx_Monitor();

/*funcion prototipo pto paralelo*/

void  send_port(unsigned char *buffer_port, unsigned char length_char);

/*funcion prototipo programacion*/
extern unsigned char *Addr_Horarios();

extern int    atoi (const char *s1);
/*io sensores */

sbit DataIn = P1^1;					//	dato de las entradas		
sbit sel_A = P3^5;					//Pulsador												*
sbit sel_B = P3^6;					//Entrada Sensor 2										*
sbit sel_C = P3^7;					//Entrada Sensor 1										*

sbit lock = P1^7;						//Relevo 	
sbit Rele_Atasco = P0^3;				//Rele de on/off del verificador o transporte
sbit led_err_imp = P0^2;			//Error 	

/*pines de ip tibbo*/

sbit rx_ip = P0^0;		
/*variables externas*/

extern unsigned char g_cEstadoComSoft;
extern unsigned char ValTimeOutCom;
extern unsigned char g_cContByteRx;
extern unsigned char xdata Buffer_Rta_Lintech[];

extern unsigned int T_GRACIA;																				/*tiempo de gracia del parqueo*/
extern unsigned char Timer_wait;
extern unsigned int  SIN_COBRO;
extern unsigned	char 	Tarjeta_on;
extern unsigned char cnt__ask_off;
extern  unsigned char Tipo_Vehiculo;
extern  unsigned char  Debug_Tibbo;																										/*variable q define expedicion de tarjetas 1= automatico 0= a boton configurable en eeprom*/
extern idata unsigned char placa[];

/*externo bit*/

extern bit aSk;
extern bit buffer_ready;
extern bit placa_ready;

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
#define SEQ_FRONT_CARD									0x0d
#define SEQ_ESPERA_VEHICULO_ENTRE				0x0e
#define SEQ_DETAIL_CARD									0x0f
#define SEQ_PTO_PARALELO								0x10
#define	SEQ_WAIT_PLACA									0x11
#define SEQ_UID													0X12
#define SEQ_LPR													0X13	
#define SEQ_TIPO_TARJETAS								0X14
#define SEQ_CAPTURE_CARD_LOOP						0X15
#define SEQ_POWER_OFF										0X16
#define SEQ_POWER_ON										0X17

/*----------------------------------------------------------------------------
Definiciones de sequencias de tareas del verificador y expedidor
------------------------------------------------------------------------------*/
#define TAREA_PRESENCIA_VEHICULAR							0X00	
#define TAREA_TIPO_TARJETA										0X01
#define TAREA_LECTURA_TARJETA_SECTOR_BLOQUE 	0x02
#define TAREA_WRITE_TARJETA_SECTOR_BLOQUE			0X03
#define TAREA_OPEN_BARRERA										0X04
#define TAREA_WRITE_PLACA_CARD								0x05
#define TAREA_TIPO_MENSUAL										0x06
#define TAREA_UID															0X07
#define TAREA_PRESENCIA_ROTACION							0X08
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
#define Rtype				        0x0A
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
#define MF70									'1'
#define OPERATE_FAIL					0X6F

/*----------------------------------------------------------------------------
definicion de recepcion serial 
------------------------------------------------------------------------------*/

#define  ESPERA_RX 					0  					//espera el primer cmd de recepcion del verificado 

/*----------------------------------------------------------------------------
tiempo de delay entre funciones
------------------------------------------------------------------------------*/

#define 	TIME_CARD					100		//50
#define 	TIME_WAIT					18
/*----------------------------------------------------------------------------
definicion de datos de trama lintech
------------------------------------------------------------------------------*/

//#define 	ETX								03
#define 	STX_LINTECH				0xf2

/*----------------------------------------------------------------------------
msj de lcd tarjeta y lcd serie
------------------------------------------------------------------------------*/
/*Los cmd PRMR son enviados por el primario*/
#define PRMR_ERROR_LOOP					0XE0
#define ERROR_LOOP							170
#define PRMR_ERROR_COD_PARK			0XE5
#define ERROR_COD_PARK					171
#define PRMR_TARJETA_INVALIDA		0XE1
#define PULSE_BOTON							172
#define PRMR_TARJETA_SIN_FORMATO  0xDF
#define TARJETA_SIN_FORMATO  			173

#define PRMR_NO_CARD_MENSUAL					0XFC
#define NO_CARD_MENSUAL					176
#define TARJETA_INVALIDA				177

#define PRMR_NO_ROTACION							0XFD

#define PRMR_SIN_SALIDA							0XE9
#define SIN_SALIDA							178
#define REGISTRA_INGRESO				179

#define PRMR_TARJETA_VENCIDA					0XEC
#define TARJETA_VENCIDA					180
#define PRMR_ERROR_TIPO_VEHICULO			0XF8
#define PRMR_MENSUAL_FUERA_HORARIO		0Xb5
#define MENSUAL_FUERA_HORARIO		181
#define ERROR_MF1								0XE2
#define HORARIO_NO_PROG					182



#define BIENVENIDO							0XFE
#define NO_CARD									0xFA	

#define RETIRE_TARJETA					0XA1

#define LOW_CARD								0x01

#define AUDIO_ENTRADA			0XA0
#define AUDIO_CAJA				0XA1
#define AUDIO_GRACIAS			0XA2
/*----------------------------------------------------------------------------
definiciones para, el debuger. saber si la trama es enviada, o la trama es de respuesta
------------------------------------------------------------------------------*/

#define 	ENVIADOS					0X0
#define		RESPUESTA					0X01
/*
definicion  de daos del reloj
					*/

#define RDIA						0x87
#define RMES						0x89
#define RANO						0x8D
#define RDIA_SEMANA			0x8B
#define RHORA						0x85
#define RMIN						0x83
#define Sabado					7
#define Domingo					1
/*----------------------------------------------------------------------------
definiciones de la tarjeta MF tipo de cliente esto esta en la posicion (0) de la memoria MF
(0) si el dato es cero esta inactiva
(1) activa o ROTACION
(2) mensualidad
------------------------------------------------------------------------------*/
enum Tipos_MF_TIPO_TARJETA{
	INACTIVA,					
	ROTACION, 					
	MENSUALIDAD,
	PREPAGO,
	CORTESIA,
	LOCATARIO,
	TARJETA_PERDIDA = 0X10,
	INHABILITADA = 0X11
};
/*----------------------------------------------------------------------------
posicion de  MF  bloque 1 sector 1
(0) tipo de tarjeta 
(01) el id del cliente
(03)codigo del parqueadero	
------------------------------------------------------------------------------*/
#define 	MF_TIPO_TARJETA		0X00
#define 	MF_COD_PARK 			0x01			//MF_ID_CLIENTE
#define		MF_ID_CLIENTE			0x03			//MF_COD_PARK

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

#define		MF_MENSUAL_ANO			0X05
#define		MF_MENSUAL_MES			0X06
#define		MF_MENSUAL_DIA			0X07

#define		MF_UID_0			0X04
#define		MF_UID_1			0X05
#define		MF_UID_2			0X06
#define		MF_UID_3			0X07

#define		MF_EXPIRA_ANO			0X08
#define		MF_EXPIRA_MES			0X09
#define		MF_EXPIRA_DIA			0X0A

#define 	HABILITA_ADDR 		15
#define 	Segundo_Tiempo		16

enum Hora_Minutos_addr{
	Hora_High_addr_Desde = 7, Hora_Low_addr_Desde = 8, Minutos_High_addr_Desde = 9, Minutos_Low_addr_Desde = 10,
	Hora_High_addr_Hasta = 11, Hora_Low_addr_Hasta = 12, Minutos_High_addr_Hasta = 13, Minutos_Low_addr_Hasta = 14
};

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
 Horario,
 Pico_Placa,
 Type_Vehiculo,
 Uid_0,
 Uid_1,
 Uid_2,
 Uid_3,
 Expira_ano,
 Expira_mes,
 Expira_dia
 
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
	Sector_2,
	Bloque_0 = 0,
	Bloque_1 = 1,
	Bloque_2 = 2
};
/*comandos pto paralelo*/
enum CMD_Trama_Pto_Paralelo{
	STX=02,
	CMD_PTO_PARALELO_EXPEDIDOR='a',
	ETX= 03,
	NULL=0,
	CMD_MONITOR_EXPEDIDOR='E',
	CMD_PTO_PARALELO_EXPEDIDOR_MENSUAL='M'
};
enum EE_AntiPassBack{
	APB_INHABILITADO_SOFT,
	APB_HABILITADO_SOFT
};	
enum Tipos_Vehiculos{
		AUTOMOVIL,					
		MOTO,
		BICICLETA
};	

/*DATOS DE CONFIGURACION EEPROM*/
#define EE_ID_CLIENTE						0x0000
#define EE_ID_PARK		  				0x0002
#define EE_DEBUG								0x0008
#define EE_USE_LPR							0x000A
#define EE_CARD_AUTOMATIC_BOTON	0x000f
#define EE_HABILITA_APB					0x0010
#define EE_PLACA								0X0011
#define EE_VALIDA_TIPO_VEHICULO_MENSUAL 0X0014
#define	EE_HABILITA_APB_MENSUAL 0X0015

/*----------------------------------------------------------------------------
Definicion de varaibles globales del objeto
------------------------------------------------------------------------------*/

static unsigned char Estado=INICIA_LINTECH;
bit MenSual = False;
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
unsigned char Captura_Expulsa()
{
	unsigned char Estado_expedidor;
	
	
	if(MenSual !=  True)
	{
		Estado_expedidor=SEQ_CAPTURE_CARD_LOOP; //SEQ_CAPTURE_CARD;																				
	}
	else
	{
	
	Estado_expedidor = SEQ_EXPULSAR_CARD;//SEQ_CARD_INSERCION_OFF;	
	}	


return 	Estado_expedidor;
}
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
			
			if ((ValTimeOutCom==1)||(buffer_ready==1) )
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
				{	
					Debug_txt_Tibbo((unsigned char *) "ATASCADO RESET\r\n\r\n");																																			/*no contesta debe reset el transporte*/
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
unsigned char   rta_cmd_transporte(unsigned char *secuencia_Expedidor )
{
	unsigned char temp;
	unsigned char EstadoComSeqMF;
	
	
			
if((temp=Trama_Validacion_P_N())!=RSPT_TRP_OK	)
		{
			if(temp==ESPR_RSPT_TRP_TRAMA)																													/*no he recibido respuesta espero*/
			{
			EstadoComSeqMF= *(secuencia_Expedidor + EstadoActual);										//																	/*SEQ_RTA_CARD_POSno ha respondido*/
			}	
			else if (temp==ERROR_TRP_TRAMA)
			{
			Debug_txt_Tibbo((unsigned char *) "RTA_CMD_ERROR\r\n");											/* trama no valida respuesta incorrecta falla en la escritura */
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);														/*imprimo la trama recibida*/	
			EstadoComSeqMF=*(secuencia_Expedidor + EstadoPasado);	
			//EstadoComSeqMF=SEQ_INICIO	;																														/// (3) Trama invalida cmd (N)reenvio cmd*/	
			}			
			else
			{
			/*Dispensador No Responde PTO SERIE ...*/
		
				if(temp=error_rx_pto()==ESPERA_MAS_TIEMPO)
				{
					EstadoComSeqMF=*(secuencia_Expedidor + EstadoActual);	
				}																										 																	/*SEQ_RTA_CARD_POS;*/
				else 																																									//(temp=error_rx_pto()==REENVIA_TRAMA)
				{
					EstadoComSeqMF=*(secuencia_Expedidor + EstadoPasado);																											/*SEQ_INICIO*/
				}																									
			}				
		}
		else
	  {
			
			DebugBufferMF(Buffer_Rta_Lintech,g_cContByteRx,RESPUESTA);
			EstadoComSeqMF=*(secuencia_Expedidor + EstadoFuturo);	
		}
	return EstadoComSeqMF;
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
unsigned char Analiza_Presencia_Mensual()
{
	unsigned char Estado_expedidor;
	if((ValidaSensoresPaso())!=False)	 																							// valido los sensor de piso
		{
			MenSual = True;
			Estado_expedidor = SEQ_TIPO_CARD;									//SEQ_UID
		}
	else
		{	
					/*no hay vehiculo en los sensores se hace el loop otra vez */
				send_portERR(PRMR_ERROR_LOOP);
				PantallaLCD(ERROR_LOOP);
				Estado_expedidor = SEQ_EXPULSAR_CARD;
							
		}
	return Estado_expedidor;
}
unsigned char Analiza_Presencia_rotacion()
{
	unsigned char Estado_expedidor;
	if((ValidaSensoresPaso())!=False)	 																							// valido los sensor de piso
		{
			
			Estado_expedidor = SEQ_MOVER_CARD_RF;									//SEQ_UID
		}
	else
		{	
					/*no hay vehiculo en los sensores se hace el loop otra vez */
				send_portERR(PRMR_ERROR_LOOP);
				PantallaLCD(ERROR_LOOP);
				Estado_expedidor = SEQ_INICIO;	//SEQ_EXPULSAR_CARD;
							
		}
	return Estado_expedidor;
}
/*------------------------------------------------------------------------------
Funcion numero unico de identificacion
------------------------------------------------------------------------------*/
unsigned char Analiza_Uid_Card(unsigned char *Atributos_Expedidor )
{
	unsigned char Estado_expedidor;
	unsigned char temp;
	unsigned char buffer_UID[17];
		if (Buffer_Rta_Lintech[Pos_Length] >= 0x0f)
			{
				
					for (temp=0; temp<16; ++temp)
					{
						buffer_UID [temp]=Buffer_Rta_Lintech[Pos_IniDatMF+temp];														/*almaceno la informacion de MF en un arreglo*/
					 
					}
					
					Debug_txt_Tibbo((unsigned char *) "buffer_UID\r\n");
					DebugBufferMF(buffer_UID,16,RESPUESTA);
					
	
					Debug_txt_Tibbo((unsigned char *) "UID_CARD :");	
					Debug_chr_Tibbo((buffer_UID [ MF_UID_0]));	
					Debug_chr_Tibbo((buffer_UID [ MF_UID_1]));	
					Debug_chr_Tibbo((buffer_UID [ MF_UID_2]));	
					Debug_chr_Tibbo((buffer_UID [ MF_UID_3]));
					Debug_txt_Tibbo((unsigned char *) "\r\n");
					
				*(Atributos_Expedidor + Uid_0) = buffer_UID [MF_UID_0];		
				*(Atributos_Expedidor + Uid_1) = buffer_UID [MF_UID_1];			
				*(Atributos_Expedidor + Uid_2) = buffer_UID [MF_UID_2];			
				*(Atributos_Expedidor + Uid_3) = buffer_UID [MF_UID_3];	
				Estado_expedidor=SEQ_LOAD_PASSWORD;																												//SEQ_TIPO_CARD;
			}		
			else
			{
				Estado_expedidor=SEQ_POWER_OFF;
			}	
			return Estado_expedidor;
}
/*------------------------------------------------------------------------------
Se analiza si expulsa la tarjeta por boton o automatica
------------------------------------------------------------------------------*/
unsigned char Ingreso_Vehiculo(void)
	{
		unsigned char CardAutomatic;	
		static unsigned char pulseboton=0;
	
	if((ValidaSensoresPaso())!=False)	 																							// valido los sensor de piso
		{
			CardAutomatic=rd_eeprom(0xa8,EE_CARD_AUTOMATIC_BOTON);	
			
			if (CardAutomatic==True)																										//se pregunta si expide la tarjeta automatica o por presionar el boton
				{
					Debug_txt_Tibbo((unsigned char *) "Tarjeta: Automatico\r\n");						//expide tarjetas automatico con presencia
																																									// muevo la tarjeta hasta el lector de rf
					Estado=SEQ_MOVER_CARD_RF;	  																						// valido el cmd enviado al verificador
				 }
			else
				{			
						if(pulseboton==False)
						{ PantallaLCD(PULSE_BOTON);	
						}
						else
						 {
							 pulseboton++;
							
						 
						 }
							sel_Pulsa();																												//se valida el pulsador en hardware
						 	if (DataIn!=True)  		  
								{
									Debug_txt_Tibbo((unsigned char *) "Pulsador Activo\r\n");				//el pulsador fue presionado
									pulseboton=0;																																//muevo tarjeta hasta el lector de RF
									Estado=SEQ_MOVER_CARD_RF; 																			//valido el cmd enviado al verificador
								}
							else
									{
										Estado=SEQ_INICIO;	
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
 
/*------------------------------------------------------------------------------
Se pregunta por el estado del expedidor si hay tarjetas y presencia para expulsar tarjeta
o si hay tarjeta en la boca o rf para entrar en mensuales
------------------------------------------------------------------------------*/

unsigned char	Responde_Estado_Sensores_Transporte()
{
	unsigned char Estado_expedidor;
		
			Debug_txt_Tibbo((unsigned char *) "TAREA_PRESENCIA_VEHICULAR\r\n");																	// trama valida Habilitado 
		
			if (Buffer_Rta_Lintech[Pos_St0]==NO_CARDS_IN_MCNSM)																	 								// CANAL LIBRE	  no tiene tarjetas en el mecanismo
			{
					
				if	((Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS)||(Buffer_Rta_Lintech[Pos_St1]==FULL_CARD	)) 	//  se detecta la tarjeta en la boca TARJETA EN BEZZEL
				{
					if (Buffer_Rta_Lintech[Pos_St1]==LOW_NIVEL_CARDS	) 																						// nivel de tarjetas
						{
							Debug_txt_Tibbo((unsigned char *) "Nivel bajo de tarjetas\r\n");														// nivel bajo de tarjetas 
							send_portERR(LOW_CARD);																																			//envio msj al primario
						//	PantallaLCD(LOW_CARD);																																						//envio msj por la raspberry nivel bajo de tarjetas
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
							
							send_portERR(NO_CARD);	
																																		// se envia msj al uC principal, visualiza en el lcd que no hay tarjetas
						//	PantallaLCD(NO_CARD);		
							Estado_expedidor=SEQ_INICIO;																																//inicio el loop
						 }
			}
			else if (Buffer_Rta_Lintech[Pos_St0]==CARD_IN_MOUTH)	 																						  //  se detecta la tarjeta en la boca TARJETA EN BEZZEL
				{
							/*hay una tarjeta en la boca del verificador */
							Debug_txt_Tibbo((unsigned char *) "Tarjeta en la boca\r\n");																//se envia msj al debuger q hay tarjeta en la boca
																																																				  //se habilita recepcion de tarjetas por boca
							Estado_expedidor=SEQ_CARD_INSERCION_ON;																										  //se trabaja mensual				
				}
			else if (Buffer_Rta_Lintech[Pos_St0]==CARD_OK_READ_RF	)	 
						
				{
							Debug_txt_Tibbo((unsigned char *) "Tarjeta en RF\r\n");
							Estado_expedidor=SEQ_CARD_INSERCION_ON;
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
			
	if (Buffer_Rta_Lintech[Card_type_H]==MF50_HIGH)																							/* pregunto si la tarjeta en el transporte es MF 50 */
		{
		if ((Buffer_Rta_Lintech[Card_type_L] == MF50_LOW) || (Buffer_Rta_Lintech[Card_type_L] == MF70))	
			{
				Debug_txt_Tibbo((unsigned char *) "Tarjeta valida MF50\r\n");													/* trama valida son MF50*/
				if(MenSual ==  True)
				{
				Estado_expedidor = SEQ_UID;																															//SEQ_LOAD_PASSWORD;												
				}
				else	
				{
					Estado_expedidor = SEQ_LOAD_PASSWORD;
				}					
			}
			else
				{
					 if (Buffer_Rta_Lintech[Pos_St0]==CARD_OK_READ_RF)	 																						  //  se detecta la tarjeta en la boca TARJETA EN BEZZEL
					{
					
					Debug_txt_Tibbo((unsigned char *) "Tarjeta invalida_1 no es MF50\r\n");						/* trama no valida */
								
					send_portERR(PRMR_TARJETA_INVALIDA);																															/*envio msj principal tarjeta invalidad*/
					PantallaLCD(TARJETA_INVALIDA);
							
						Estado_expedidor = Captura_Expulsa();	
					
					}	
					else	
					{
						MenSual = False;
						Estado_expedidor = SEQ_INICIO;		
					}						
				}
		}
		else 
			{
				 if (Buffer_Rta_Lintech[Pos_St0]==CARD_OK_READ_RF)	 																						  //  se detecta la tarjeta en la boca TARJETA EN BEZZEL
					{
						/*patina el transporte */
					
				Debug_txt_Tibbo((unsigned char *) "Tarjeta invalida no es MF50\r\n");						/* trama no valida */
								
				send_portERR(PRMR_TARJETA_INVALIDA);	   																													//la tarjeta no es valida 
				PantallaLCD(TARJETA_INVALIDA);																									/*envio el msj por la pantalla lcd o la raspberry*/
									
				Estado_expedidor = Captura_Expulsa();	
										
						
					}
					else	
					{
						MenSual = False;
						Estado_expedidor = SEQ_INICIO;		
					}											
			}
							
		return Estado_expedidor;																																																	
	}	
/*------------------------------------------------------------------------------
	Funcion que lee los datos en el Sector 1 Bloque 1
	se lee
	ID_CLIENTE
	COD_PARK
	TIPO DE TARJETA
	EXPIRA_ANO
	EXPIRA_MES
	EXPIRA_DIA
------------------------------------------------------------------------------*/

unsigned char  Responde_Lectura_Tarjeta_Sector1_Bloque1 (unsigned char *Atributos_Expedidor)
{
	unsigned char temp;
	unsigned char Estado_expedidor;
	unsigned char buffer_S1_B1[17];
	unsigned char ID_CLIENTE;
	unsigned char COD_PARK;		
	static unsigned char falla=0;
			Debug_txt_Tibbo((unsigned char *) "TAREA_LECTURA_TARJETA_SECTOR1_BLOQUE1\r\n");		
																	
																
			
			if (Buffer_Rta_Lintech[Pos_Length] >=0x18)
			{
				
					for (temp=0; temp<16; ++temp)
					{
						buffer_S1_B1 [temp]=Buffer_Rta_Lintech[Pos_IniDatMF+temp];														/*almaceno la informacion de MF en un arreglo*/
					 
					}
					
					
					Debug_txt_Tibbo((unsigned char *) "Buffer_s1_b1\r\n");
					DebugBufferMF(buffer_S1_B1,16,RESPUESTA);
					ID_CLIENTE=rd_eeprom(0xa8,EE_ID_CLIENTE);	
					COD_PARK=rd_eeprom(0xa8,EE_ID_PARK);
					/*Compara  MF_ID_CLIENTE y MF_COD_PARK con el codigo del parqueadero ID_CLIENTE  COD_PARK*/
					
					if (((buffer_S1_B1 [ MF_ID_CLIENTE])==ID_CLIENTE) && ((buffer_S1_B1 [ MF_COD_PARK] ) == COD_PARK)||((ID_CLIENTE==0)&&(COD_PARK==0)))		
					{
							
						Debug_txt_Tibbo((unsigned char *) "CARD ID_CLIENTE: ");								
						Debug_HexDec((buffer_S1_B1 [ MF_ID_CLIENTE]));
						Debug_txt_Tibbo((unsigned char *) "\r\n");
						
						Debug_txt_Tibbo((unsigned char *) "ID_CLIENTE: ");								
						Debug_HexDec(ID_CLIENTE);
						Debug_txt_Tibbo((unsigned char *) "\r\n");
						
						Debug_txt_Tibbo((unsigned char *) "CARD COD_PARK:");										
						Debug_HexDec(buffer_S1_B1 [ MF_COD_PARK]);
						Debug_txt_Tibbo((unsigned char *) "\r\n"); 
						
						Debug_txt_Tibbo((unsigned char *) "COD_PARK:");										
						Debug_HexDec(buffer_S1_B1 [ MF_COD_PARK]);
						Debug_txt_Tibbo((unsigned char *) "\r\n"); 

						Debug_txt_Tibbo((unsigned char *) "TIPO DE TARJETA: ");
						Debug_chr_Tibbo(buffer_S1_B1 [MF_TIPO_TARJETA]);
						Debug_txt_Tibbo((unsigned char *) "\r\n");
						
						*(Atributos_Expedidor + Tipo_Tarjeta) = buffer_S1_B1 [MF_TIPO_TARJETA];
						/*fecha de vencimiento de mensual o prepago*/
						*(Atributos_Expedidor + Expira_ano) = buffer_S1_B1 [MF_EXPIRA_ANO];
						*(Atributos_Expedidor + Expira_mes) = buffer_S1_B1 [MF_EXPIRA_MES];
						*(Atributos_Expedidor + Expira_dia) = buffer_S1_B1 [MF_EXPIRA_DIA];
						
						*(Atributos_Expedidor + Sector) = Sector_1;
						*(Atributos_Expedidor + Bloque) = Bloque_2;
						falla=0;
						Estado_expedidor = SEQ_READ_SECTOR_BLOQUE;
				
					}
					else
					{
						falla=0;
						Debug_txt_Tibbo((unsigned char *) "ERROR COD PARK\r\n");
						send_portERR(PRMR_ERROR_COD_PARK);
						PantallaLCD(ERROR_COD_PARK);																												//envio el msj por la pantalla lcd o la raspberry
						Estado_expedidor = Captura_Expulsa();																									//	 codigo de parqueo erro expulso la tarjeta 		
					}
			}
			else
			{
					
				if ((Buffer_Rta_Lintech[Rtype]) == OPERATE_FAIL)
				{
					falla++;
				}
				 if (falla <= 2)
					
				{
					Debug_txt_Tibbo((unsigned char *) "ERROR OPERATE FAIL\r\n");
					Estado_expedidor = SEQ_POWER_ON;
															
				}
				else
				{
					falla=0;
					Debug_txt_Tibbo((unsigned char *) "TARJETA SIN FORMATO\r\n");
					send_portERR(PRMR_TARJETA_SIN_FORMATO);
					PantallaLCD(TARJETA_SIN_FORMATO);																												/*envio el msj por la pantalla lcd o la raspberry*/
				 Estado_expedidor =Captura_Expulsa();		
				}
			}
			
	return Estado_expedidor;
}	
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/

unsigned char Responde_Lectura_Tarjeta_Sector1_Bloque2 (unsigned char *Atributos_Expedidor)
{
	unsigned char temp;
	unsigned char Estado_expedidor;
	unsigned char buffer_S1_B2[17];	

			
			Debug_txt_Tibbo((unsigned char *) "TAREA_LECTURA_TARJETA_SECTOR1_BLOQUE2\r\n");		
			
												
			
			if (Buffer_Rta_Lintech[Pos_Length] >=0x18)
			{
				
					for (temp=0; temp<16; ++temp)
					{
						buffer_S1_B2 [temp] =Buffer_Rta_Lintech[Pos_IniDatMF+temp];														/*almaceno la informacion de MF en un arreglo*/
					}
					Debug_txt_Tibbo((unsigned char *) "Buffer_s1_b2\r\n");
					DebugBufferMF(buffer_S1_B2,16,RESPUESTA);
					
								
					
					Debug_txt_Tibbo((unsigned char *) "HORARIO:");
					Debug_chr_Tibbo((buffer_S1_B2 [MF_TIPO_VEHICULO] & 0XF0) >> 4);
					*(Atributos_Expedidor + Horario)= ((buffer_S1_B2 [MF_TIPO_VEHICULO] & 0XF0) >> 4);
					Debug_txt_Tibbo((unsigned char *) "\r\n");
					
					Debug_txt_Tibbo((unsigned char *) "PICO Y PLACA:");
					
					*(Atributos_Expedidor + Pico_Placa)= ((buffer_S1_B2 [MF_IN_PAGO] & 0XF0) >> 4);
					Debug_chr_Tibbo(*(Atributos_Expedidor + Pico_Placa));
					Debug_txt_Tibbo((unsigned char *) "\r\n");
					
			
					Debug_txt_Tibbo((unsigned char *) "ANTIPASSBACK:");
					Debug_chr_Tibbo(buffer_S1_B2 [MF_APB] );
					Debug_txt_Tibbo((unsigned char *) "\r\n");
				
					/*antipassback 00 inicializado, 01 IN, 02 OUT, 03 NO USA*/
					
					*(Atributos_Expedidor + Apb)=		buffer_S1_B2 [MF_APB] ;
					
					
						*(Atributos_Expedidor + Type_Vehiculo	) = buffer_S1_B2 [MF_TIPO_VEHICULO]& 0x0f;
						Estado_expedidor =SEQ_TIPO_TARJETAS;		// Valida_Tipo_Tarjeta(Atributos_Expedidor,Buffer_Write_MF);
			
	
			}
			else
			{
				Estado_expedidor = Captura_Expulsa(); //momentario
			}
		return Estado_expedidor;	
}

unsigned char Responde_Lectura_Tarjeta_Sector1_Bloque0 (unsigned char *Nombre_Mensual)
{
	unsigned char temp;
	unsigned char Estado_expedidor;
		

			
			Debug_txt_Tibbo((unsigned char *) "TAREA_LECTURA_TARJETA_SECTOR1_BLOQUE0\r\n");		
			
												
			
			if (Buffer_Rta_Lintech[Pos_Length] >=0x18)
			{
				
					for (temp=0; temp<16; ++temp)
					{
						*(Nombre_Mensual + temp ) =Buffer_Rta_Lintech[Pos_IniDatMF+temp];														/*almaceno la informacion de MF en un arreglo*/
					}
					*(Nombre_Mensual + temp )=NULL;
					Debug_txt_Tibbo((unsigned char *) "Nombre Mensual:");
					Debug_txt_Tibbo(Nombre_Mensual );
					Debug_txt_Tibbo((unsigned char *) "\r\n");			
					Estado_expedidor = SEQ_LPR;						//SEQ_TIPO_TARJETAS;																								// Valida_Tipo_Tarjeta(Atributos_Expedidor,Buffer_Write_MF);
			
	
			}
			else
			{
				Estado_expedidor = Captura_Expulsa(); //momentario
			}
		return Estado_expedidor;	
}
unsigned char Responde_Write_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF)
{
	unsigned char Estado_expedidor;
	Debug_txt_Tibbo((unsigned char *) "TAREA_WRITE_TARJETA_SECTOR1_BLOQUE2\r\n");	
	Debug_txt_Tibbo((unsigned char *) "Write_s1_b2\r\n");
	DebugBufferMF(Buffer_Write_MF,16,RESPUESTA);
	if(MenSual !=  True)
	{
	*(Atributos_Expedidor + Sector) = Sector_1;
	*(Atributos_Expedidor + Bloque) = Bloque_0;
	Armar_Trama_Tarjeta_Sector1_Bloque0(Buffer_Write_MF);
		Estado_expedidor = SEQ_WRITE_SECTOR_BLOQUE;
	}
	else
	{
		*(Atributos_Expedidor + Sector) = Sector_1;
		*(Atributos_Expedidor + Bloque) = Bloque_0;
		Estado_expedidor = SEQ_READ_SECTOR_BLOQUE;
		
	}
	return Estado_expedidor;
}
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
unsigned char Responde_Write_Tarjeta_Sector1_Bloque0(unsigned char *Buffer_Write_MF)
{

	Debug_txt_Tibbo((unsigned char *) "TAREA_WRITE_TARJETA_SECTOR1_BLOQUE0\r\n");	
	Debug_txt_Tibbo((unsigned char *) "Write_s1_b0\r\n");
	DebugBufferMF(Buffer_Write_MF,16,RESPUESTA);
	
	return SEQ_LPR;
}
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
unsigned char Pregunta_Lpr(unsigned char *Atributos_Expedidor)
{
	unsigned char Estado_expedidor;
		if(rd_eeprom(0xa8,EE_USE_LPR)== True)
			 {
				 /*monitor trama*/
				 Debug_txt_Tibbo(Armar_Trama_Monitor(Atributos_Expedidor));
					if (rd_eeprom(0xa8,EE_DEBUG) == False)
					 {
						 Debug_Tibbo=False;
					 }
			 }
	
		if (rd_eeprom(0xa8,EE_PLACA)!= False)
		{
			/*espero placa*/
			Estado_expedidor=SEQ_WAIT_PLACA;
			ValTimeOutCom=TIME_WAIT	;
			Timer_wait=False;
		}
		else 
		{
		Estado_expedidor=SEQ_FRONT_CARD;
		}
	return Estado_expedidor;
}
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/
unsigned char Responde_Write_Tarjeta_Sector2_Bloque0(unsigned char *Buffer_Write_MF)
{
	
	Debug_txt_Tibbo((unsigned char *) "PLACA:");	
	Debug_txt_Tibbo((unsigned char *) placa);
	Debug_txt_Tibbo((unsigned char *) "\r\n");
	Debug_txt_Tibbo((unsigned char *) "Write_s2_b0\r\n");
	DebugBufferMF(Buffer_Write_MF,16,ENVIADOS);
	return SEQ_FRONT_CARD;
}
unsigned char  Respuesta_Segunda_clave(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF)
{
	Debug_txt_Tibbo((unsigned char *) "TAREA_SEGUNDA CLAVE\r\n");	
	DebugBufferMF(Buffer_Write_MF,16,RESPUESTA);
	*(Atributos_Expedidor + Sector) = Sector_2;
	*(Atributos_Expedidor + Bloque) = Bloque_0;
	Armar_Trama_Placa(Buffer_Write_MF);
	return SEQ_WRITE_SECTOR_BLOQUE;
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
//void  Armar_Trama_Tarjeta_Sector1_Bloque1(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF)
//{
//	*(Buffer_Write_MF +0) = *(Atributos_Expedidor + Tipo_Tarjeta);
//	*(Buffer_Write_MF +0) = *(Atributos_Expedidor + Tipo_Tarjeta);
//}	

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

void  Armar_Trama_Tarjeta_Sector1_Bloque2(unsigned char *Atributos_Expedidor,unsigned char *Buffer_Write_MF)
{
	
	/*fecha de ingreso se lee año,mes,dia,hora y minutos*/
	
	Block_read_Clock_Hex(Buffer_Write_MF);
	
	/*descuentos los borro*/
	
	*(Buffer_Write_MF + 5)=0;
	*(Buffer_Write_MF + 6)=0;
	*(Buffer_Write_MF + 7)=0;
	
	if (*(Atributos_Expedidor + Tipo_Tarjeta) == ROTACION)
	{
		*(Buffer_Write_MF + 8)  = Tipo_Vehiculo;
		*(Buffer_Write_MF + 9)  = (Dir_board())- 0x30;
		*(Buffer_Write_MF +10) = APB_INT;
	}
	
	/*mensual*/
	else
	{
		
		*(Buffer_Write_MF + 8)  = (*(Atributos_Expedidor + Horario) << 4) | (*(Atributos_Expedidor + Type_Vehiculo));
		*(Buffer_Write_MF + 9)  =  (*(Atributos_Expedidor + Pico_Placa) << 4)| (Dir_board())- 0x30;
		
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
unsigned char *Armar_Trama_Pto_Paralelo_Expedidor(unsigned char *h)
{
	static unsigned char buffer[28];
	unsigned char ticket[11];
	unsigned char j;
	/*la trama esta compuesta de
	STX,CMD,-,NoTICKET,-,FECHAINT,-,placa,ETX*/

	buffer[0]=STX;
	buffer[1]=CMD_PTO_PARALELO_EXPEDIDOR;
	buffer[2]=NULL;
	strcpy(ticket, Lee_No_Ticket());
	strcat(buffer , ticket);
	j=strlen(buffer);
	
	buffer[j++]= '-';
	
	Block_read_Clock_Hex(buffer+j);					//leo la fecha de entrada
	buffer[j]=buffer[j]+0x030;							/*año de entrada*/
	buffer[j+1]=buffer[j+1]+0x030;					/*mes de entrada*/
	buffer[j+2]=buffer[j+2]+0x030;					/*dia de entrada*/
	buffer[j+3]=buffer[j+3]+0x030;					/*hora de entrada*/
	buffer[j+4]=buffer[j+4]+0x030;					/*minutos de entrada*/
	buffer[j+5]= '-';
	/*placa*/
	if (rd_eeprom(0xa8,EE_PLACA)!=0)
	{		
		if (placa_ready != False)
		{
			buffer[j+6]= placa[0];
			buffer[j+7]= placa[1];
			buffer[j+8]= placa[2];
			buffer[j+9]= placa[3];
			buffer[j+10]= placa[4];
			buffer[j+11]= placa[5];
			buffer[j+12]= NULL;
		}
		else
		{
			buffer[j+6]= ' ';
			buffer[j+7]= NULL;
		}
	}
	else 
	{
		buffer[j+6]= ' ';
		buffer[j+7]=NULL;
	}
	j=strlen(buffer);
	buffer[j]= ETX;
	*h=j+1;
	
	return buffer;
}
unsigned char *Armar_Trama_Pto_Paralelo_Expedidor_Mensual(unsigned char *Atributos_Expedidor,unsigned char *j)
{
	static unsigned char buffer[28];

	/*la trama esta compuesta de
	STX,CMD,-,NoTICKET,-,FECHAINT,-,placa,ETX*/

	buffer[0]=STX;
	buffer[1]=CMD_PTO_PARALELO_EXPEDIDOR_MENSUAL;
	buffer[2]=*(Atributos_Expedidor + Uid_0);
	buffer[3]=*(Atributos_Expedidor + Uid_1);
	buffer[4]=*(Atributos_Expedidor + Uid_2);
	buffer[5]=*(Atributos_Expedidor + Uid_3);
	
	
	Block_read_Clock_Hex(buffer+6);					//leo la fecha de entrada
	buffer[6]=buffer[6]+0x030;							/*año de entrada*/
	buffer[7]=buffer[7]+0x030;					/*mes de entrada*/
	buffer[8]=buffer[8]+0x030;					/*dia de entrada*/
	buffer[9]=buffer[9]+0x030;					/*hora de entrada*/
	buffer[10]=buffer[10]+0x030;					/*minutos de entrada*/

	/*placa*/
	if (rd_eeprom(0xa8,EE_PLACA)!=0)
	{		
		
			buffer[11]= placa[0];
			buffer[12]= placa[1];
			buffer[13]= placa[2];
			buffer[14]= placa[3];
			buffer[15]= placa[4];
			buffer[16]= placa[5];

	}

	if(*(Atributos_Expedidor + Type_Vehiculo) == AUTOMOVIL )
	{
		buffer[17]= 'C';
	}
	else
	{
		buffer[17]= 'M';
	}
	
	buffer[18]= ETX;
	buffer[19]= NULL;
	*j=19;
	return buffer;
}
unsigned char *Armar_Trama_Monitor(unsigned char *Atributos_Expedidor)
{
	static unsigned char buffer[24];
	unsigned char ticket[11];
	unsigned char j;
	/*la trama esta compuesta de
	STX,address_board,CMD,Tipo_Vehiculo,NoTICKET,:,fecha int añomesdishoraminuto,:,ETX*/
	Debug_Tibbo=False;
	buffer[0]=STX;
	buffer[1]=Dir_Board_Monitor();
	buffer[2]=CMD_MONITOR_EXPEDIDOR;
	if(Tipo_Vehiculo == AUTOMOVIL)
		{
			buffer[3]= 'C';
		}
	else
		{
			buffer[3]= 'M';
		}
		
		/*ticket*/
		if(MenSual !=  True)
		{
		buffer[4]=NULL;
		strcpy(ticket, Lee_No_Ticket());
		strcat(buffer , ticket);
		}
		else
		{
			ByteHex_Decimal(buffer+4,*(Atributos_Expedidor + Uid_2));
			j=strlen(buffer);
			buffer[j]= ' ';
			buffer[j+1]= NULL;
			j=strlen(buffer);
			Two_ByteHex_Decimal(buffer+j,*(Atributos_Expedidor + Uid_1),*(Atributos_Expedidor + Uid_0));
			
		}
		j=strlen(buffer);
		buffer[j]=':';
		/*fecha de entrada*/
		
		Block_read_clock_ascii(buffer+j+1);			//leo la fecha de entrada
		j=strlen(buffer);
	
		buffer[j]= ':';
		buffer[j+1]=ETX;
		buffer[j+2]=NULL;
		Debug_Tibbo=True;
	return buffer;
}
void Armar_Trama_Placa(unsigned char *Buffer_Write_MF)
{
	unsigned char j;
	
		for(j=0; j<8;j++)
		{
		*(Buffer_Write_MF +j)=placa[j];
		}
			for (j=8; j<16; j++)	 					
		{
			*(Buffer_Write_MF +j)=0x00;
		}
}
unsigned char Load_Secuencia_Expedidor(unsigned char *Secuencia_Expedidor,unsigned const  estadoactivo,unsigned const estadoactual,unsigned const estadofuturo)
{
	*(Secuencia_Expedidor + EstadoPasado ) = estadoactivo ;
	*(Secuencia_Expedidor + EstadoActual ) = estadoactual;
	*(Secuencia_Expedidor + EstadoFuturo ) = estadofuturo;
	return estadoactual;
}
unsigned char Disparo_Lock_Entrada_Vehiculo(unsigned char *Nombre_Mensual)
{
	unsigned char Estado_expedidor;
	Debug_txt_Tibbo((unsigned char *) "TAKE CARD\r\n");
	
	PantallaLCD(RETIRE_TARJETA);
	
	if (Buffer_Rta_Lintech[Pos_St0]==NO_CARDS_IN_MCNSM)																	  // CANAL LIBRE	  no tiene tarjetas en el mecanismo
		{
			Debug_txt_Tibbo((unsigned char *) "TAREA_OPEN_BARRERA\r\n");	
			lock=ON;
			send_portERR(BIENVENIDO);
			PantallaLCD_LINEA_2(BIENVENIDO,Nombre_Mensual);
			Estado_expedidor=SEQ_PTO_PARALELO;
			
			
		}
	else if (Buffer_Rta_Lintech[Pos_St0]==CARD_IN_MOUTH)	 																		//  se detecta la tarjeta en la boca TARJETA EN BEZZEL
	{
		if(Valida_Sensor1_Auto()!= False)
		{
			Estado_expedidor=SEQ_DETAIL_CARD;
		}
		else
		{
			Estado_expedidor=SEQ_CAPTURE_CARD;
		}
	
	}
	return Estado_expedidor;
}
unsigned char Send_Pto_Paralelo(unsigned char *Atributos_Expedidor)
{
	unsigned char *Trama_Expedidor ;
	unsigned char leng_trama_pto;
	if(MenSual == True)
	{
	Trama_Expedidor=Armar_Trama_Pto_Paralelo_Expedidor_Mensual(Atributos_Expedidor,&leng_trama_pto );
	}
	else 
	{
		Trama_Expedidor=Armar_Trama_Pto_Paralelo_Expedidor(&leng_trama_pto);
		Incremente_Ticket();
	}
	
	send_port(Trama_Expedidor,leng_trama_pto);	
	Debug_txt_Tibbo((unsigned char *) "Trama_pto_paralelo\r\n");
	DebugBufferMF(Trama_Expedidor,leng_trama_pto,ENVIADOS);
	Debug_txt_Tibbo((unsigned char *) "\r\n");
	
	
	clear_placa();
	ValTimeOutCom=TIME_WAIT	;
	Timer_wait=0;
	return SEQ_ESPERA_VEHICULO_ENTRE;
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
unsigned char Entrega_Card_Captura()
{
	unsigned char Estado_expedidor;
	
		
	Debug_txt_Tibbo((unsigned char *) "ESPERA VEHICULO ENTRE\r\n");	
	sel_Sensor2();																//garantiso q la barrera se encuentre en posicion baja	
  if ((DataIn==0))				
	{  
		if (ValidaSensor()==0)
		{
			lock=OFF;
			Debug_txt_Tibbo((unsigned char *) "Vehiculo Entrando");
			Estado_expedidor=SEQ_INICIO;
		}
		
	}
	else
	{
			if(Valida_Sensor1_Auto()!= False)
			{
			/*presencia vehicular*/
				Estado_expedidor=SEQ_ESPERA_VEHICULO_ENTRE;//SEQ_INICIO;//
				
				if ((ValTimeOutCom == 1) || (ValTimeOutCom > TIME_WAIT))
				{
						if (Timer_wait >= 5)
				 {
					 lock=OFF;
					 Estado_expedidor=SEQ_INICIO;;
				 }
				else if (Timer_wait <= 4)
				 {
					ValTimeOutCom=TIME_WAIT	;
				 }
			 }
				
			}
			else
			{
			lock=OFF;
			Estado_expedidor=SEQ_INICIO;
			}
	}	
 
	return Estado_expedidor;
}
unsigned char Wait_Placa(unsigned char *secuencia_expedidor, unsigned char estadoactivo)
{
	unsigned char Estado_expedidor;
	
	
while ((ValTimeOutCom != 1) && (ValTimeOutCom <= TIME_WAIT) && (placa_ready == False))
	{
		if (rx_ip==0)																													/*pregunto si llega datos de monitor pto serie emulado*/
				{
					
					Rx_Monitor();
				}
	}
	
		if(placa_ready!=False)
			{
				Clave_Seguridad_S2();
				Estado_expedidor=Load_Secuencia_Expedidor(secuencia_expedidor,estadoactivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);		
				*(secuencia_expedidor +TareadelCmd ) = TAREA_WRITE_PLACA_CARD;
			}
			else
			{
					Estado_expedidor=SEQ_WAIT_PLACA;
			 if ((ValTimeOutCom == 1) || (ValTimeOutCom > TIME_WAIT))
			 {	
				if (Timer_wait >= 5)
				 {
					 Estado_expedidor=SEQ_FRONT_CARD;
				 }
				else if (Timer_wait <= 4)
				 {
					ValTimeOutCom=TIME_WAIT	;
				 }
			 }
			}
	return Estado_expedidor;
}
unsigned char Valida_Vehiculo_Card_Mensual(unsigned char *Atributos_Expedidor)
{
	unsigned char Estado_expedidor;
	
		Debug_txt_Tibbo((unsigned char *) "VEHICULO MENSUAL TARJETA: ");
		if(*(Atributos_Expedidor + Type_Vehiculo	) == False)
		 {
			Debug_txt_Tibbo((unsigned char *) "(0) CARRO");
		 }
		else
		 {
			Debug_txt_Tibbo((unsigned char *) "(1) MOTO");
		 }
											
		Debug_txt_Tibbo((unsigned char *) "\r\n");
	if ((rd_eeprom(0xa8,EE_VALIDA_TIPO_VEHICULO_MENSUAL)) != True) 
	{
		if 	(*(Atributos_Expedidor + Type_Vehiculo	) == Tipo_Vehiculo )					
		{
		 Debug_txt_Tibbo((unsigned char *) "COINCIDE CARD CON LOOK\r\n ");
			Estado_expedidor = True ;
		}
		
		else
		{
			send_portERR(PRMR_ERROR_TIPO_VEHICULO);	
			Debug_txt_Tibbo((unsigned char *) "TIPO DE VEHICULO NO CORRESPONDE\r\n ");
			Estado_expedidor = False;	
		}
	}	
	else 
	{
			Debug_txt_Tibbo((unsigned char *) "NO INTERESA COINCIDENCIA CARD CON LOOK\r\n ");
			Estado_expedidor = True ;
	}
		return Estado_expedidor;
}
unsigned char Tarjeta_Mensual(unsigned char *Atributos_Expedidor,unsigned  char *Buffer_Write_MF )
{
	unsigned char Estado_expedidor;
//	unsigned char fecha_asii[7];
	
	/*cheque la fecha de expiracion del mensual*/
	if (Horarios(Atributos_Expedidor) == True)
	{
	 if ( check_fechaOut(Atributos_Expedidor+Expira_ano) == True )
			{
				/*valida el vehiculo en el loop y en la card*/
				if( Valida_Vehiculo_Card_Mensual(Atributos_Expedidor) == False)
				{
				Estado_expedidor = Captura_Expulsa();		
				}
				else
				{
					Debug_txt_Tibbo((unsigned char *) "MENSUAL AL DIA\r\n");	
					*(Atributos_Expedidor + Sector) = Sector_1;
					*(Atributos_Expedidor + Bloque) = Bloque_2;
					Armar_Trama_Tarjeta_Sector1_Bloque2(Atributos_Expedidor,Buffer_Write_MF);
					Estado_expedidor=SEQ_WRITE_SECTOR_BLOQUE;
				}
			}
			else 
			{
				send_portERR(PRMR_TARJETA_VENCIDA);	
						
				PantallaLCD(TARJETA_VENCIDA);
				Debug_txt_Tibbo((unsigned char *) "MENSUAL EXPIRA\r\n");
				Estado_expedidor = SEQ_EXPULSAR_CARD;
			}
	}
	else
	{
		
		Estado_expedidor = SEQ_EXPULSAR_CARD;
	}
	return Estado_expedidor;
}
unsigned char Tarjeta_Rotacion(unsigned char *Atributos_Expedidor,unsigned  char *Buffer_Write_MF )
{
				*(Atributos_Expedidor + Sector) = Sector_1;
					*(Atributos_Expedidor + Bloque) = Bloque_2;
					Armar_Trama_Tarjeta_Sector1_Bloque2(Atributos_Expedidor,Buffer_Write_MF);
					return  SEQ_WRITE_SECTOR_BLOQUE;
	
	
}
unsigned char Valida_Tipo_Tarjeta(unsigned char *Atributos_Expedidor,unsigned  char *Buffer_Write_MF )
{
	unsigned char Estado_expedidor;
	if(*(Atributos_Expedidor + Tipo_Tarjeta) ==  MENSUALIDAD)
	{
		Debug_txt_Tibbo((unsigned char *) "TIPO DE TARJETA MENSUALIDAD\r\n ");
		if(MenSual ==  True)
		{
		//Estado_expedidor=SEQ_CAPTURE_CARD;																				

		/*APB Habilitado por software*/
		
			if(	(rd_eeprom(0xa8,EE_HABILITA_APB_MENSUAL) == APB_HABILITADO_SOFT) )
			{
				/*APB por Card*/
		 
				if((*(Atributos_Expedidor + Apb) == APB_OUT)||(*(Atributos_Expedidor + Apb) == APB_INICIADO) || (*(Atributos_Expedidor + Apb) == APB_NO))  			
				{
					Estado_expedidor = Tarjeta_Mensual(Atributos_Expedidor,Buffer_Write_MF );

				}
				else
				{
					Debug_txt_Tibbo((unsigned char *) "ERROR: INT ANTIPASSBACK MENSUAL \r\n");
					send_portERR(PRMR_SIN_SALIDA);
					//PantallaLCD(REGISTRA_INGRESO);
					Estado_expedidor = Captura_Expulsa();	
				} 
			}
		
		else
		{
			Debug_txt_Tibbo((unsigned char *) "ANTIPASSBACK INHABILITADO MENSUAL \r\n");
		
		 Estado_expedidor = Tarjeta_Mensual(Atributos_Expedidor,Buffer_Write_MF );
		}
	}
	else
	 {
		send_portERR(PRMR_NO_CARD_MENSUAL);
		PantallaLCD(NO_CARD_MENSUAL);			
		Estado_expedidor = Captura_Expulsa();	
	 } 
 }
	else if (*(Atributos_Expedidor + Tipo_Tarjeta) ==  ROTACION)
	{
		
			Debug_txt_Tibbo((unsigned char *) "TIPO DE TARJETA ROTACION\r\n ");
		if(MenSual != True)
		{
			if ( (rd_eeprom(0xa8,EE_HABILITA_APB))!= APB_INHABILITADO_SOFT)
			{
				if((*(Atributos_Expedidor + Apb) == APB_OUT)||(*(Atributos_Expedidor + Apb) == APB_INICIADO) || (*(Atributos_Expedidor + Apb) == APB_NO) )
															
				{
					Estado_expedidor = Tarjeta_Rotacion(Atributos_Expedidor,Buffer_Write_MF );
				}
				else
				{
					Debug_txt_Tibbo((unsigned char *) "ERROR: INT ANTIPASSBACK ROTACION \r\n");
					send_portERR(PRMR_SIN_SALIDA);
					PantallaLCD(SIN_SALIDA);
					Estado_expedidor = Captura_Expulsa();
				}
			}
			else
			{
				Debug_txt_Tibbo((unsigned char *) "ANTIPASSBACK INHABILITADO ROTACION \r\n");
				Estado_expedidor = Tarjeta_Rotacion(Atributos_Expedidor,Buffer_Write_MF );
			}	
		}
		else
		{
			/*tarjeta por insercion por boca */	
			send_portERR(PRMR_NO_CARD_MENSUAL);
			PantallaLCD(NO_CARD_MENSUAL);			
			Estado_expedidor = SEQ_EXPULSAR_CARD;				//Captura_Expulsa();		
		}
	}
	else
	{
		Debug_txt_Tibbo((unsigned char *) "TIPO NUEVO DE TARJETA SIN DEFINIR\r\n ");
		send_portERR(PRMR_NO_ROTACION);
		
		Estado_expedidor = Captura_Expulsa();		
	}
	
	return Estado_expedidor;
}
/*
unsigned char Festivos()
{
	
	unsigned char dia_semana,day, month, year,DiaFestivo=0;

	
	dia_semana=bcd_hex(lee_clk(RDIA_SEMANA));
	day = bcd_hex(lee_clk(RDIA));
	month = bcd_hex(lee_clk(RMES));
	year = bcd_hex(lee_clk(RANO));
	
 	if (year==20)
	{
		if (((month==1)&&(day==1))||((month==1)&&(day==6))||((month==3)&&(day==23))||((month==4)&&(day==9))||((month==4)&&(day==10))||((month==5)&&(day==1))||((month==5)&&(day==25))||((month==6)&&(day==3)))	
		{
		 	DiaFestivo = True;
  		}
		else if	(((month==7)&&(day==20))||((month==8)&&(day==7))||((month==8)&&(day==17))||((month==10)&&(day==12))||((month==11)&&(day==2))||((month==11)&&(day==16))||((month==12)&&(day==8))||((month==12)&&(day==25)))
		{
		 	DiaFestivo = True;
		}

	}
 	else if (year==21)
	{
		if (((month==1)&&(day==1))||((month==1)&&(day==11))||((month==3)&&(day==22))||((month==4)&&(day==1))||((month==4)&&(day==2))||((month==5)&&(day==1))||((month==5)&&(day==17))||((month==6)&&(day==7))||((month==6)&&(day==14)))	
		{
		 	DiaFestivo = True;
  		}
		else if	(((month==7)&&(day==5))||((month==7)&&(day==20))||((month==8)&&(day==7))||((month==8)&&(day==16))||((month==10)&&(day==18))||((month==11)&&(day==1))||((month==11)&&(day==15))||((month==12)&&(day==8))||((month==12)&&(day==25)))
		{
		 	DiaFestivo = True;
		}

	}
	else if (year==22)
	{
		if (((month==1)&&(day==1))||((month==1)&&(day==10))||((month==3)&&(day==21))||((month==4)&&(day==14))||((month==4)&&(day==15))||((month==5)&&(day==1))||((month==5)&&(day==30))||((month==6)&&(day==20))||((month==6)&&(day==27)))	
		{
		 	DiaFestivo = True;
  		}
		else if	(((month==7)&&(day==4))||((month==7)&&(day==20))||((month==8)&&(day==7))||((month==8)&&(day==15))||((month==10)&&(day==17))||((month==11)&&(day==7))||((month==11)&&(day==14))||((month==12)&&(day==8))||((month==12)&&(day==25)))
		{
		 	DiaFestivo = True;
		}

	
	
	}
	if ((dia_semana == Sabado)||(dia_semana == Domingo)||(DiaFestivo == True))
	{
		DiaFestivo = True;
	}
	else
	{
		DiaFestivo = False;
	}
	return DiaFestivo;
}
*/
/*
unsigned char Dia_Pico_Placa(unsigned char * Atributos_Expedidor)
{
	unsigned char Par_Impar;
	unsigned char dato;
	
	if( Festivos() == False)
	{
	dato=lee_clk(RDIA);

		if((dato % 2 == False) && (*(Atributos_Expedidor + Pico_Placa)	== True))
		{
		
		/*es par*/
/*
		Par_Impar = True;
		
		}
		else
		{
		/*es impar */
		/*
		Par_Impar = False;
		}
	}
	else
	{
		Par_Impar = True;
	}
	return Par_Impar;
}
*/
unsigned char Horarios(unsigned char * Atributos_Expedidor)
{
	unsigned char Estado_Horario;
	
	unsigned char Addr_horarios [11];
	unsigned char dia_semana,EE_dia_semana;
	unsigned int addr;
	if (*(Atributos_Expedidor + Horario) != False )
	{
		/*se Lee la direccion del horario*/
		strcpy (Addr_horarios,(Addr_Horarios()));
		
		addr= Addr_horarios[(*(Atributos_Expedidor + Horario)) -1] ;
		
		 /*leemos si esta habilitado*/
		
		if ((rd_eeprom(0xa8,addr + HABILITA_ADDR)) == True)
		{
			/*miramos si el dia de la semana esta habilitado*/
			dia_semana = lee_clk(RDIA_SEMANA);
			Debug_txt_Tibbo((unsigned char *) "DIA DE LA SEMANA: ");
			Debug_chr_Tibbo(dia_semana);
			Debug_txt_Tibbo((unsigned char *) "\r\n");
			
		
			EE_dia_semana = rd_eeprom(0xa8,addr + dia_semana - 1 ) -0x30;
			Debug_txt_Tibbo((unsigned char *) "DIA PROGRAMADO: ");
			Debug_chr_Tibbo(EE_dia_semana);
			Debug_txt_Tibbo((unsigned char *) "\r\n");
		
			if ( EE_dia_semana == dia_semana)
			{
				/*miramos si esta en el rango del horario*/
				Estado_Horario = Bloque_Horario(addr);
			}
			else
			{

				Debug_txt_Tibbo((unsigned char *) "HORARIO DEL DIA NO PROGRAMADO\r\n");
				Estado_Horario= False;
			}
		}
		else 
		{
			
			Estado_Horario= False;
			PantallaLCD(HORARIO_NO_PROG);
			Debug_txt_Tibbo((unsigned char *) "INHABILITADO HORARIO \r\n");
			
		}
		
	}
	else
	{
		Estado_Horario = True;
	
		Debug_txt_Tibbo((unsigned char *) "NO TIENE HORARIO PROGRAMADO\r\n");
	}
	return Estado_Horario;
}
unsigned int Hora_Maxima(unsigned int addr)
{
	unsigned char Hora_High,  Minuto_High;
	unsigned char HoraIni , MinutoIni; 
	unsigned int  Hora_Prog;
	
	 Hora_High 		= (rd_eeprom(0xa8, (addr + Hora_High_addr_Desde )) - 0x30)  << 4;
	 HoraIni 			= Hora_High | ((rd_eeprom(0xa8, (addr + Hora_Low_addr_Desde ))) - 0x30);
	 Minuto_High 	= ((rd_eeprom(0xa8, (addr + Minutos_High_addr_Desde ))) - 0x30)  << 4;
	 MinutoIni	 	=  Minuto_High | ((rd_eeprom(0xa8, (addr +  Minutos_Low_addr_Desde ))) - 0x30);
	 Debug_chr_Tibbo(HoraIni);
	 Debug_chr_Tibbo(MinutoIni);
	 Debug_txt_Tibbo((unsigned char *) "\r\n");
	
	 return Hora_Prog = (HoraIni *60) + (MinutoIni ) ;
}
unsigned char En_Horario(unsigned int HoraNow, unsigned int Hora_Prog,unsigned int addr)
{
	unsigned char Estado_Horario;	
	
	if( Hora_Prog  <=  HoraNow )				//HoraNow >=  Hora_Prog 
	{
		
		/*hasta la hora que puede ingresar el vehiculo */
		Debug_txt_Tibbo((unsigned char *) "HORA PROGRAMADA HASTA: ");
		Hora_Prog = Hora_Maxima(addr+4);

	
		if( HoraNow <= Hora_Prog)
		{
			//send_portERR(PRMR_MENSUAL_FUERA_HORARIO);
			Debug_txt_Tibbo((unsigned char *) "EN HORARIO PROGRAMADO\r\n");
			Estado_Horario = True;
		}
		else
		{
			//send_portERR(PRMR_MENSUAL_FUERA_HORARIO);
			//PantallaLCD(MENSUAL_FUERA_HORARIO);
			Debug_txt_Tibbo((unsigned char *) "DESPUES DEL HORARIO PROGRAMADO\r\n");
			Estado_Horario = False;
		}
		
	}
	else
	{
				//send_portERR(PRMR_MENSUAL_FUERA_HORARIO);
				//PantallaLCD(MENSUAL_FUERA_HORARIO);
				Debug_txt_Tibbo((unsigned char *) "ANTES DEL HORARIO PROGRAMADO\r\n");
				Estado_Horario = False;
			
	}
	return Estado_Horario;
}

unsigned Bloque_Horario(unsigned int addr)
{
	unsigned char Estado_Horario;	
	unsigned int HoraNow, Hora_Prog;
	
	/*la hora del momento de entrada del vehiculo*/
	
	Debug_txt_Tibbo((unsigned char *) "HORA AHORA: ");
	Debug_chr_Tibbo(lee_clk(RHORA));
	Debug_chr_Tibbo(lee_clk(RMIN));
	Debug_txt_Tibbo((unsigned char *) "\r\n");
	HoraNow = (lee_clk(RHORA) * 60) + (lee_clk(RMIN) );
	
	/* desde la hora en que puede ingresar vehiculo */
	
	
	Debug_txt_Tibbo((unsigned char *) "HORA PROGRAMADA DESDE: ");
	Hora_Prog = Hora_Maxima(addr);
	Estado_Horario=En_Horario(HoraNow,Hora_Prog,addr);
	if(Estado_Horario == False )
	{
		
			
			if(rd_eeprom(0xa8,addr + Segundo_Tiempo ) == True)
			{
				Debug_txt_Tibbo((unsigned char *) "HORA PROGRAMADA SEGUNDA DESDE: ");
				Hora_Prog = Hora_Maxima(addr+10);
				Estado_Horario=En_Horario(HoraNow,Hora_Prog,addr+10);
			}
			else
			{
				send_portERR(PRMR_MENSUAL_FUERA_HORARIO);
				PantallaLCD(MENSUAL_FUERA_HORARIO);
				Estado_Horario = False;
			}
	}

return Estado_Horario;
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

unsigned char SecuenciaExpedidorMF( unsigned char EstadoActivo)
{
	static unsigned char Buffer_Write_MF[17];
	static unsigned char Atributos_Expedidor[15];
	static unsigned char Secuencia_Expedidor[4];
	static unsigned char Nombre_Mensual[17];
	
	switch (EstadoActivo)
	{
//***********************************************************************************************
		
		case SEQ_INICIO:

			if ((ValTimeOutCom==True)|| (ValTimeOutCom > TIME_CARD))
			{
				lock=OFF;																																										/*rele de disparo a la barrera*/
				Rele_Atasco=OFF;																																					 /*activo el rele de reset del verificador logica negativa*/		
				Check_Status(SENSOR_NORMAL);																															/* se pregunta al transporte en q estado estan las TI*/
				EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);
				Secuencia_Expedidor [ TareadelCmd  ] = TAREA_PRESENCIA_VEHICULAR;
			}
  		break;
		case	SEQ_CMD_ACEPTADO:
		/*cmd comun para todos*/
			EstadoActivo=rta_cmd_transporte(Secuencia_Expedidor);
			break;
		case SEQ_MOVER_CARD_RF:
			Mov_Card(MovPos_RF);
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_TIPO_CARD);
			break;
		case SEQ_CARD_INSERCION_ON:
			Card_Insercion(Habilita);	
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);
			Secuencia_Expedidor[TareadelCmd ] = TAREA_TIPO_MENSUAL;
			break;
		case SEQ_TIPO_CARD:
			Aut_Card_check_Status();
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);
			Secuencia_Expedidor[TareadelCmd ] = TAREA_TIPO_TARJETA;
			break;
		case SEQ_LOAD_PASSWORD:
			LoadVerify_EEprom();
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_READ_SECTOR_BLOQUE);
			Atributos_Expedidor [ Sector ]    = Sector_1;
			Atributos_Expedidor [ Bloque ]    = Bloque_1;
			break;
		case SEQ_READ_SECTOR_BLOQUE:
			RD_MF(Atributos_Expedidor [ Sector ],Atributos_Expedidor [ Bloque ]);
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);
			Secuencia_Expedidor[TareadelCmd ] = TAREA_LECTURA_TARJETA_SECTOR_BLOQUE;
			break;
		case SEQ_WRITE_SECTOR_BLOQUE:
			WR_MF(Atributos_Expedidor [ Sector ],Atributos_Expedidor [ Bloque ],Buffer_Write_MF);
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);
			Secuencia_Expedidor[TareadelCmd ] = TAREA_WRITE_TARJETA_SECTOR_BLOQUE;
			break;
		case SEQ_CAPTURE_CARD:
			Mov_Card(MovPos_Capture);
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_INICIO);		 //SEQ_INICIOSEQ_MOVER_CARD_RF
			break;
		case SEQ_CARD_INSERCION_OFF:
			Card_Insercion(Inhabilita);
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_EXPULSAR_CARD);
			break;
		case SEQ_EXPULSAR_CARD:
			Mov_Card(MovPos_EjectFront);
			MenSual = False;
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_INICIO);		
			break;
		case SEQ_FRONT_CARD:
			Mov_Card(MovPos_Front);
		 	EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_DETAIL_CARD);		
			break;
		case SEQ_DETAIL_CARD:
			Check_Status(SENSOR_DETAIL);
			EstadoActivo=Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);		
			Secuencia_Expedidor[TareadelCmd ] = TAREA_OPEN_BARRERA;
			break;
		case SEQ_LOAD_EEPROM:
			Dwload_EEprom();
			EstadoActivo = Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_READ_SECTOR_BLOQUE);	
			break;
		case SEQ_ESPERA_VEHICULO_ENTRE:
			MenSual = False;
			EstadoActivo = Entrega_Card_Captura();
			break;
		case SEQ_PTO_PARALELO:
			EstadoActivo = Send_Pto_Paralelo(Atributos_Expedidor);
			
			break;
		case SEQ_WAIT_PLACA:
			EstadoActivo = Wait_Placa(Secuencia_Expedidor,EstadoActivo);
			break;
		case SEQ_UID:	
			Unique_Identifier_UID();
			EstadoActivo = Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);		
			Secuencia_Expedidor[TareadelCmd ] = TAREA_UID	;
			break;
		case SEQ_LPR:
			EstadoActivo = Pregunta_Lpr(Atributos_Expedidor);
			break;
		case SEQ_TIPO_TARJETAS:
			EstadoActivo = Valida_Tipo_Tarjeta(Atributos_Expedidor,Buffer_Write_MF);;
			break;
		case SEQ_CAPTURE_CARD_LOOP:
			Mov_Card(MovPos_Capture);
			EstadoActivo = Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_RESPUESTA_TRANSPORTE);		 //SEQ_INICIOSEQ_MOVER_CARD_RF TAREA_PRESENCIA_ROTACION
		  Secuencia_Expedidor[TareadelCmd ] = TAREA_PRESENCIA_ROTACION;
			break;
		case SEQ_POWER_OFF:
			Power_off();
				EstadoActivo = Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_UID);
			break;
		case SEQ_POWER_ON:
			Power_off();
				EstadoActivo = Load_Secuencia_Expedidor(Secuencia_Expedidor,EstadoActivo,SEQ_CMD_ACEPTADO,SEQ_LOAD_PASSWORD);
			break;
/*------------------------------------------------------------------------------
		Tareas especificas de cada paso
------------------------------------------------------------------------------*/			
		case SEQ_RESPUESTA_TRANSPORTE:
			ValTimeOutCom=TIME_WAIT;
			if (Secuencia_Expedidor [TareadelCmd]==TAREA_PRESENCIA_VEHICULAR)
			{
			EstadoActivo=Responde_Estado_Sensores_Transporte();	
			
			}
			else if (Secuencia_Expedidor [TareadelCmd]==TAREA_TIPO_TARJETA)
					{
						
						EstadoActivo=Responde_Tipo_Tarjeta();
					}
			else if (Secuencia_Expedidor [TareadelCmd]==TAREA_LECTURA_TARJETA_SECTOR_BLOQUE)
					{
						if ((Atributos_Expedidor [ Sector ]== Sector_1)&& (Atributos_Expedidor [ Bloque ]==Bloque_1))
						{
							EstadoActivo=Responde_Lectura_Tarjeta_Sector1_Bloque1 (Atributos_Expedidor);
						}
						else if ((Atributos_Expedidor [ Sector ]== Sector_1)&& (Atributos_Expedidor [ Bloque ]==Bloque_0))
						{
							EstadoActivo=Responde_Lectura_Tarjeta_Sector1_Bloque0 (Nombre_Mensual);
						}
						else
						{
							EstadoActivo=Responde_Lectura_Tarjeta_Sector1_Bloque2(Atributos_Expedidor);
						}
					}
			else if (Secuencia_Expedidor [TareadelCmd]==TAREA_WRITE_TARJETA_SECTOR_BLOQUE)
					{	
						if ((Atributos_Expedidor [ Sector ]== Sector_1)&& (Atributos_Expedidor [ Bloque ]==Bloque_2))
						{	
							EstadoActivo=Responde_Write_Tarjeta_Sector1_Bloque2 (Atributos_Expedidor,Buffer_Write_MF);
						}
						else if ((Atributos_Expedidor [ Sector ]== Sector_2)&& (Atributos_Expedidor [ Bloque ]==Bloque_0))
						{
							EstadoActivo=Responde_Write_Tarjeta_Sector2_Bloque0(Buffer_Write_MF);
						}
						else
						{
							EstadoActivo=Responde_Write_Tarjeta_Sector1_Bloque0 (Buffer_Write_MF);
						}
					}
			else if (Secuencia_Expedidor [TareadelCmd]==TAREA_OPEN_BARRERA)
					{		
						EstadoActivo = Disparo_Lock_Entrada_Vehiculo(Nombre_Mensual);
					}
			else if (Secuencia_Expedidor [TareadelCmd] == TAREA_WRITE_PLACA_CARD)
					{		
						EstadoActivo = Respuesta_Segunda_clave(Atributos_Expedidor,Buffer_Write_MF);
					}
			else if (Secuencia_Expedidor [TareadelCmd] == TAREA_TIPO_MENSUAL)	
					{
						EstadoActivo = Analiza_Presencia_Mensual();
					}
					
			else if (Secuencia_Expedidor [TareadelCmd] == TAREA_UID	)	
					{
						EstadoActivo = Analiza_Uid_Card(Atributos_Expedidor);
					}	


			else if (Secuencia_Expedidor [TareadelCmd] == TAREA_PRESENCIA_ROTACION	)	
					{
						EstadoActivo = Analiza_Presencia_rotacion();
					}		
			else
					{
						Debug_txt_Tibbo((unsigned char *) "TAREA_3\r\n");
					}
			break;
		default:
		EstadoActivo = SEQ_INICIO;	
		break;	
			
	}	
	return EstadoActivo;
}
