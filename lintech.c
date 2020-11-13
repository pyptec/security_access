#include<lintech.h>
/*----------------------------------------------------------------------------
tiempo de delay entre funciones
------------------------------------------------------------------------------*/

#define 	TIME_CARD					100		//50
#define		TIME_MOV					150
/*----------------------------------------------------------------------------
definicion de datos de trama lintech
------------------------------------------------------------------------------*/

#define 	ETX								03
#define 	STX_LINTECH				0xf2
/*------------------------------------------------------------------------------
Definicion de Lintech en el comando Inicializa
------------------------------------------------------------------------------*/

#define	TO_FRONT				'0'
#define	CAPTURE_BOX			'1'
#define	SIN_MOVIMIENTO	'3'

/*------------------------------------------------------------------------------
 definiciones de lintech en el comando Card_Insercion
------------------------------------------------------------------------------*/

#define Habilita			 	0x30
#define Inhabilita			0x31

/*------------------------------------------------------------------------------
Definicion de Lintech en el comando mover tarjeta (Mov_Card)
------------------------------------------------------------------------------*/

#define 	MovPos_Front				'0'		
#define 	MovPos_IC						'1'
#define  	MovPos_RF						'2'
#define  	MovPos_Capture			'3'
#define 	MovPos_EjectFront		'9'


#define	SENSOR_DETAIL				0x31
#define SENSOR_NORMAL				0x30
/*----------------------------------------------------------------------------
definicion de recepcion serial 
------------------------------------------------------------------------------*/

#define  ESPERA_RX 					0  					//espera el primer cmd de recepcion del verificado 


/*funciones portotipo*/
extern void Debug_txt_Tibbo(unsigned char * str);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void EscribirCadenaSoft_buffer(unsigned char *buffer,unsigned char tamano_cadena);
extern void Debug_chr_Tibbo(unsigned char Dat);
extern unsigned char Dir_board();

/*atributos */
extern unsigned char g_cEstadoComSoft;
extern unsigned char ValTimeOutCom;

/*externo bit*/

extern bit buffer_ready;
/*----------------------------------------------------------------------------
funcion de inicializacion del transporte

------------------------------------------------------------------------------*/

void Inicializa(unsigned char TipoMovimiento)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];
	bcc=0;
	if ((TipoMovimiento==SIN_MOVIMIENTO)||(TipoMovimiento==TO_FRONT)||(TipoMovimiento==CAPTURE_BOX))
	{
		
		Debug_txt_Tibbo((unsigned char *) "Inicializa Dispensador\r\n\r\n");
		
		g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X03;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]='0';
		g_scArrTxComSoft[6]=TipoMovimiento;
		g_scArrTxComSoft[7]=ETX;
		for (j=0; j<8; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[8]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,9,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	}
}

/*------------------------------------------------------------------------------
cmd de lintech que responde en que estado de los sensores se encuentra

(30) solo envia el resumen de los sensores
(31) da un reporte detallado de los sensores
S_DETAIL				0x31
S_NORMAL				0x30
detalle=s_detail
------------------------------------------------------------------------------*/

void Check_Status(unsigned char Detalle)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];

	if (Detalle == SENSOR_NORMAL)
		{
		 	Debug_txt_Tibbo((unsigned char *) "Check_Status SENSOR_NORMAL\r\n");
		}
		else
		{
			Debug_txt_Tibbo((unsigned char *) "Check_Status SENSOR_DETAIL\r\n");
		}
	bcc=0;

	g_scArrTxComSoft[0]=STX_LINTECH;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X03;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]='1';
	g_scArrTxComSoft[6]=Detalle;
	g_scArrTxComSoft[7]=ETX;
	for (j=0; j<8; j++)
	{
		bcc=g_scArrTxComSoft[j]^bcc;
	}
	g_scArrTxComSoft[8]=bcc;
	buffer_ready=0;
	g_cEstadoComSoft=ESPERA_RX;
	DebugBufferMF(g_scArrTxComSoft,9,0);
	EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);
	ValTimeOutCom=TIME_CARD;
}

/*------------------------------------------------------------------------------
Procedimiento que habilita la insercion o inhabilita la insersion
(31) inhabilita
(30) habilita
tipo=1 habilita la insercion de tarjeta
tipo=0 inhabilita la insersion
------------------------------------------------------------------------------*/

void Card_Insercion(char Tipo)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];
	if (Tipo==Habilita)
	{
		Debug_txt_Tibbo((unsigned char *) "Habilita Insersion\r\n");
		g_scArrTxComSoft[6]=Habilita;
	}
	else
	{
		
		Debug_txt_Tibbo((unsigned char *) "Inhabilita Insersion\r\n");
		g_scArrTxComSoft[6]=Inhabilita;
	
	}

	bcc=0;

	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X03;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]='3';

	g_scArrTxComSoft[7]=ETX;
	for (j=0; j<8; j++)
	{
		bcc=g_scArrTxComSoft[j]^bcc;
	}

	g_scArrTxComSoft[8]=bcc;

	buffer_ready=0;
	g_cEstadoComSoft=ESPERA_RX;
	DebugBufferMF(g_scArrTxComSoft,9,0);
	EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);
	ValTimeOutCom=TIME_CARD;
}

