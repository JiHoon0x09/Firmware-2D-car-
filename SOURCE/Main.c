/*
 * =====================================================================
 * NAME         : Main.c
 *
 * Descriptions : Main routine for S3C2450
 *
 * IDE          : GCC-4.1.0
 *
 * Modification
 *	  
 * =====================================================================
 */
#include <stdlib.h>
#include "2450addr.h"
#include "my_lib.h"
#include "option.h"

#include "./Images/ROAD.H"
#include "./Images/CAR1.H"
#include "./Images/CAR2.H"
#include "./Images/CAR3.H"
#include "./Images/CAR4.H"
#include "./Images/CAR5.H"
#include "./Images/CAR6.H"
#include "./Images/CAR7.H"
#include "./Images/CAR8.H"
#include "./Images/CAR9.H"
#include "./Images/CAR10.H"
#include "./Images/CAR11.H"
#include "./Images/MYCAR.H"
#include "./Images/MYROAD.H"
#include "./Images/MOVEROAD.H"
#include "./Images/START.H"
#include "./Images/OVER.H"
#include "./Images/CENTER.H"
#include "./Images/SIDE.H"
/*	
 * 620 : 실습 10-1 Non-Palletized mode Test	
 * 
 * Advanced Course
 * 
 */
/***************************************
 * 
 * Title: Non-Palletized mode Test
 * 
 ***************************************/
#define  NonPal_Fb   ((volatile unsigned short(*)[480]) FRAME_BUFFER)
typedef struct CAR_ {
	unsigned char *name;
	int time;
	int x;
	int y;
	double velocity;
	int flag;
}
CAR;

void Touch_ISR(void) __attribute__ ((interrupt ("IRQ")));
volatile  int ADC_x, ADC_y;
volatile  int Touch_Pressed=0;


void GameInit(void);
void MakeCar(CAR car[], int mode, int time);
int CrashCheck(CAR car[], CAR MyCar);
CAR MycarMove(CAR MyCar);

const int RoadNum = 4;

void Main(void) {
	int i;
	int nowtime =  0;
	CAR car[RoadNum];
	CAR MyCar;
	
	srand(111);
	
	GameInit();
	/*Car Init*/
	for (i=0;i<RoadNum; i++)
					car[i].flag = 0;
	MyCar.name = mycar;
	MyCar.x = 290;
	MyCar.y = 115;
	MyCar.velocity = 0;
	MyCar.flag = 1;
	MyCar.time = 0;
	MakeCar(car, 1, 0);
	
	/* Draw start map */
	Lcd_Draw_BMP(0,0,start);
	
	while(1) {
		if(Touch_Pressed) {
			while(!(Touch_Pressed==0));
			break;
		}
	}
	/* Draw road map & car */
	Lcd_Draw_BMP(0, 0, road);
	Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
	
	
	nowtime = 1;
	while(nowtime) {
		
		MyCar = MycarMove(MyCar);
		
		for (i=0; i<RoadNum; i++) {
			if(car[i].time <= nowtime && car[i].flag == 1) {
				Lcd_Draw_BMP(car[i].x, car[i].y, car[i].name);
				car[i].x += car[i].velocity;
			}
		}
		for (i=0; i<RoadNum; i++) {
			if(car[i].x > 295) {
				Lcd_Draw_BMP(car[i].x-car[i].velocity, car[i].y, myroad);
				car[i].flag = 0;
			}
		}
		
		MakeCar(car, nowtime%2, nowtime);
		
		if(CrashCheck(car, MyCar)) break;
		
		
		if(1000 < nowtime) {
			nowtime -= 1000;
			for (i=0;i<RoadNum;i++) {
				car[i].time -= 1000;
			}
		}
		nowtime++;
	}
	
	Lcd_Draw_BMP(0,0,over);
	
	while(1) {
		if(Touch_Pressed) {
			while(!(Touch_Pressed==0));
			break;
		}
	}
}


