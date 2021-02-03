#include <avr/io.h>
#include <util/delay.h>


void wait( int ms )
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms( 1 );		// library function (max 30 ms at 8MHz)
	}
}

int main( void )
{
	
	DDRD = 0b11110111;			// All pins PORTD are set to output 
	
	while (1)
	{
		while(PINC & 0x01){		
			PORTD = 0x01;
			wait(250);
			PORTD = 0x0;
			wait(250);
		}
		PORTD = 0x0;
	}

	return 1;
}
