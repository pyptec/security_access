/**********************************************
Diseñado por Ing. Jaime Pedraza  Marzo 4 de 2019
**********************************************/
/*
*
@file		delay.c
@brief	delay functions 
*
*/

#include "delay.h"

/*****************************************************************************************
	Function name: Delay_1us
	Input		:	cntd_us; Delay duration = cntd_us * 1u seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*1u second.
*****************************************************************************************/
void Delay_20us(unsigned int cntd_us)
{
	unsigned int i;
	
	for(i = 0; i<cntd_us; i++) {

		}
}


/*****************************************************************************************
	Function name: Delay_1ms
	Input		:	cntd_ms; Delay duration = cntd_ms * 1m seconds
	Output	:	non
	Description
	: A delay function for waiting cntd_ms*1m second. This function use wait_1us but the wait_1us
		has some error (not accurate). So if you want exact time delay, please use the Timer.
*****************************************************************************************/
void Delay_1ms(unsigned int cntd_ms)
{
	unsigned int i;
	for (i = 0; i < cntd_ms; i++){ Delay_20us(150);}
}

/*****************************************************************************************
	Function name: Delay_10ms
	Input		:	cntd_10ms; Delay duration = cntd_10ms * 10m seconds
	Output	:	non
	Description
	: A delay function for waiting cntd_10ms*10m second. This function use wait_1ms but the wait_1ms
		has some error (not accurate more than wait_1us). So if you want exact time delay, 
		please use the Timer.
*****************************************************************************************/

void Delay_10ms(unsigned int cntd_10ms)
{
	unsigned int i;
	for (i = 0; i <cntd_10ms; i++) Delay_1ms(10);
}

void Delay (void)  {                   			/* wait function 						*/
  ;                                   			
}

