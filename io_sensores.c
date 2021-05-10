
#include <io_sensores.h>
#include <reg51.h>

/*funciones externas*/
extern void Debug_txt_Tibbo(unsigned char * str);
extern void load_and_send_info_reloj();
extern void load_and_send_id_cod();
extern void send_portERR(unsigned char cod_err);
extern unsigned char rd_eeprom (unsigned char control,unsigned int Dir); 
extern void Delay_10ms(unsigned int cntd_10ms);

sbit DataIn = P1^1;					//			
sbit sel_A = P3^5;					//Pulsador												*
sbit sel_B = P3^6;					//Entrada Sensor 2										*
sbit sel_C = P3^7;					//Entrada Sensor 1										*
sbit busy = P3^3;  					/*bussy de Entrada Interrupcion del Procesador principal			*/

#define NO_TIFIQUE_EVP			'N'
#define SI_NOTIFIQUE_EVP		'S'

#define AUTOMOVIL						0X00
#define MOTO								0X01
/*datos en eeprom*/
#define EE_CPRCN_ACTIVA				0x000C
#define EE_ADDRESS_HIGH_BOARD		0X0012

extern bit COMPARACION_ACTIVA;
extern  unsigned char Tipo_Vehiculo;
bit		PULSADOR_BOTTON = 0;

//***************************************************************************************************
void sel_Funcion(void)
{
	sel_A=0;
	sel_B=0;
	sel_C=0;
}
//***************************************************************************************************
void sel_Auto(void)
{
	sel_A=0;
	sel_B=1;
	sel_C=0;
}
//***************************************************************************************************
void sel_Pulsa(void)
{
	sel_A=1;
	sel_B=1;
	sel_C=0;
}

//***************************************************************************************************
void sel_Sensor2(void)
{
	sel_A=0;
	sel_B=0;
	sel_C=1;
}
//***************************************************************************************************
void sel_Sensor1(void)
{
	sel_A=1;
	sel_B=0;
	sel_C=1;
}
//***************************************************************************************************
void sel_Dir1(void)
{
	sel_A=0;
	sel_B=1;
	sel_C=1;
}
//***************************************************************************************************
void sel_Dir2(void)
{
	sel_A=1;
	sel_B=1;
	sel_C=1;
}
void Botton ()
{
	sel_Pulsa();
	if (DataIn != 1)	
	{
		Debug_txt_Tibbo((unsigned char *) "Pulsador Activo\r\n");				//el pulsador fue presionado
		PULSADOR_BOTTON = 1;
	}
	
}

char ValidaSensor(void)
{
	
char	Valido=0;
int	Tmin=500;
	while ((DataIn==1)&&(Valido==0))
	{
		Tmin--;
		if (Tmin==0)
		{
			 Valido=1;
		}
	}
	return Valido;
}
char ValidaSensor_cero(void)
{
	
char	Valido=1;
int	Tmin=500;
	while ((DataIn==0)&&(Valido==1))
	{
		Tmin--;
		if (Tmin==0)
		{
			 Valido=0;
		}
	}
	return Valido;
}
/*------------------------------------------------------------------------------
Funcion q debuelve la direccion de la tarjeta
------------------------------------------------------------------------------*/
unsigned char Dir_board()
{
	unsigned char Board=0x01;
	unsigned char Board_High;
	sel_Dir1();
	if (DataIn==1)
	{
	 	Board++;
	}
	sel_Dir2();
	if (DataIn==1)
	{
		Board=Board+2;
	}
	if(Board == 0x01)
	{	
	Board_High=rd_eeprom(0xa8,EE_ADDRESS_HIGH_BOARD);
	
		if(Board_High != 0)
		{
			if(Board_High == 0xff)
			{
				Board=0x01;
			}
			else 	Board= Board_High ;
		}	
	
	}
	return (Board+0x30);
}
unsigned char Dir_Board_Monitor()
{
		unsigned char Board_High;
		
		Board_High=rd_eeprom(0xa8,EE_ADDRESS_HIGH_BOARD);
		if(Board_High != 0)
		{
			if(Board_High == 0xff)
			{
				Board_High=0x01;
			}
		}
		return  Board_High+0x30;
}
/*------------------------------------------------------------------------------
Funcion q debuelve la direccion de la tarjeta
------------------------------------------------------------------------------*/
unsigned char Valida_Sensor1_Auto()
{
	unsigned char sensor; 
				sel_Sensor1();	
				if (DataIn==0)														// sensor1  se encuentra activo puede ser carro, si esta inhabilitado debe ser moto					
				{  sel_Auto();
					if (DataIn==0)
					{					  															// sensor Auto activo es un carro 
					   Debug_txt_Tibbo((unsigned char *) "Sensores Auto y sensor1 activos.\n\r");
					   Tipo_Vehiculo=AUTOMOVIL;
						sensor= 0xff;
						Botton ();
					}
					else
					{
							Debug_txt_Tibbo((unsigned char *) "Sensor activo sensor1.\n\r");
							Tipo_Vehiculo=MOTO;
							sensor=  0xff ;
							Botton ();
					}
						
				   	
				}
				else
				{ 	sel_Auto();
					if (DataIn==0)
					{
					
					Debug_txt_Tibbo((unsigned char *) "Detectado. Sensor Auto.\n\r");
						Tipo_Vehiculo=MOTO;
						sensor= 0xff;	
						Botton ();
					}
					else
					{
						Debug_txt_Tibbo((unsigned char *) "Sensores no detectados.\n\r");
						sensor=  0x00;	
					}
						

				}
				
				return sensor;
}
/*------------------------------------------------------------------------------
Detecto la activacion de los sensores de salida
------------------------------------------------------------------------------*/

unsigned char  ValidaSensoresPaso(void)
{
 unsigned char sensor;
		sel_Sensor2();																//garantiso q la barrera se encuentre en posicion baja	
   		if ((DataIn==1))				
		{  
			if (ValidaSensor()==1)
			{
				sensor=Valida_Sensor1_Auto();
	
			}
			else 
			{
				Debug_txt_Tibbo((unsigned char *) "Vehiculo Esperando Salir.\n\r");
				return 0x00;
			}
		}else 
		{
		Debug_txt_Tibbo((unsigned char *) "Vehiculo Saliendo. Un momento.\n\r");
		return 0x00;
		}
	return sensor;
}


	
/*------------------------------------------------------------------------------
funcion de msj en lcd de informacion
toggle=0		envia por el pto paralelo STX, cmd (i), año, mes, dia, hora, minutos, seg, dia de la semana, ETX
toggle=1		
toggle=2		envia ID y COD_PARK   por el pto paralelo STX, cmd (D), ID_CLIENTE, COD_CLIENTE, ETX  
------------------------------------------------------------------------------*/
 void msj_lcd_informativo()
 {
	static unsigned char contador=0;
 unsigned char info=0;
 static unsigned char toggle=0;
	 contador++;
	 if (contador >= 50)
	 {
			contador = 0;
  		sel_Funcion();
		if ((DataIn==0)&&(info==0))
		{
			if (ValidaSensor_cero()==0)
			{
 		   if ((toggle==0)&&(info==0))
			{
			 if (busy==1)
				{
					info=1;
					load_and_send_info_reloj();	
					toggle=1;	
				}
					if (ValidaSensor() == 1)
					{
						Delay_10ms(10);
					}
			}
			else if((toggle==1)&&(info==0))
				{
					if (busy==1)
					{
					info=1;
					load_and_send_id_cod();	
					toggle=0;	
					}
				}
			}
		}
		 sel_Funcion();   								// el antirebote
			if (DataIn==1)
			{
			info=0;
			}
		}
}
