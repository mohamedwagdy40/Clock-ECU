/*
 * MINI_PROJECT1.C
 *
 *  Created on: Aug 24, 2020
 *      Author: medo_
 */



#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char sec1 = 0;
unsigned char sec2 = 0;
unsigned char min1 = 0;
unsigned char min2 = 0;
unsigned char hours1 = 0;
unsigned char hours2 = 0;

void Timer1_Init(void)
{
	TCCR1A = (1<<WGM12) ;        //for use the timer 1 in 16-bit mode
	TCCR1B = (1<<CS12) | (1<<CS10);             //pre-scaler =1024
	OCR1A = 977;                             //compare value to make over flow every 1 second
	TIMSK |= (1<<OCIE1A);                    //enable capture compare A interrupt
	SREG |= (1<<7);                          //enable global interrupt
}

void INT0_Init(void)
{
	DDRD &= ~(1<<PD2);                       //config PD2 as input
	PORTD |= (1<<PD2);                      //internal pull up
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);                   //The falling edge of INT0 generates an interrupt request.
	GICR |= (1<<INT0);                      //External Interrupt Request 0 Enable
	//SREG |= (1<<7);						//this bit is already set by timer 1 initialize
}

void INT1_Init(void)
{
	DDRD &= ~(1<<PD3);                       //config PD3 as input
	MCUCR |= (1<<ISC10) | (1<<ISC11);       //The rising edge of INT1 generates an interrupt request.
	GICR |= (1<<INT1);                      //External Interrupt Request 1 Enable
	//SREG |= (1<<7);						//this bit is already set by timer 1 initialize
}

void INT2_Init(void)
{
	DDRB &= ~(1<<PB2);                       //config PB2 as input
	PORTB |= (1<<PB2);                       //internal pull up
	GICR |= (1<<INT2);                       //External Interrupt Request 2 Enable
	//SREG |= (1<<7);						//this bit is already set by timer 1 initialize
}

/*****************************************ISR Functions**********************************************************/

ISR(TIMER1_COMPA_vect)
{
	sec1 ++;
	TCNT1 = 0;
}

ISR(INT0_vect)
{
	sec1=sec2=min1=min2=hours1=hours2=0;                           //reset the stop watch
}

ISR(INT1_vect)
{
	TCCR1B &= ~(1<<CS12) ;             //disconnect timer frequency
	TCCR1B &= ~(1<<CS10);
}

ISR(INT2_vect)
{
	TCCR1B = (1<<CS12) | (1<<CS10);             //reconnect timer frequency
}

int main ()
{
	Timer1_Init();
	INT0_Init();
	INT1_Init();
	INT2_Init();

	DDRC = 0x0f;                             //config first 4 pins in port c as output
	DDRA = 0x3f;                             //config first 6 pins in port a as output
	PORTA = 0;


	while(1)
	{
		if(sec1 == 10)
		{
			sec1 = 0;
			sec2 ++;
		}
		if(sec2 == 6)
		{
			sec2 = 0;
			min1 ++;
		}
		if(min1 == 10)
		{
			min1 = 0;
			min2 ++;
		}
		if(min2 == 6)
		{
			min2 = 0;
			hours1 ++;
		}
		if(hours1 == 10)
		{
			hours1 = 0;
			hours2 ++;
		}
		if(hours2 == 2 && hours1 == 4)
		{
			hours2 = 0;
			hours1 = 0;
		}


		PORTA = (1<<5);
		PORTC = ( (PORTC&(0xf0)) | (sec1&(0x0f)) );
		_delay_ms(2);

		PORTA = (1<<4);
		PORTC = ( (PORTC&(0xf0)) | (sec2&(0x0f)) );
		_delay_ms(2);

		PORTA = (1<<3);
		PORTC = ( (PORTC&(0xf0)) | (min1&(0x0f)) );
		_delay_ms(2);

		PORTA = (1<<2);
		PORTC = ( (PORTC&(0xf0)) | (min2&(0x0f)) );
		_delay_ms(2);

		PORTA = (1<<1);
		PORTC = ( (PORTC&(0xf0)) | (hours1&(0x0f)) );
		_delay_ms(2);

		PORTA = (1<<0);
		PORTC = ( (PORTC&(0xf0)) | (hours2&(0x0f)) );
		_delay_ms(2);
	}
}

