//Jose Sanchez Milestone 1 code
//usart send code no bluetooth


#include <stdio.h>		/* Include std. library file */
#include <util/delay.h>		/* Include Delay header file */
#include <avr/io.h>
#include "usart_ATmega1284.h"
#include "timer.h"
#define F_CPU 8000000UL


int ADC_Value;
int ADC_Value2;
void ADC_Init();
int ADC_Read(char);
void ADC_Init()
{
	DDRA = 0x00;  /* Make ADC port as input */
	ADCSRA = 0x87;  /* Enable ADC, fr/128  */
	ADMUX = 0x40;  /* Vref: Avcc, ADC channel: 0 */
}
int ADC_Read(char channel)
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x07);/* set input channel to read */
	ADCSRA |= (1<<ADSC); /* start conversion */
	while((ADCSRA &(1<<ADIF))== 0); /* monitor end of conversion interrupt flag */
	
	ADCSRA |= (1<<ADIF); /* clear interrupt flag */
	ADC_value = (int)ADCL; /* read lower byte */
	ADC_value = ADC_value + (int)ADCH*256;/* read higher 2 bits, Multiply with weightage */
	return ADC_value;  /* return digital value */
}


enum States {START, ON, OFF} state;

enum States2{START2, ON2, OFF2} state2;


void TickSM2()
{
	switch (state2)
	{
		//transitions
		case START2:
		state = ON2;
		break;
		case ON2:
		state = ON2;
		break;
		default:
		state = START;
		break;
	}
	switch(state2)
	{
		case START2:
		break;
		case ON2:
		if(USART_IsSendReady(0))
		{
			if( (~PINC&0x03) == 0x01)
			{
				USART_Send(0x10,0);
				PORTB = 0xFF;
				USART_Flush(0);
			}
		}
		break;
		default:
		break;
	}
}


void tickSM()
{
	switch (state)
	{//Transitions
		case START:
		state = ON;
		break;
		case ON:
		state = ON;
		break;
		default:
		state = START;
		break;
	}
	switch (state)
	{
		case START:
		break;
		case ON:
		ADC_Value = ADC_Read(0);
		ADC_Value2 = ADC_Read(1);
		
		if(USART_IsSendReady(0))
		{
			if (ADC_Value <400)
			{
				USART_Send(0x05,0);
				PORTB = 0x05;
				USART_Flush(0);
				break;
			}
			if (ADC_Value >1000)
			{
				USART_Send(0x0A,0);
				PORTB = 0x0A;
				USART_Flush(0);
				break;
			}
			if(ADC_Value2 < 400)
			{
				USART_Send(0x06,0);
				PORTB = 0x06;
				USART_Flush(0);
				break;
			}
			if(ADC_Value2 > 1000)
			{
				USART_Send(0x09,0);
				PORTB = 0x09;
				USART_Flush(0);
				break;
			}
			if( (~PINC&0x03) == 0x01)
			{
				USART_Send(0x10,0);
				PORTB = 0x10;
				USART_Flush(0);
				break;
			}
			if( (~PINC&0x03) == 0x02)
			{
				USART_Send(0x20,0);
				PORTB = 0x20;
				USART_Flush(0);
				break;
			}
			PORTB = 0x00;
			USART_Send(0x00, 0);
			USART_Flush(0);
			
			break;
		}
		break;
		default:
		break;
	}
}

int main(void)
{
	initUSART(0);

	ADC_Init();
	PORTC = 0x00; PINC = 0xFF;
	TimerSet(30);
	TimerOn();

	while (1)
	{
		
		
		TickSM2();
		tickSM();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}