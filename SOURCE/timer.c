#include "2450addr.h"
#include "option.h"
#include "my_lib.h"

void Timer_Init(void)
{
	/* 
	* 	Timer0 Init 
	* Prescaler value : 255, dead zone length = 0
	* Divider value : 1/16, no DMA mode
	* New frequency : (PCLK/(Prescaler value+1))*Divider value = (66Mhz/(256))*(1/16)
	*				= 16.113Khz(16113Hz)
	*/
	/* YOUR CODE HERE */  
	rTCFG0 = (0<<8)|(0xff); 
	rTCFG1 = (0<<20)|(3); 
	
	/* TCON설정 :Dead zone disable,  auto reload on, output inverter off
	*  manual update no operation, timer0 stop, TCNTB0=0, TCMPB0 =0
	*/
	/* YOUR CODE HERE */  
	rTCON  = (0<<4)|(1<<3)|(0<<2)|(0<<1)|(0);
	rTCNTB0 = 0;
	rTCMPB0 = 0;
 
}

void Timer_Delay(int msec)
{
	/*
	* 1) TCNTB0설정 : 넘겨받는 data의 단위는 msec이다.
	*                  따라서 msec가 그대로 TCNTB0값으로 설정될 수는 없다.
	* 2) manual update후에  timer0를 start시킨다. 
	* 	 note : The bit has to be cleared at next writing.
	* 3) TCNTO0값이 0이 될때까지 기다린다. 	
	*/
	/* YOUR CODE HERE */  
	rTCNTB0 = 16.113*msec;	

	rTCON |= (1<<1)|(0);
	rTCON &= ~(1<<1);
	
	rTCON |= 1;	
	
	while(rTCNTO0);
	
}