#include<pantallas.h>
#include <reg51.h>
#include <string.h>

sbit sel_com = P0^7;				//Micro switch		


/*funciones externas*/
extern char putchar (char c);
extern void Block_read_Clock(unsigned char *datos_clock);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_Dividir_texto();
extern void clean_tx();
extern void Debug_txt_Tibbo(unsigned char * str);
extern void          _nop_     (void);
extern void Debug_chr_Tibbo(unsigned char Dat);

#define ERROR_LOOP							0XE0
#define TARJETA_INVALIDA				0XE1
#define TARJETA_SIN_FORMATO	    0xDF
#define ERROR_COD_PARK					0XE5
#define SIN_INGRESO							0XE6
#define SIN_PAGO								0XE7
#define EXCEDE_GRACIA						0XE8
#define FUERA_DE_LINEA					0XB6
#define LECTURA_DE_TARJETAS			0xB0

#define AUDIO_CAJA				0XA1
#define TARJETA_VENCIDA			0XEC
#define TARJETA_ATASCADA		0XEF

#define BIENVENIDO							0XFE
#define GRACIAS									0XFF

#define NOTIFIQUE_EVP						'N'
#define NO_NOTIFIQUE_EVP				'S'
#define COMPARANDO_PLACA				'P'						/*msj de eeror de placa*/
#define ENVIANDO_COD						'D'
#define INFO1										'I'
#define INFO2										'i'

/*variables externas */
extern unsigned char 	Raspberry;
/*------------------------------------------------------------------------------
transmite el caracter pto serie
data_com = al caracter a escribir
enable_char_add = si esta en (1) envia un null (0) adicional, si es (0) no envia caracter adicional
------------------------------------------------------------------------------*/
void tx_chrlcd (unsigned char data_com, unsigned char enable_char_add)
{
	unsigned char d;
	d=putchar(data_com);
	if (enable_char_add != 0)	d=putchar(0x00);
	
}

/*------------------------------------------------------------------------------
envia un msj asta null(0)
msg= apuntador del msj
enable_char_add = si esta en (1) envia un null (0) adicional, si es (0) no envia caracter adicional
------------------------------------------------------------------------------*/
void LCD_txt (unsigned char * msg,unsigned char enable_char_add )
{
	unsigned char i;
	 
	for (i=0; msg[i] != '\0'; i++)
	{
 	 	tx_chrlcd(msg[i],enable_char_add);
	}
}
/*------------------------------------------------------------------------------
Escribo el reloj en ascii en bloque 
msg= apuntador del msj
length_char longitud de la tram
enable_char_add = si esta en (1) envia un null (0) adicional, si es (0) no envia caracter adicional
------------------------------------------------------------------------------*/
void LCD_txt_num_char(unsigned char * msg,unsigned char length_char, unsigned char enable_char_add)
{
	unsigned char i;
	 
	for (i=0; i<length_char; i++)
	{
 	 	tx_chrlcd(msg[i],enable_char_add);
	}
}
/*------------------------------------------------------------------------------
Transmite una trama por el pto serie con el protocolo para  raspberry
msg= es el apuntador del msj
------------------------------------------------------------------------------*/
void Raspberry_data (unsigned char * msg)
{
	static unsigned char i;
	unsigned char lenth_cadena;
	unsigned char d;
	
	lenth_cadena=strlen(msg);
	Debug_chr_Tibbo(lenth_cadena);
	Debug_Dividir_texto();	
	Debug_txt_Tibbo(msg);	
	Debug_Dividir_texto();		
	for (i=0;  i<lenth_cadena+1 ; i++)
	{
		d=putchar(*msg);
		msg++;
	
	
 	 
	}
	
	
	
}


