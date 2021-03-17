#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#define F_CPU 8e6

void adcInit( void )
{
	ADMUX = 0b01100001;
	ADCSRA = 0b10000110;
}

void changeADCChannel(int channel){
	ADMUX = 0x60 | channel;
}

int readADC(){
	ADCSRA |= (1 << 6);
	while ( ADCSRA & (1 << 6) );
	return ADCH;
}

void wait(int ms)
{
	for (int i=0; i<ms; i++)
	_delay_ms(1);
}

int main()
{	
	DDRF = 0x00;
	DDRA = 0xFF;
	DDRD = 0xFF;
	DDRE = 0xFF;
	
	adcInit();

	while(1){
		changeADCChannel(0);
		PORTD = readADC();
		changeADCChannel(1);
		PORTE = readADC();
		wait(100);
	}
	return (1);
}