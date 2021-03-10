#define F_CPU 8e6

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


// Build in led Arduino on PB7 (pin 13)
void wait( int ms ) {
	for (int i=0; i<ms; i++) {
		_delay_ms( 1 );		// library function (max 30 ms at 8MHz)
	}
}

void adcInit( void )
{
	ADMUX = 0b11100011;
	ADCSRA = 0b10000110;
}

int main(void) {
	DDRF = 0x00;
	DDRA = 0xFF;
	DDRB = 0xFF;
	adcInit();
	
	while (1)  {
		ADCSRA |= (1 << 6);
		while ( ADCSRA & (1 << 6) ) ;
		PORTA = ADCH;
		wait(100);
	}
	return 0;
}