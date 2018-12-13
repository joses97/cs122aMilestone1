//Jose Sanchez CS122a milestone 1 code 
//RECIEVER/MOTOR DRIVER CODE WITH USART



#define F_CPU 8000000UL
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart_ATmega1284.h"
#include "timer.h"





enum States { start, recieve } state;
void tickSM()
{
	switch (state) //transitions
	{
		case start:
		state = recieve;
		break;
		case recieve:
		state = recieve;
		break;
		default:
		break;
	}
	switch (state) //actions
	{
		case start:
		break;
		case recieve:
		if ( USART_HasReceived(0) )
		{
			unsigned char tmp;
			tmp = USART_Receive(0);
			if( (tmp == 0x00) | (tmp == 0x05) | (tmp == 0x0A) | (tmp == 0x06) | (tmp == 0x09))
			{
				PORTB = USART_Receive(0);
				USART_Flush(0);
				break;
			}
			if(tmp == 0x10)
			{
				OCR1A = 65;
				break;
			}
			else if(tmp == 0x20)
			{
				OCR1A = 280;
				break;
			}
		}
		break;
	}

}

int main(void)
{
	DDRD |= (1<<PD5);
	TCNT1 = 0;
	ICR1 = 2499;
	
	
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	
	
	initUSART(0);
	TimerSet(5);
	TimerOn();
	unsigned char temp = 0;
	while (1)
	{
		tickSM();
		while(!TimerFlag);
		TimerFlag = 0;
		
	}
	
}