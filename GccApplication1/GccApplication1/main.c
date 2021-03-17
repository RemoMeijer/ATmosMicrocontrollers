#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#define F_CPU 8e6

void wait(int ms)
{
	for (int i=0; i<ms; i++)
	_delay_ms(1);
}

int main()
{	
	while(1){
		wait(1000);
	}

	return (1);
}