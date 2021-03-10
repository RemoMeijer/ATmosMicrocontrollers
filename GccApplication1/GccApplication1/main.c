#define F_CPU 8e6

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void wait( int ms ) {
	for (int i=0; i<ms; i++) {
		_delay_ms( 1 );		// library function (max 30 ms at 8MHz)
	}
}

void adcInit( void )
{
	ADMUX = 0b01100000;			
	ADCSRA = 0b11100110;		
}


// Main program: ADC at PF1
int main( void )
{
	DDRF = 0x00;
	DDRA = 0xFF;
	DDRB = 0xFF;
	adcInit();

	while (1)
	{
		PORTB = ADCL;			
		PORTA = ADCH;
		wait(100);				
	}
}