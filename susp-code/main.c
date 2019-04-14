/******************************************************************
*Project name:	suspension system
*
*Fuction:
*
*Circuit Connection:	see #define XXXX section
*
*System clock:	12MHz	
*
*Author:	om.w
******************************************************************/


#include <avr/io.h>        
#include <avr/interrupt.h>  
#include "motor.h"
#include "ui.h"

void avr_init(void);

int main(void)            
{
	avr_init();
	sei(); 	
	
	while(1)
	{		
		switch (Mode)
		{			
			case 0:
				ui_mode0();	
				break;
			case 1:
				ui_mode1();	
				break;
			default:	break;
		}
	}
}

void avr_init(void)
{
//	Usart_Init();
	motor_init();	
	lcd_init();
	ui_mode0_init();

}