/*------------------------------------------------------------------------------
CMD q mueve la tarjeta 
 	MovPos_Front				'0'		
 	MovPos_IC						'1'
  MovPos_RF						'2'
  MovPos_Capture			'3'
 	MovPos_EjectFront		'9'
------------------------------------------------------------------------------*/

void Mov_Card(unsigned char Posicion)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[10];
	bcc=0;

 	if ((Posicion==MovPos_RF)||(Posicion==MovPos_IC)||(Posicion==MovPos_Front)||(Posicion==MovPos_EjectFront)||(Posicion==MovPos_Capture))
	{
	 	if (Posicion==MovPos_RF)
		{
		 	Debug_txt_Tibbo((unsigned char *) "Moviendo Tarjeta a RF\r\n");
		}
		else if (Posicion==MovPos_IC)
		{
			Debug_txt_Tibbo((unsigned char *) "Moviendo Tarjeta a IC\\r\n");
   		}
		else if (Posicion==MovPos_Front)
		{
			Debug_txt_Tibbo((unsigned char *) "Moviendo Tarjeta a Bezel\r\n");
 		}
		else if (Posicion==MovPos_EjectFront)
		{
			Debug_txt_Tibbo((unsigned char *) "Expulsando Tarjeta\r\n");
		}
		else if (Posicion==MovPos_Capture)
		{
			Debug_txt_Tibbo((unsigned char *) "Capturando Tarjeta\r\n");
 		}

		g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X03;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]='2';
		g_scArrTxComSoft[6]=Posicion;
		g_scArrTxComSoft[7]=ETX;
		for (j=0; j<8; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[8]=bcc;
		buffer_ready=0;
		g_cEstadoComSoft=ESPERA_RX;
		DebugBufferMF(g_scArrTxComSoft,9,0);
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);
		ValTimeOutCom=TIME_CARD;
	}

}

//*******************************************************************************************
// rutina q mira el tipo de tarjeta si es valido para el uso 	
//*******************************************************************************************

void Aut_Card_check_Status(void)
{
unsigned char j, bcc;
unsigned char	g_scArrTxComSoft[10];
		  bcc=0;
	
	Debug_txt_Tibbo((unsigned char *) "Aut_Card_check_Status\r\n");
 	
	g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X03;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]=0x50;
		g_scArrTxComSoft[6]=0x31;
		g_scArrTxComSoft[7]=ETX;
	   	for (j=0; j<8; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[8]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,9,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	}	
/*	------------------------------------------------------------------------------
	------------------------------------------------------------------------------*/
void Clave_Seguridad_S2(void)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[21];
		  bcc=0;
	Debug_txt_Tibbo((unsigned char *) "Clave Seguridad S2\r\n");


	g_scArrTxComSoft[0]=STX_LINTECH;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X0e;	// Numero Datos
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0x60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0x20;
   	g_scArrTxComSoft[9]=0x00;
	g_scArrTxComSoft[10]=0x02;

	g_scArrTxComSoft[11]=0x06;

	g_scArrTxComSoft[12]=0x41;
	g_scArrTxComSoft[13]=0x63;
	g_scArrTxComSoft[14]=0x53;
   	g_scArrTxComSoft[15]=0x45;
	g_scArrTxComSoft[16]=0x76;
	g_scArrTxComSoft[17]=0x50;


	g_scArrTxComSoft[18]=ETX;
	
	 	for (j=0; j<19; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[19]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,20,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,20);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	

}
/*------------------------------------------------------------------------------
CMD q programa la clave en el verificador o transporte
------------------------------------------------------------------------------*/

	void Dwload_EEprom (void)
{
 	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[21];
	bcc=0;
	Debug_txt_Tibbo((unsigned char *) "Download MF EEprom\r\n");
	
	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X0E;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0x60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0Xd0;
	g_scArrTxComSoft[9]=0X00;
	g_scArrTxComSoft[10]=0X01;
	g_scArrTxComSoft[11]=0x06;
	g_scArrTxComSoft[12]='3'; //33
	g_scArrTxComSoft[13]='V';//56
 	g_scArrTxComSoft[14]='0';//30
	g_scArrTxComSoft[15]='p';//70
	g_scArrTxComSoft[16]='4';//34
	g_scArrTxComSoft[17]='r';//72
	g_scArrTxComSoft[18]=ETX;
	
 	for (j=0; j<19; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[19]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,20,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,20);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
}

/*------------------------------------------------------------------------------
Funcion q verifica si la clave y la carga en el transporte
------------------------------------------------------------------------------*/

void LoadVerify_EEprom(void)
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[15];
	bcc=0;
	Debug_txt_Tibbo((unsigned char *) "Carga y Verifica de EEprom\r\n");


	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X07;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0x60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0x21;
  g_scArrTxComSoft[9]=0x00;
	g_scArrTxComSoft[10]=0x01;
	g_scArrTxComSoft[11]=ETX;
	

	

	for (j=0; j<12; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[12]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,13,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,13);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
		
}