void GameInit(void){
	Uart_Init(115200);
	Timer_Init();
	Lcd_Port_Init();
	NonPal_Lcd_Init();
	Touch_Init();
	pISR_ADC = (unsigned int)Touch_ISR;
	rINTSUBMSK &= ~(0x1<<9);
	rINTMSK1 &= ~(0x1<<31);
}
CAR MycarMove(CAR MyCar){
	int i;
	for (i=0;i<5;i++) {
		if(Touch_Pressed) {
			if(135 < ADC_x && ADC_x < 260) {
				if(320 < ADC_y && ADC_y < 500) {
					MyCar.y += 20;
					if(150 < MyCar.y && MyCar.y < 170){
						
						Lcd_Draw_BMP(MyCar.x, MyCar.y-38, center);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
					else if(130 < MyCar.y && MyCar.y < 150){
						
						Lcd_Draw_BMP(MyCar.x, MyCar.y-18, center);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
					else if(210 < MyCar.y && MyCar.y < 230){
						Lcd_Draw_BMP(MyCar.x, MyCar.y-25, side);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
					//4차선에서 3차선 
					else if(60 < MyCar.y && MyCar.y < 80){
						Lcd_Draw_BMP(MyCar.x, MyCar.y-29, side);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
					else{
						Lcd_Draw_BMP(MyCar.x, MyCar.y-20, moveroad);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
				} 
				else {
					MyCar.y -= 20;
					//2차선에서 3차선 넘어갈 때 -1
					if(80 < MyCar.y && MyCar.y < 100){
						Lcd_Draw_BMP(MyCar.x, MyCar.y+22, center);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
					//2차선에서 3차선 넘어갈 때 -2
					else if(60 < MyCar.y && MyCar.y < 80){
						Lcd_Draw_BMP(MyCar.x, MyCar.y+42, center);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
					//1차선에서 2차선 넘어갈 때 
					else if(150< MyCar.y && MyCar.y<170){
						Lcd_Draw_BMP(MyCar.x, MyCar.y+35, side);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);							
					}
					//3차선에서 4차선 넘어갈 때 
					else if(0 < MyCar.y && MyCar.y < 20){
						Lcd_Draw_BMP(MyCar.x, MyCar.y+31, side);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);
					}
					else{
						Lcd_Draw_BMP(MyCar.x, MyCar.y+42, moveroad);
						Lcd_Draw_BMP(MyCar.x, MyCar.y, MyCar.name);	
					}
				}
			}
			while(!(Touch_Pressed==0)) {
				;
			}
		}
	}	
	
	return MyCar;
}
	
void MakeCar(CAR car[], int mode, int time) {
	int i;
	int tempnum;
	switch(mode) {
		case 1:
							for (i=0;i<RoadNum;i++) {
			if(car[i].flag == 0) {
				car[i].flag = 1;
				car[i].velocity = 5+ rand()%7;
				//0.5 ~ 2까지 
				car[i].time = time + rand()%70;
				car[i].x = 0;
				switch(i) {
					case 0: car[i].y = 10;
					break;
					case 1: car[i].y = 80;
					break;
					case 2: car[i].y = 155;
					break;
					case 3: car[i].y = 220;
					break;
					default:		;
				}
				tempnum = rand()%11;
				switch(tempnum) {
					case 0: car[i].name = car1;
					break;
					case 1: car[i].name = car2;
					break;
					case 2: car[i].name = car3;
					break;
					case 3: car[i].name = car4;
					break;
					case 4: car[i].name = car5;
					break;
					case 5: car[i].name = car6;
					break;
					case 6: car[i].name = car7;
					break;
					case 7: car[i].name = car8;
					break;
					case 8: car[i].name = car9;
					break;
					case 9: car[i].name = car10;
					break;
					case 10: car[i].name = car11;
					break;
					default:		;
				}
			}
		}
		break;
		default :
						for (i=RoadNum;i>-1;i--) {
			if(car[i].flag == 0) {
				car[i].flag = 1;
				car[i].velocity = 5+ rand()%7;
				//0.5 ~ 2까지 
				car[i].time = time + rand()% 70;
				car[i].x = 0;
				switch(i) {
					case 0: car[i].y = 10;
					break;
					case 1: car[i].y = 80;
					break;
					case 2: car[i].y = 155;
					break;
					case 3: car[i].y = 220;
					break;
					default:		;
				}
				tempnum = rand()%11;
				switch(tempnum) {
					case 0: car[i].name = car1;
					break;
					case 1: car[i].name = car2;
					break;
					case 2: car[i].name = car3;
					break;
					case 3: car[i].name = car4;
					break;
					case 4: car[i].name = car5;
					break;
					case 5: car[i].name = car6;
					break;
					case 6: car[i].name = car7;
					break;
					case 7: car[i].name = car8;
					break;
					case 8: car[i].name = car9;
					break;
					case 9: car[i].name = car10;
					break;
					case 10: car[i].name = car11;
					break;
					default:		;
				}
			}
		}
		
	}
}

int CrashCheck(CAR car[], CAR MyCar){
	int flag;
	int i;
	
	flag = 0;
	for(i=0; i<RoadNum; i++){
		if(MyCar.x < car[i].x + 80){
			if(MyCar.y < (car[i].y+40) && (car[i].y)<MyCar.y+40){
				flag = 1;
				break;
			}
		}
	}
	
	switch(flag){
		case 0 : return 0; break;
		case 1 : return 1; break;
		default: ;
	}
}


void Touch_ISR() {
	/* 인터럽트 허용하지 않음 on Touch */
	rINTSUBMSK |= (0x1<<9);
	rINTMSK1 |= (0x1<<31);
	/* TO DO: Pendng Clear on Touch */
	rSUBSRCPND |= (0x1<<9);
	rSRCPND1 |= (0x1<<31);
	rINTPND1 |= (0x1<<31);
	if(rADCTSC & 0x100) {
		rADCTSC &= (0xff);
		Touch_Pressed = 0;
	} else {
		/* TO DO : Stylus Down, YM_out Enable, YP_out Disable, XM_out Disable, XP_out disable
		 * 		   XP Pull-up Disable, Auto Sequential measurement of X/Y, No operation mode */
		rADCTSC =(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);
		/* TO DO : ENABLE_START */
		rADCCON |=(1);
		/* wait until End of A/D Conversion */
		while(!(rADCCON & (1<<15)));
		/*store X-Position & Y-Position Conversion data value to ADC_x, ADC_y */
		ADC_x = (rADCDAT0 & 0x3ff);
		ADC_y = (rADCDAT1 & 0x3ff);
		Touch_Pressed = 1;
		/* TO DO : change to Waiting for interrupt mode 
		 *		   Stylus Up, YM_out Enable, YP_out Disable, XM_out Disable, XP_out disable
		 * 		   XP Pull-up Disable, Normal ADC conversion, Waiting for interrupt mode */
		rADCTSC =(1<<8)|(1<<7)|(1<<6)|(1<<4)|(0<<3)|(0<<2)|(3);
	}
	/* 인터럽트 다시 허용  on Touch */
	rINTSUBMSK &= ~(0x1<<9);
	rINTMSK1 &= ~(0x1<<31);
}

