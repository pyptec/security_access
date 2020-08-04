/*------------------------------------------------------------------------------
MAIN.C:  sistema verificador

Diseñado Ing. Jaime Pedraza
------------------------------------------------------------------------------*/
#include <main.h>
/*------------------------------------------------------------------------------
interrupcion por timer 
ValTimeOutCom se decrementa cada overflow de la interrupcion
Timer_wait		Incrementa cada overflow de la interrrupcion
clock=22.1184mhz
ciclo de mqn= clock/12 =0.5nseg
timer= ciclo mqn* reloj = 0.5 x65535 =32
temporizado=timer* ValTimeOutCom = 32*100=320ms
------------------------------------------------------------------------------*/
 timer0_int() interrupt 1 using 2
    {
			
			ValTimeOutCom--;
			if (ValTimeOutCom == 1)
			{
				Timer_wait++;
			}
			
			Timer_tivo++;
			TF0=0;
			
		}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void ini_timer(void)
{

	TMOD=(TMOD & 0xF0) | 0x01;// configuro el reg TMOD sin precalador a 16 bits
		
 	TH0=0X00;								//registro de la parte alta del timer0 de 16 bits									*						
	TL0=0X00;								// registro de la parte baja del timer0
	TR0=1;									// habilita el timer0 reg TCOM bit 4 para interrup

}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void on_Timer0_Interrup(void)
{
	TF0=0;									//registro TCON bit 5 bandera de overflow 
	ET0=1;									// enable interrupcion bit 1 del registro IENO
	
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
void off_Timer0_Interrup(void)
{
	ET0=0;									// enable interrupcion bit 1 del registro IENO
	
}
/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
_getkey waits until a character is received from the serial port.  This may not
be the exact desired operation (for example if the buffer is empty, this
function hangs waiting for a character to be received).
------------------------------------------------------------------------------*/
char _getkey (void)
{
int k;

do
  {
  k = com_getchar ();
  }
while (k == -1);

return ((unsigned char) k);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
char putchar (char c)
{
volatile unsigned int i;

while (com_putchar (c) != 0)
  {
  for (i=0; i<1000; i++)
    {
    /*** DO NOTHING ***/
    }
  }

return (c);
}
/*------------------------------------------------------------------------------
escribir una cadena de datos por pto serie
------------------------------------------------------------------------------*/
void EscribirCadenaSoft_buffer(unsigned char *buffer,unsigned char tamano_cadena)
{
	unsigned char i,d;
		for(i=0;i<tamano_cadena;i++)
   	{
      d=putchar(*buffer);
			buffer++;
    }
}

/*------------------------------------------------------------------------------
Rutina de las condiciones iniciales del board
------------------------------------------------------------------------------*/
void inicia_board(void)
{
cond_ini_tibbo();							/* inicia tibbo*/
cond_ini_clock();							/* inicia clock*/
cond_ini_pto();								/*Inicializacion del pto*/
Atascado=0;										/*prendo el verificador*/
sel_com=1;										/*seleccionar el pto serie */
lock=0;												/*barrera off*/	
	
}

/*------------------------------------------------------------------------------
Rutina que lee la eeprom, los bit de configuracion 
------------------------------------------------------------------------------*/
void variable_inicio()
{
	
	
	T_GRACIA=rd_eeprom(0xa8,EE_TIEMPO_GRACIA);
	SIN_COBRO=rd_eeprom(0xa8,EE_SIN_COBRO);
	Debug_Tibbo=rd_eeprom(0xa8,EE_DEBUG);
	USE_LPR=rd_eeprom(0xa8,EE_USE_LPR);
	Raspberry = rd_eeprom(0xa8,EE_TIPO_PANTALLA);
	
}
/*------------------------------------------------------------------------------
Note that the two function above, _getkey and putchar, replace the library
functions of the same name.  These functions use the interrupt-driven serial
I/O routines in SIO.C.
------------------------------------------------------------------------------*/


void main (void)
{
	
	static unsigned char buffer[40];
	unsigned char Estado_Comunicacion_Secuencia_MF=SEQ_INICIO;
	unsigned char Length_trama;

	
ini_timer();																															/* initialize interrupt timer0 */
on_Timer0_Interrup();																											/* habilita el  timer0 */
com_initialize ();              																					/* initialize pto serie */
com_baudrate ();            																							/* setup for 9600 baud */
inicia_board();																														/*Rutina de las condiciones iniciales del board*/
EA = 1;                         																					/* Enable Interrupts global*/

time_bit();																																/*solo para pruebas*/
Delay_20us(33);
time_mbit();
//	Debug_txt_Tibbo((unsigned char *) "pqui estoy");	
tx_aux('a');	

sel_Funcion();																															/*el pulsador funcion es el cmd q da la entrada a programacion */
	
	if (DataIn==0)							
	{
	menu();																																		/*menu de configuracion*/
	}
	//Debug_txt_Tibbo((unsigned char *) "aqui estoy");

	variable_inicio();																												/*leo y cargo  las variables de inicio de configuracion*/
while(Secuencia_inicio_expedidor());																				/* procedimiento de inicio de transporte (reset, y grabar eeprom)*/
	ValTimeOutCom=TIME_CARD;
																							
	while (1) 																																/* Loop Principal								*/	
	{   
	
 				
				if (rx_ip==0)																													/*pregunto si llega datos de monitor pto serie emulado*/
				{
					 Rx_Monitor();
				}
				
			
					
				Estado_Comunicacion_Secuencia_MF=SecuenciaExpedidorMF(Estado_Comunicacion_Secuencia_MF);					//SecuenciaExpedidor();
				
						
					  																				
			
				if (busy==0)																													/*comunicacion con el pto paralelo*/
				{
			
				Length_trama=recibe_port(buffer);																		/*recibe informacion del pto paralelo*/
				/*solo de prueba*/		
				Debug_pto_paralelo(buffer,Length_trama);					
						
				Valida_Trama_Pto(buffer,Length_trama);																/*valido la informacion recibida */
				 
				}
		msj_lcd_informativo();																									/*muestra la informacion de  ID cliente, cod parque, fecha,comparacion*/
	

		//}
	}

}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