/*------------------------------------------------------------------------------
Funcion q lee la MF dandole el sector y el bloque
------------------------------------------------------------------------------*/

void RD_MF(unsigned char Sector, unsigned char Bloque)
{

	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[15];
	bcc=0;

	Debug_txt_Tibbo((unsigned char *) "Leyendo MF > Sector: ");
	Debug_chr_Tibbo(Sector);
	
	Debug_txt_Tibbo((unsigned char *) " Bloque: ");
	Debug_chr_Tibbo(Bloque);
	Debug_txt_Tibbo((unsigned char *) "\r\n");


	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;									  
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=0X08;
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0X60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0xb0;
	g_scArrTxComSoft[9]=Sector;
	g_scArrTxComSoft[10]=Bloque;
	g_scArrTxComSoft[11]=0x01;
	g_scArrTxComSoft[12]=ETX;

	for (j=0; j<13; j++)
	{
		bcc=g_scArrTxComSoft[j]^bcc;
	}
	g_scArrTxComSoft[13]=bcc;


		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,14,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,14);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;

}

/*------------------------------------------------------------------------------
Funcion q lescribe la MF dandole el sector y el bloque y los datos
------------------------------------------------------------------------------*/


void WR_MF(unsigned char Sector, unsigned char Bloque,unsigned char *buffer)	   
	 
{
	unsigned char j, bcc;
	unsigned char	g_scArrTxComSoft[31];
	bcc=0;

	Debug_txt_Tibbo((unsigned char *) "Escribe MF > Sector: ");
	Debug_chr_Tibbo(Sector);
	
	Debug_txt_Tibbo((unsigned char *) " Bloque: ");
	Debug_chr_Tibbo(Bloque);
	Debug_txt_Tibbo((unsigned char *) "\r\n");
																	  
	g_scArrTxComSoft[0]=0xF2;
	g_scArrTxComSoft[1]=0X00;									  
	g_scArrTxComSoft[2]=0X00;
	g_scArrTxComSoft[3]=24;																	/* Numero Datos a programar */ 
	
	g_scArrTxComSoft[4]='C';
	g_scArrTxComSoft[5]=0X60;
	g_scArrTxComSoft[6]='3';
	g_scArrTxComSoft[7]=0x00;
	g_scArrTxComSoft[8]=0xd1;
	g_scArrTxComSoft[9]=Sector;															//Sector;
	g_scArrTxComSoft[10]=Bloque;														//Bloque;
	g_scArrTxComSoft[11]=0x01;

		for (j=0; j<=16; ++j)
			{
				g_scArrTxComSoft[j+12]=*(buffer + j);	
			}
	
	g_scArrTxComSoft[28]=ETX;
	
	for (j=0; j<=28; j++)
	{
		bcc=bcc^g_scArrTxComSoft[j];
	}
	g_scArrTxComSoft[29]=bcc;

	
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,30,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,30);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	
}
//*******************************************************************************************
// rutina q mira el tipo de tarjeta si es valido para el uso 	
//*******************************************************************************************

void Unique_Identifier_UID(void)
{
unsigned char j, bcc;
unsigned char	g_scArrTxComSoft[10];
		  bcc=0;
	
	Debug_txt_Tibbo((unsigned char *) "UID\r\n");
 	
	g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X05;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]=0x60;
		g_scArrTxComSoft[6]=0x30;
		g_scArrTxComSoft[7]=0x41;
		g_scArrTxComSoft[8]=0x30;
		g_scArrTxComSoft[9]=ETX;
	   	for (j=0; j<10; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[10]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,11,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,11);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;
	}	
void Power_off(void)
{
unsigned char j, bcc;
unsigned char	g_scArrTxComSoft[10];
		  bcc=0;
	
	Debug_txt_Tibbo((unsigned char *) "POWER OFF UID\r\n");
 	
	g_scArrTxComSoft[0]=STX_LINTECH;
		g_scArrTxComSoft[1]=0X00;
		g_scArrTxComSoft[2]=0X00;
		g_scArrTxComSoft[3]=0X03;
		g_scArrTxComSoft[4]='C';
		g_scArrTxComSoft[5]=0x60;
		g_scArrTxComSoft[6]=0x31;
		g_scArrTxComSoft[7]=ETX;
	   	for (j=0; j<8; j++)
		{
			bcc=g_scArrTxComSoft[j]^bcc;
		}
		g_scArrTxComSoft[8]=bcc;
		buffer_ready=0;																		/* buffer del pto serie (0) inicia a esperar la trama*/
		g_cEstadoComSoft=ESPERA_RX;												/* Espera el ASK en el pt o serie para empesar a almacenas*/
		DebugBufferMF(g_scArrTxComSoft,9,0);								/*muestra la trama enviada al pto serie a debug por tibbo*/
		EscribirCadenaSoft_buffer(g_scArrTxComSoft,9);		/* envio la trama por el pto serie*/
		ValTimeOutCom=TIME_CARD;


}