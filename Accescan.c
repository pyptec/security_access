#include <Accescan.h>
#include <reg51.h>
					/*funciones prototipo*/
extern void Block_write_clock_ascii(unsigned char *datos_clock);
extern void PantallaLCD(unsigned char cod_msg);
extern void Reloj_Pantalla_Lcd();
extern unsigned char rx_Data(void);
extern void Debug_txt_Tibbo(unsigned char * str);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_Dividir_texto();
extern void tx_aux(unsigned char caracter);
extern unsigned char Dir_board();
extern void Block_read_clock_ascii(unsigned char *datos_clock);
extern void Two_ByteHex_Decimal(unsigned char *buffer,unsigned char id_h,unsigned char id_l);
extern void ByteHex_Decimal(unsigned char *buffer,unsigned char valorhex);
extern void PantallaLCD(unsigned char cod_msg);
extern void LCD_txt (unsigned char * msg,char enable_char_add );
extern unsigned char  ValidaSensoresPaso(void);
extern void tx_aux(unsigned char caracter);
extern void PantallaLCD_LINEA_2(unsigned char cod_msg, unsigned char *buffer);
extern unsigned char hex_bcd (unsigned char byte);
extern void Trama_pto_Paralelo_P(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2,unsigned char cmd);
extern void  send_port(unsigned char *buffer_port, unsigned char length_char);
extern unsigned char validar_clk(unsigned char *datos_clock);
extern void graba_serie(unsigned char *buffer);
extern unsigned char *Lee_No_Ticket();
extern char  *strcpy  (char *s1, const char *s2);
/*------------------------------------------------------------------------------*/
			/*variables externas */
extern unsigned int Timer_tivo;
extern unsigned char Timer_wait;
extern unsigned char Tipo_Vehiculo;
extern unsigned char USE_LPR;
extern unsigned char  Debug_Tibbo;
/*------------------------------------------------------------------------------*/
	/*bit externos*/
sbit rx_ip = P0^0;				
sbit lock = P1^7;						//Relevo 
sbit Atascado = P0^3;				//Rele de on/off del verificador o transporte
sbit led_err_imp = P0^2;			//Error 	
/*------------------------------------------------------------------------------*/

#define STX											02 
#define ETX											03 
#define FUERA_DE_LINEA					0xb6
#define ON_LINE									0xAA

/*mensajes de pantalla*/
#define BIENVENIDO							0XFE
#define SIN_PAGO								0XE7
#define LECTURA_DE_TARJETAS			0xB0

/*tipo de vehiculo*/
#define AUTOMOVIL						0X00
#define MOTO								0X01

