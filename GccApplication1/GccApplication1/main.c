#define F_CPU 16e6

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>


int outpitch = 0;
int wavenum = 0;
int envval = 1;
int waveform;

int pitch = 400;
int lfoval = 0;

static uint16_t waveforms[4] =
{
	0b1111011111011110,
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
	uint8_t shiftout;
	shiftout = waveform & (1<<0);
	// move waveform 1 place to the right
	waveform >>= 1;
		
	// if we removed a 1 with the bit shift put a 1 to the end of the waveform
	// else do nothing and let it stay 0
	waveform &= ~(1 << 15);
	if(shiftout)
		waveform |= (1 << 15);

	PORTA = waveform;
	PORTB = waveform >> 8;
	
	//shiftout &= envval;
	
	if(shiftout)
		DDRF = 0x0;
		//PORTE |= (1 << 2);
	else
		DDRF = 0x1;
		//PORTE &= ~(1 << 2);
	
}

// set new pitch
void update_pitch()
{
	uint16_t newpitch = 1U + (floor((pitch-lfoval)*1U)/10);
	
	if (newpitch != outpitch)
	{
		outpitch = newpitch;
		if (TCNT1 > outpitch)
		TCNT1 = 0;
		OCR1A = outpitch;
	}
}

void setupTimer(){
	OCR1A = 500;
	TIMSK |= (1 << 4);
	sei();
	TCCR1A = 0b0;
	TCCR1B = 0b1100;
	
}

int main()
{	
	DDRF = 0x01;
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRD = 0x0;
	//DDRD = 0xFF;
	DDRE = 0b01000110;
	DDRG = 0x00;
	adcInit();
	int pressed = 0;
	wavenum = 2;
	waveform = waveforms[wavenum];
	
	setupTimer();
	
	while(1){
		//changeADCChannel(0);
		//PORTA = readADC();
		changeADCChannel(1);
		pitch = readADC();
		
		update_pitch();
		
		if(PINE & 0x80){
			if(pressed == 0){
				//PORTE ^= (1<<6);
				wavenum++;
				wavenum = wavenum > 3? 0 : wavenum;
				wavenum = wavenum < 0? 3 : wavenum;
				waveform = waveforms[wavenum];
				pressed = 1;
			}
		} else { 
			pressed = 0;
		}
		
		wait(100);
	}
	return (1);
}