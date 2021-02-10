#include <avr/io.h>
#include <util/delay.h>

const unsigned char Numers[] = {
	0b00111111, //0
	0b00000110, //1
	0b01011011, //2
	0b01001111, //3
	0b01100110, //4
	0b01101101, //5
	0b01111101, //6
	0b00000111, //7
	0b01111111, //8
	0b01101111, //9
	0b01110111, //A
	0b01111100, //b
	0b00111001, //C
	0b01011110, //d
	0b01111001, //E
	0b01110001, //F
};

void wait( int ms )
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms( 1 );        // library function (max 30 ms at 8MHz)
	}
}

void display(int number)
{
	if(number > 15){
		PORTE = Numers[14];
		} else {
		PORTE = Numers[number];
	}
}


int main( void )
{
	DDRE = 0b11111111;                    // PORTD all output
	int i = 0;

	while (1)
	{
		display(i);
		wait(1000);
		i = i + 1;
		if(i == 20){
			i = 0;
		}
	}
	return 1;
}