//unsigned char S1_B2[]={0x13, 0x03, 0x1D, 0x0B, 0x0E, 00, 00, 00, 00, 00, 0x01, 0x13, 0x03, 0x1D, 0x0E, 0x1D};
//unsigned char S1_B0[]={0x32, 0x31, 0x30, 0x37, 0x31, 0x35, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01};
//unsigned char S_B[]={0xE7, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01};
/*------------------------------------------------------------------------------
Rutina q valida los cmd del pto paralelo
recibe el buffer de los datos
length_trama= longitud de la trama
------------------------------------------------------------------------------*/
void Valida_Trama_Pto(unsigned char *buffer, unsigned char length_trama)
{
	 unsigned char buff[11];
	/*-------------------------------CMD H reloj para el board y la pantalla lcd------------------------------------------*/
		if((length_trama==25)&&(*buffer==STX)&&(*(buffer+2)=='H')&&*(buffer+(length_trama-1))==ETX)													/*cmd de Accescan que me envia el reloj actualizado*/
		{ 
			if(validar_clk(buffer+3)==0)
			{
			Block_write_clock_ascii(buffer+3);																																								/* se escribe el reloj de hardware*/
		
			Reloj_Pantalla_Lcd();																																															/* Escribo el reloj actual  en la pantalla lcd*/
		
			}		
		}
		/*-------------------------------CMD B6 fuera de linea -------------------------------------------------------------*/
		else if(*buffer==FUERA_DE_LINEA)																																										/*cmd de Accescan que dice q esta fuera de linea*/
		{
			if (Timer_wait>=20)																																																/* se envia el msj fuera de linea*/
			{																																																									/* con un tiempo de retardo =((1/(22118400/12)*65535)*30)*/
				PantallaLCD(FUERA_DE_LINEA);
				led_err_imp=0;																																																	/*error led on*/
				Timer_wait=0;
				lock=0;																																																					/*relevo off despues de 1 minuto*/
				Atascado=0;	
			}
		}
		/*-------------------------------CMD AA en linea ------------------------------------------------------------------*/
		else if ((*buffer==ON_LINE)	)																																												/*en linea*/
		{
			
			if (Timer_wait>=20)																																																/* se envia el msj fuera de linea*/
			{	
				Timer_wait=0;																																																		/*se inicia el timer*/
				lock=0;
				led_err_imp=1;																																																	/*relevo off despues de 1 minuto*/
				Atascado=0;	
			}	
			
			if ((Debug_Tibbo==0)&&(USE_LPR==1)&& (Timer_tivo>=600))
			{
				Timer_tivo=0;
				Debug_Tibbo=1;
				Debug_txt_Tibbo((unsigned char *) "LIVE");
				Debug_Tibbo=0;
			}
			
			
		}	


		else if ((length_trama==19)&&(*buffer==STX)&&(*(buffer+1)=='O')&&*(buffer+(length_trama-1))==ETX)										/*mensaje de bienvenidos*/
		{
 			PantallaLCD(BIENVENIDO);
 		}
		else if ((length_trama==1)&&(*buffer==0xA1))																																				/*cmd 0xA1 audio caja que es igual a no registra pago */
		{
				 PantallaLCD(SIN_PAGO);
		}
							/*-------------------------------CMD de wiegand---------------------------------------------------*/
		else if ((length_trama==6)&&(*buffer==STX)&&(*(buffer+1)=='W')&&*(buffer+(length_trama-1))==ETX)										/* cmd q comunica con monitor po wigan*/
		{
				if (USE_LPR==1)
				{
							/*-------------------------------mensaje en la pantalla---------------------------------------------------*/
							ByteHex_Decimal(buff,*(buffer+2));																																				/*convierto el primer byte_hex a decimal		*/
							buff[3]=' ';
							Two_ByteHex_Decimal(buff+4,*(buffer+3),*(buffer+4))	;																											/*convierto un byte de 16 bits a decimal*/																									
						
							PantallaLCD_LINEA_2(LECTURA_DE_TARJETAS,buff);
																																																												/*transmito el codigo de la tarjeta a la pantalla lcd*/
							/*--------------------------------------------------------------------------------------------------------*/
					
							while(!ValidaSensoresPaso());
								
							Cmd_LPR_Salida_wiegand(buff);
				}																																							
				
				else
				{
					 /*-------------------------------mensaje en la pantalla---------------------------------------------------*/
							ByteHex_Decimal(buff,*(buffer+2));																																				/*convierto el primer byte_hex a decimal		*/
							buff[3]=' ';
							Two_ByteHex_Decimal(buff+4,*(buffer+3),*(buffer+4))	;																											/*convierto un byte de 16 bits a decimal*/																									
							PantallaLCD_LINEA_2(LECTURA_DE_TARJETAS,buff);																														/*transmito el codigo de la tarjeta a la pantalla lcd*/
																																																		
					/*--------------------------------------------------------------------------------------------------------*/	
				
				}
		}
		/*numero de serie del ticket 02 , No serie 10dig,  03*/
		else if ((length_trama==12)&&(*buffer==STX)&&*(buffer+(length_trama-1))==ETX)
		{
			graba_serie(buffer+1);																																													/*graba el No de concecutivo  en eeprom*/
					
		}
}
/*------------------------------------------------------------------------------
Rutina q recibe  los cmd de Monitor por el tibbo
return el num de caracteres recibidos
y almacena la trama en un apuntador
------------------------------------------------------------------------------*/
unsigned char recibe_cmd_Monitor(unsigned char *buffer_cmd)
{
	unsigned char j, NumDatos,time_out,MaxChrRx;
	unsigned int contador;
	
		NumDatos=0;
		MaxChrRx=11;

	if (USE_LPR==1)
	{
			for (j=0; j<MaxChrRx; j++)
			{
				contador=0;
				time_out=0;
				while ((rx_ip==1)&&(time_out==0))
				{
					contador++;
					if (contador>20000)
					{
						time_out=1;
						j=MaxChrRx;
					}				
				}
				if(time_out==1)break;
					NumDatos++;
	 				*buffer_cmd=rx_Data();
						buffer_cmd++;
			}

			*buffer_cmd=0;
			

	}
	return	NumDatos;
}	
/*------------------------------------------------------------------------------
Rutina q valida los cmd de Monitor
------------------------------------------------------------------------------*/
/*
void Valida_Trama_Monitor(unsigned char *buffer, unsigned char length_trama)
{		
	length_trama=1;
		if	((*(buffer+2)==ETX)&&(*(buffer+1)=='P')) 																																						/* APERTURA DE BARRETA*/ 
/*				{
	 				lock=1;																																																						/*habilita el relevo ON*/
