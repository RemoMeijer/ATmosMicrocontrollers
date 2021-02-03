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
	
	DDRD = 0b11111111;			// All pins PORTD are set to output 
	PORTD = 0x01;
	int reverse = 0;
	while (1)
	{
		wait(500);
		if(PORTD == 0x80) {
			reverse = 1;
		} else if (PORTD == 0x01) {
			reverse = 0;
		}
		
		if(reverse) {
			PORTD = PORTD >> 1;
		} else {
			PORTD = PORTD << 1;
		}
	}

	return 1;
}
