
#include <eeprom.h>
#include <reg51.h>
#include <string.h>

/*funciones externas*/
extern void          _nop_     (void);
extern void Delay (void);
extern void Delay_20us(unsigned int cnt);
extern void Delay_1ms(unsigned int cnt);
extern long   atol (const char *s1);
extern void Debug_txt_Tibbo(unsigned char * str);
extern int sprintf  (char *, const char *, ...);
extern void Debug_chr_Tibbo(unsigned char Dat);

//******************************************************************************************
// 		RUTINAS DE EEPROM 24FC1025
//******************************************************************************************
sbit sda = P1^2;
sbit scl = P1^3;
//******************************************************************************************

bit memory_flag;
unsigned char byte_rd;
unsigned char l_data_eeprom;
unsigned char l_chr;

/*define posiciones de memoria*/
#define EE_ID_CLIENTE		0x0000

#define EE_TICKET_ID					0X0200


//*******************************************************************************************
void ackd(void)
{
	unsigned int i;
	memory_flag=1;
	scl=1;
	sda=1;
	for (i=0; i<1100; i++)					   //500
	{
	  if (sda==0)
	  {
	  	memory_flag=0;
		scl=0;

		break;
	  }
	}
	
	if (memory_flag==1)
	{ 

	}  
}	
//*******************************************************************************************
//*******************************************************************************************
void ack_lect(void)
{
	int i;
	scl=1;
	sda=1;
	memory_flag=1;
	for (i=0; i<650; i++)
	{
	  if (sda==0)
	  {
	  	memory_flag=0;
		scl=0;
		break;
	  }
	}
}	
//*******************************************************************************************
void p_me (void)
{
	  scl=1;
	  _nop_();
	  scl=0;
}
//*******************************************************************************************
void e_d_m (unsigned char a_serial_eeprom)
{
	unsigned char nbits;
  	for (nbits=0; nbits<8; nbits++)
	{
	  a_serial_eeprom <<=1;
	  if (CY==1)
	  { 
	    sda=1;
	  }
	  else
	  {
	    sda=0;
	  }
	  scl=1;
	  p_me();  				
	}
	return;
}
//********************************************************************************************
void start (void)
{
 	sda=1;
 	scl=1;
 	_nop_();
 	sda=0;
 	scl=0;
 	return;
}
//*******************************************************************************************
void stop (void)
{
 	scl=0;
 	sda=0;
 	scl=1;
 	sda=1;
 	return;
}
//*******************************************************************************************
unsigned char l_d_m (void)
{
	unsigned char nbits;
   	for (nbits=0; nbits<8; nbits++)
	{
		scl=0;
   		if (sda==1) 
	  	{
			l_data_eeprom = l_data_eeprom| 0x01;
			if (nbits<=6) 
			{
				l_data_eeprom<<=1;
				scl=1;
			}
	  	}
	   	if (sda==0)
	  	{
	  		l_data_eeprom = l_data_eeprom & 0xFE;
			if (nbits<=6) 
		 	{
		  		l_data_eeprom <<=1;
				scl=1;
		 	}
 	  	}
    }
	scl=0;
 	return l_data_eeprom;
}
//*******************************************************************************************
//*******************************************************************************************
//	ESCRIBE EN EEPROM																		*
//*******************************************************************************************
//void wr_eeprom (unsigned char control,unsigned char dir_h,unsigned char dir_l,unsigned char data_eeprom)
void wr_eeprom (unsigned char control,unsigned int Dir, unsigned char data_eeprom)
{
	unsigned char dir_h, dir_l;
	dir_l=Dir;
	Dir>>=8;
	dir_h=Dir;


 	scl=0;
	sda=0;
// 	wait();
    start();
	e_d_m(control);
	ackd();
	e_d_m(dir_h);
	ackd();
	e_d_m(dir_l);
	ackd();
	e_d_m(data_eeprom);
	ackd();
	stop();
	Delay_1ms(13);
	
	scl=1;
	sda=1;
	Delay_20us(98);										/*wait long*/
	Delay_20us(98);
}
//*******************************************************************************************
//	ESCRIBE EN EEPROM																		*
//*******************************************************************************************
/*
void wrpage_eeprom (unsigned char control,unsigned char dir_h,unsigned char dir_l,unsigned char data_eeprom)  
{
 	unsigned int j;
	scl=0;
	sda=0;
 	wait();

	wait();

    start();
	e_d_m(control);
	ack();
	e_d_m(dir_h);
	ack();
	e_d_m(dir_l);
	ack();
 	for (j=0; j<=127; j++)
	{
		e_d_m(data_eeprom);
		ack();
	}
	stop();
	Delay_1ms(13);
	scl=1;
	sda=1;
 return;
}																						   */
//*******************************************************************************************	
//*******************************************************************************************	
//	LEE EN EEPROM																			*
//*******************************************************************************************
//unsigned char rd_eeprom (unsigned char control,unsigned char dir_h,unsigned char dir_l) 
unsigned char rd_eeprom (unsigned char control,unsigned int Dir) 
{

	unsigned char dir_h, dir_l;
	dir_l=Dir;
	Dir>>=8;
	dir_h=Dir;

 	scl=0;
	sda=0;
 //	wait();
 	start();
	e_d_m(control);
	ack_lect();
	e_d_m(dir_h);
  	ack_lect();
	e_d_m(dir_l);
 	ack_lect();
	start();
	e_d_m(control+0x01);
 	ackd();
	scl=0;
 	l_d_m();
	stop();
  	scl=1;
	sda=1;
	return l_data_eeprom;
}
/*----------------------------------------------------------------------------------
Funcion q recibe el numero de ticket en un arreglo
error=1 valida los 10 digitos del ticket y si no es numerico los escribe en cero
j= proposito general
Noticket= variable de 32 bits tiene el numero del ticket
-----------------------------------------------------------------------------------*/
void graba_serie(unsigned char *buffer)
{
	
	unsigned char  j;
	unsigned char error=0;
	
	
/*valido q los datos recibidos sean numericos*/
	
	*(buffer+10)=0;
	
	for (j=0; j<=9; j++)
	{
		if	((*(buffer+j)<0x30)||(*(buffer+j)>0x39))
		{
			error=1;
			
			Debug_txt_Tibbo((unsigned char *) "Error de ticket");
			Debug_txt_Tibbo((unsigned char *) buffer);
			Debug_txt_Tibbo((unsigned char *) "\n\r");
		}
	
	}
	
	
	/* son numericos*/
		if (error==0)
		{
			Write_EEprom_Ticket(buffer);
				
		}
		else
		{
			wr_eeprom(0xa8,EE_TICKET_ID,00);
			wr_eeprom(0xa8,EE_TICKET_ID+1,00);
			wr_eeprom(0xa8,EE_TICKET_ID+2,00);
			wr_eeprom(0xa8,EE_TICKET_ID+3,00);	
			
		}
}
void Write_EEprom_Ticket(unsigned char *buffer)
{
	unsigned char  cod_3,cod_2,cod_1,cod_0;
	unsigned long int Noticket,Bnoticket=0;
	Debug_txt_Tibbo((unsigned char *) "Numero de ticket:");
	Debug_txt_Tibbo((unsigned char *) buffer);
	Debug_txt_Tibbo((unsigned char *) "\n\r");
	Noticket= atol(buffer);
		
	Debug_txt_Tibbo((unsigned char *) "No de ticket HEX:");
	Bnoticket=Noticket>>24;
	cod_3=Bnoticket;
	Debug_chr_Tibbo(cod_3);
		
	Bnoticket=Noticket >>16;
	cod_2=Bnoticket;
	Debug_chr_Tibbo(cod_2);
			
	Bnoticket=Noticket >>8;
	cod_1=Bnoticket;
	Debug_chr_Tibbo(cod_1);
			
	cod_0=Noticket;
	Debug_chr_Tibbo(cod_0);
	Debug_txt_Tibbo((unsigned char *) "\n\r");
			
	wr_eeprom(0xa8,EE_TICKET_ID,cod_3);
	wr_eeprom(0xa8,EE_TICKET_ID+1,cod_2);
	wr_eeprom(0xa8,EE_TICKET_ID+2,cod_1);
	wr_eeprom(0xa8,EE_TICKET_ID+3,cod_0);	
}
/*----------------------------------------------------------------------------------
Procedimiento que lee el No de Ticket en EEprom y lo retorna en un strint
-----------------------------------------------------------------------------------*/
unsigned char *Lee_No_Ticket()
{
	unsigned long int Noticket;
	unsigned char Lee_No_Ticket[11];
	unsigned char num_chr;
	Noticket=Read_EEprom_Ticket();
	sprintf( Lee_No_Ticket,"%lu",Noticket);
	num_chr=strlen(Lee_No_Ticket);
	Lee_No_Ticket[num_chr] = 0;
	Debug_txt_Tibbo((unsigned char *) Lee_No_Ticket);
	Debug_txt_Tibbo((unsigned char *) "\n\r");
	return Lee_No_Ticket;
}
/*----------------------------------------------------------------------------------
Procedimiento que lee el No de Ticket en EEprom y lo retorna en un long int
-----------------------------------------------------------------------------------*/
unsigned long int  Read_EEprom_Ticket()
{
	unsigned char cod3,cod2,cod1,cod0;
	unsigned long int Noticket=0;
	cod3=rd_eeprom(0xa8,EE_TICKET_ID);
	cod2=rd_eeprom(0xa8,EE_TICKET_ID+1);
	cod1=rd_eeprom(0xa8,EE_TICKET_ID+2);
	cod0=rd_eeprom(0xa8,EE_TICKET_ID+3);
	
	Noticket=((Noticket| cod3)<<8);
	Noticket=((Noticket| cod2)<<8);
	Noticket=((Noticket| cod1)<<8);
	Noticket=Noticket| cod0;
	return Noticket;
}
/*----------------------------------------------------------------------------------
Procedimiento que incrementa en uno el numero del Ticket y la graba en  EEprom 
-----------------------------------------------------------------------------------*/
void Incremente_Ticket()
{
	
	unsigned long int Noticket;
	unsigned char Lee_No_Ticket[11];
	Noticket=Read_EEprom_Ticket();
	Noticket=Noticket+1;
	sprintf( Lee_No_Ticket,"%lu",Noticket);
	Write_EEprom_Ticket(Lee_No_Ticket);
}

//***********************************************************************************************
//Rutina que lee la eeprom 
//***********************************************************************************************
void LeerMemoria(unsigned int addres, unsigned char *res)
	{
unsigned char i;
do {
 	*res=rd_eeprom(0xa8,addres);;
	i=*res;
	addres++;
	res++;
}while(i !='\0');
 	*res='\0';
	}
void EscribirMemoria(unsigned int addres,unsigned char *res)
	{


while(*res !='\0'){
	if(*res =='\r'){*res='\0';}  
	wr_eeprom(0xa8,addres,*res);
 
    addres++;
		res++;
	}
 wr_eeprom(0xa8,addres,*res);
	 addres++;
		res++;
  wr_eeprom(0xa8,addres,0);

}