#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#define F_CPU 8e6

int outpitch = 0;
int wavenum = 0;
int envval = 1;
int waveform;

int pitch = 400;
int lfoval = 0;

static uint16_t waveforms[4] =
{
	0b1100000000000000,
	0b1111111100000000,
	0b1010101010101010,
	0b0000000000000000
};

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

ISR(TIMER1_COMPA_vect)
{
	// take first bit
	int shiftout = waveform & 1;
	// move waveform 1 place to the right
	waveform >>= 1;
	
	// if we removed a 1 with the bit shift put a 1 to the end of the waveform
	// else do nothing and let it stay 0
	if(shiftout)
		waveform |= (1 << 15);
	
	//shiftout &= envval;
	
	if(shiftout)
		PORTE |= (1 << 2);
	else
		PORTE &= ~(1 << 2);
	}

// set new pitch
void update_pitch()
{
	uint16_t newpitch = 200U + ((pitch-lfoval)*4U);
	
	if (newpitch != outpitch)
	{
		outpitch = newpitch;
		if (TCNT1 > outpitch)
		TCNT1 = 0;
		OCR1A = outpitch;
	}
}

int main()
{	
	DDRF = 0x00;
	DDRA = 0xFF;
	DDRD = 0xFF;
	DDRE = 0xFF;
	
	adcInit();
	
	sei();
	
	TCCR1B |= _BV(WGM12);  // enable CTC
	TIMSK |= _BV(OCIE1A); // enable CTC interrupt
	
	wavenum = 3;
	waveform = waveforms[wavenum];
	
	TCCR1B |= _BV(CS11); // set prescaler to 8; starts oscillator
	
	while(1){
		changeADCChannel(0);
		PORTA = readADC();
		changeADCChannel(1);
		PORTD = readADC();
		
		//update_pitch();
		
		wait(100);
	}
	return (1);
}