/*------------------------------------------------------------------------------
Escribo el reloj en ascii en bloque 
AA 80 28 trama de inicio de configuracion de la pantalla
07 numero de caracteres de la trama de reloj
20 19 03 26 09 21 20  el dato del reloj
------------------------------------------------------------------------------*/
void Reloj_Pantalla_Lcd()
{

 unsigned char Ini_Clock_LCD   []={0xaa,0x80,0x28,0x07,0x20,0x00,0,0,0,0,20,0,0} ;
					if (Raspberry==0)
					{	
					sel_com=0;																																			/*switch del pto serie a la pantalla*/
					Block_read_Clock(Ini_Clock_LCD+5);																							/*Leo el reloj programado*/
					//Debug_Dividir_texto();																													/*lineas de separacion del texto*/
					//DebugBufferMF(Ini_Clock_LCD,12,0);																							/*muestra la trama por debug*/
					//Debug_Dividir_texto();																													/*linea de separacion de texto*/
					REN = 0;																																				/*inhabilita recepcion de datos*/
					LCD_txt_num_char(Ini_Clock_LCD,13,0);																						/*cmd de inicializacion del reloj del lcd*/
																											
					REN = 1;																																				/*habilita recepcion de datos*/
					sel_com=1;	
						/*switch pto serie a verificador o expedidor */
					}
					else
					{
						sel_com=0;
						Ini_Clock_LCD [0]=0;
						Block_read_Clock(Ini_Clock_LCD);
						Raspberry_data((unsigned char  *) "d;hora");
						sel_com=1;	
					}
}
/*------------------------------------------------------------------------------
Rutina de msj de pantalla
------------------------------------------------------------------------------*/
void PantallaLCD(unsigned char cod_msg)
{

unsigned char Ini_LCD_Line_one   []={0xaa,0x80,0x18,0x01,0x02,0x00} ;
//unsigned char Ini_LCD_Line_two   []={0xaa,0x80,0x18,0x02,0x02,0x00} ;
//unsigned char Ini_Off_Line []={0xaa,0x80,0x18,0x01,0x03,0x00} ;
	
unsigned char num_chr;

 	
		sel_com=0;
	
		if (Raspberry==0)
		{
			LCD_txt (Ini_LCD_Line_one,0);
			
			switch (cod_msg)
			{
		
				case 'P':
					
					num_chr=strlen((unsigned char  *) "ERROR: VALIDANDO PLACA... ");
		   		tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "ERROR: VALIDANDO PLACA... ",1);
					
					break;

				case ERROR_LOOP:
					
					num_chr=strlen((unsigned char *) "ERROR: LOOP1 SIN PRESENCIA VEHICULAR ");	
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "ERROR: LOOP1 SIN PRESENCIA VEHICULAR ",1);
					
					break;
				
				case TARJETA_INVALIDA:
					
					num_chr=strlen((unsigned char *) "ERROR: TARJETA INVALIDA ");	
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "ERROR: TARJETA INVALIDA ",1);
					
					break;
				
				case ERROR_COD_PARK:
		
					num_chr=strlen((unsigned char *) "TARJETA NO ES DEL PARQ. ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "TARJETA NO ES DEL PARQ. ",1);
		
					break;
					
				case TARJETA_SIN_FORMATO:
		
					num_chr=strlen((unsigned char *) "TARJETA SIN FORMATO ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "TARJETA SIN FORMATO ",1);

				  break;
					
				case SIN_PAGO:
		
					num_chr=strlen((unsigned char  *) "TARJETA NO REGISTRA PAGO ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "TARJETA NO REGISTRA PAGO ",1);

					break;
						
				case EXCEDE_GRACIA:
		
					num_chr=strlen((unsigned char  *) "EXCEDE TIEMPO DE GRACIA ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "EXCEDE TIEMPO DE GRACIA ",1);
			
					break;
				
				case 	FUERA_DE_LINEA:
					
					num_chr=strlen((unsigned char *) "FUERA DE LINEA ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "FUERA DE LINEA ",1);
				
					break;
				
				case	BIENVENIDO:
					
					num_chr=strlen((unsigned char *) "BIENVENIDO ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "BIENVENIDO ",1);
					
					break;
				
				case	SIN_INGRESO:
					
					num_chr=strlen((unsigned char *) "ERROR: SIN INGRESO ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "ERROR: SIN INGRESO ",1);
					
					break;
								
			}
				sel_com=1;	
	
		}
      else
      {
         sel_com=0;   
         switch (cod_msg)
         {
      
            case 'P':
                    
                 
									Raspberry_data((unsigned char  *) "a;98;ERROR VALIDANDO PLACA \n\r\0");
                  break;
            case AUDIO_CAJA:
                  
									Raspberry_data((unsigned char  *) "a;27;¡ NO REGISTRA PAGO ! \n\r\0");
                  break;
            case ERROR_LOOP:
               
									Raspberry_data((unsigned char  *) "a;98;SIN PRESENCIA VEHICULAR \n\r\0");
                  break;
            case TARJETA_INVALIDA:
               
									Raspberry_data((unsigned char  *) "a;98;TARJETA INVALIDA \n\r\0");
                  
                  break;
            case 0xe2:

									Raspberry_data((unsigned char  *) "a;98;ERROR: ANTICOLL \n\r\0");
                  break;
            case 0xe3:

									Raspberry_data((unsigned char  *) "a;98;ERROR: SELECT CARD \n\r\0");
                  break;
            case 0xe4:
                
									Raspberry_data((unsigned char  *) "a;98;TARJETA INVALIDA \n\r\0");
                  break;
            case ERROR_COD_PARK:
                 
									Raspberry_data((unsigned char  *) "a;98;CARD NO ES DE PARQUEADERO \n\r\0");
                  break;
            
            case SIN_INGRESO:
               
									Raspberry_data((unsigned char  *) "a;30;TARJETA SIN INGRESO \n\r\0");
                  break;
            case SIN_PAGO:
               
									Raspberry_data((unsigned char  *) "a;27;TARJETA NO REGISTRA PAGO\n\r\0");
                  break;
            case EXCEDE_GRACIA:
                
									Raspberry_data((unsigned char  *) "a;28;EXCEDE TIEMPO DE GRACIA \n\r\0");
                  break;
            case 0xe9:
                
									Raspberry_data((unsigned char  *) "a;98;ERROR: GRABACION DE DATOS \n\r\0");
                  break;
            case 0xeb:
                
									Raspberry_data((unsigned char  *) "a;27;DIRIJASE A CAJA \n\r\0");
                  break;
            case TARJETA_VENCIDA:
                 // Raspberry_txt((unsigned char  *) "a;07;");
									Raspberry_data((unsigned char  *) "  MENSUALIDAD VENCIDA !   ");
                  Raspberry_data((unsigned char  *) "  VENCIO : 20");
         // falta la parte alta del vencimiento
                  break;
            case 0xed:
                
									Raspberry_data((unsigned char  *) "a;98;SIN RESPUESTA \n\r\0");
                  break;
            case TARJETA_ATASCADA:
                 
									Raspberry_data((unsigned char  *) "TARJETA ATASCADA \n\r\0");
                  break;
            case 0xf8:
                  
                 
									Raspberry_data((unsigned char  *) "a;98;SALIDA INVALIDA \n\r\0");
                  break;
            case GRACIAS:
                 
									Raspberry_data((unsigned char  *) "a;26;GRACIAS... \n\r\0");
                  break;
						
						case 	FUERA_DE_LINEA:
							Raspberry_data((unsigned char  *) "a;99;FUERA DE LINEA\n\r\0");
                  break;
						case 	'0':		/*Bienvenido.pulse el boton*/
            Raspberry_data((unsigned char  *) "a;00;BIENVENIDO. PULSE EL BOTON\n\r\0");
                  break;
						case 	'9':		/*NO olvide pasar por el punto de pago*/
            Raspberry_data((unsigned char  *) "a;99;NO OLVIDE PASAR POR EL PUNTO DE PAGO\n\r\0");
                  break;
						case 	'X':		/*Bienvenido*/
            Raspberry_data((unsigned char  *) "a;99;BIENVENIDO\n\r\0");
                  break;
						case 	'C':		/*Mensualidad cancelada*/
            Raspberry_data((unsigned char  *) "a;97;MENSUALIDAD CANCELADA ... DIRIJASE A ADMINISTRACION\n\r\0");
                  break;
						case 	'a':		/*Mensualidad cancelada*/
            Raspberry_data((unsigned char  *) "a;99;NIVEL BAJO DE TARJETAS\n\r\0");
                  break;
         }
         sel_com=1;   
      }
      
     

}
void PantallaLCD_LINEA_2(unsigned char cod_msg, unsigned char *buffer)
{
	
unsigned char Ini_LCD_Line_one   []={0xaa,0x80,0x18,0x01,0x02,0x00} ;


unsigned char num_chr;
	
sel_com=0;
	
		//if (Raspberry==0)
		//{
		
			
			switch (cod_msg)
			{
		
				case	LECTURA_DE_TARJETAS:
					LCD_txt (Ini_LCD_Line_one,0);
					num_chr=strlen((unsigned char *) "WIEGAND ");
					num_chr=num_chr+strlen(buffer)+1;
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "WIEGAND ",1);	 											/*funcion q trasmite el msj al LCD  y el (1) coloca los caracteres NULL*/
					LCD_txt(buffer,1);	 																									/*funcion q trasmite el msj al LCD  y el (1) coloca los caracteres NULL*/
					
				
				
				
					break;
				
				
	}
				sel_com=1;	
}
