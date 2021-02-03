#include <avr/io.h>
#include <util/delay.h>

typedef struct {
	unsigned char data;
	unsigned int delay ;
} PATTERN_STRUCT;

PATTERN_STRUCT pattern[] = { 
	{0x00, 100}, {0x01, 100}, {0x02, 100}, {0x04, 100}, {0x10, 100}, {0x20, 100}, {0x40, 100}, {0x80, 100},
	{0x00, 100}, 
	{0xAA,  50}, {0x55,  50}, 
	{0xAA,  50}, {0x55,  50}, 
	{0xAA,  50}, {0x55,  50}, 
	{0x00, 100},
	{0x81, 100}, {0x42, 100}, {0x24, 100}, {0x18, 100}, {0x0F, 200}, {0xF0, 200}, {0x0F, 200}, {0xF0, 200}
};

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
	int index = 0;
		
	while (1)
	{
		PORTD = pattern[index].data;
		wait(pattern[index].delay);
		index++;
		
		if(index == 25){
			index = 0;
		}
	}

	return 1;
}