//					Timer_wait=0;
//	 			}
//		else if (*buffer=='<')
//		{																																																												/*placa*/
//		}
//}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
/*
void Cmd_Monitor()
{
		
	
}
*/
/*------------------------------------------------------------------------------
Transmito un caracter al software monitor 
------------------------------------------------------------------------------*/
void Monitor_chr (unsigned char *str,unsigned char num_char)
{
	unsigned char j;
	for (j=0; j<num_char; j++)
		{
		tx_aux(*str);
		str++;
		}
}
	
/*------------------------------------------------------------------------------
Transmito CMD de salida wiegand 
------------------------------------------------------------------------------*/
void Cmd_LPR_Salida_wiegand(unsigned char *buffer)
{
	unsigned char Buffer_Lpr[30];
	unsigned char j=3;
	Buffer_Lpr[0]=STX;																			/*inicio de cmd STx*/
	Buffer_Lpr[1]=Dir_board();															/*direccion de la tarjeta*/
	Buffer_Lpr[2]='S';																			/*cmd S que salida wiegand*/
	if(Tipo_Vehiculo!=0)																		/*Tipo de vehiculo*/
		{
			Buffer_Lpr[j++]='M';																/*moto*/
		}
		else
		{
			Buffer_Lpr[j++]='C';																/*carro*/
		}
	
		
		
		for (j=4; *buffer != '\0'; j++)												/*numero del tab o tarjeta Mf*/
			{
		   		Buffer_Lpr[j]=*buffer;
					buffer++;
				
			}
			Buffer_Lpr[j++]=':';																/*separador del tab  o tarjeta MF*/
									
			Block_read_clock_ascii(Buffer_Lpr+j);								/*año,mes,dia,hora,minutos,*/
			Buffer_Lpr[j+10]=':';																/*separador fecha*/
			Buffer_Lpr[j+11]=ETX;																/*fin de la trama*/
		
					
		
			Monitor_chr(Buffer_Lpr,j+12);												/*rutina de envio de la trama a monitor*/
}
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void Cmd_LPR_Salida(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2)
{
	
	
	unsigned char Buffer_Lpr[30];
	unsigned temp;
	unsigned char j=3;
	Buffer_Lpr[0]=STX;																/*inicio de cmd STx*/
	Buffer_Lpr[1]=Dir_board();												/*direccion de la tarjeta*/
	Buffer_Lpr[2]='S';																/*numero de digitos de transmicion NO IMPORTA NO ES VALIDADO EN PRINCIPAL*/
	
		if(*(buffer_S1_B2+8)!=0)												/*Tipo de vehiculo*/
		{
			Buffer_Lpr[j++]='M';													/*moto*/
		}
		else
		{
			Buffer_Lpr[j++]='C';													/*carro*/
		}
	
	
	do
	{
	 Buffer_Lpr[j++]=*buffer_S1_B0;									/*ticket o consecutivo*/
		buffer_S1_B0++;
	}while (*buffer_S1_B0!=0);
	
	
	
	
	
		Buffer_Lpr[j++]=':';														/*separador de la fecha de entrada*/

		temp=hex_bcd(*(buffer_S1_B2+0));								/*año a ascii*/
		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
		
		temp=hex_bcd(*(buffer_S1_B2+1));								/*mes a ascii*/
		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
		temp=hex_bcd(*(buffer_S1_B2+2));								/*Dia a ascii*/
		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
		temp=hex_bcd(*(buffer_S1_B2+3));								/*Hora a ascii*/
		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
		temp=hex_bcd(*(buffer_S1_B2+4));								/*Minutos a ascii*/
		Buffer_Lpr[j++]=((temp & 0xf0)>>4)| 0x30;
		Buffer_Lpr[j++]=((temp & 0x0f))| 0x30;
	
		
	
		Buffer_Lpr[j++]=':';														/*separador tipo fecha*/
																										/**/
				
		Buffer_Lpr[j++]=ETX;	
	
		Monitor_chr(Buffer_Lpr,j);												/*rutina de envio de la trama a monitor*/
}
void Cmd_Lpr_Int()
{
	unsigned char Buffer_Lpr[30];
	unsigned char j=3;
	unsigned char *ticket;
	Buffer_Lpr[0]=STX;																/*inicio de cmd STx*/
	Buffer_Lpr[1]=Dir_board();												/*direccion de la tarjeta*/
	Buffer_Lpr[2]='E';																/*numero de digitos de transmicion NO IMPORTA NO ES VALIDADO EN PRINCIPAL*/
	if (Tipo_Vehiculo==AUTOMOVIL)	   			//cambio del vehiculo
	{
		Buffer_Lpr[3]='C';
		Buffer_Lpr[4]=0;
	}
	else
		{
		 	Buffer_Lpr[3]=('M');
			Buffer_Lpr[4]=0;
		}
		
	ticket=Lee_No_Ticket();
	strcpy( Buffer_Lpr,ticket);
}

	