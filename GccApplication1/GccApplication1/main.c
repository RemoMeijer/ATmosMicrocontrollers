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
	DDRD = 0b11111111;					// PORTD all output 
	int delay = 1000;
	int changed = 0;
	
	while (1)
	{
		if(PINC & 0x1){
			if(changed == 0){
				changed = 1;
				delay = delay == 1000? 250 : 1000;
			}
		} else {
			changed = 0;
		}
		
		PORTD = 0xFF;
		wait(delay);
		PORTD = 0x0;
		wait(delay);
	}
	return 1;
}
