#define F_CPU 8e6

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#define NUM_WAVEFORMS 8

int outpitch = 0;
int wavenum = 0;
int envval = 1;
uint32_t waveform;

int pitch = 400;

uint16_t lfofreq = 0; 
uint16_t lfodepth = 0; 
uint8_t lfowavenum = 0;
uint8_t lfotimer = 0;  
uint16_t lfoval = 0;   
uint16_t lfodelta = 0;

static uint32_t waveforms[NUM_WAVEFORMS] =
{
	0b10101010101010101010101010101010,
	0b11101001000100010000100000100000,
	0b00000100000100001000100010010111,
	0b11111111000000001111111100000000,
	0b00101000011101100010100001110110,
	0b11110111110111101111011111011110,
	0b00000000000000000000000000000000, // random noise
	0b00000000000000000000000000000000
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

void changeWaveform(){
	wavenum++;
	wavenum = wavenum > NUM_WAVEFORMS - 1? 0 : wavenum;
	wavenum = wavenum < 0? NUM_WAVEFORMS - 1 : wavenum;
	waveform = waveforms[wavenum];
	
	PORTA = wavenum;
}

void changelfoWaveform(){
	lfowavenum++;
	lfowavenum = lfowavenum > 7? 0 : lfowavenum;
	lfowavenum = lfowavenum < 0? 7 : lfowavenum;
	
	PORTB = lfowavenum;
}

// faster noise generator than rand()
uint16_t noise()
{
	static uint16_t lfsr = 0xACE1u;
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);
	return lfsr;
}

ISR(TIMER1_COMPA_vect)
{
	uint8_t shiftout;
	
	if(wavenum != NUM_WAVEFORMS - 2){
		// take first bit
		shiftout = waveform & (1<<0);
		// move waveform 1 place to the right
		waveform >>= 1;
	} else {
		shiftout = noise() & 1;
	}
	
	// if we removed a 1 with the bit shift put a 1 to the end of the waveform
	// else do nothing and let it stay 0
	waveform &= ~(0b10000000000000000000000000000000);
	if(shiftout){
		waveform |= 0b10000000000000000000000000000000;
	}

	
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

void setupTimer(){
	OCR1A = 500;
	TIMSK |= (1 << 4);
	sei();
	TCCR1A = 0b0;
	TCCR1B = 0b1010;
}

void update_lfo()
{
	switch (lfowavenum)
	{
		case 0: // triangle
		lfoval = (lfotimer*lfodelta) >> 8;
		if (lfotimer >= lfofreq/2)
			lfoval = lfodepth - lfoval;
		break;
		case 1: // sawtooth up
		lfoval = (lfotimer*lfodelta) >> 8;
		break;
		case 2: // sawtooth down
		lfoval = lfodepth - ((lfotimer*lfodelta) >> 8);
		break;
		case 3: // square
		lfoval = (lfotimer >= lfofreq/2) ? lfodepth : 0;
		break;
		case 4: // half square
		lfoval = (lfotimer < lfofreq/4) ? lfodepth : 0;
		break;
		case 5: // half sawtooth up
		if (lfotimer < lfofreq/2)
			lfoval = (lfotimer*lfodelta) >> 7;
		else
			lfoval = 0;
		break;
		case 6: // half sawtooth down
		if (lfotimer < lfofreq/2)
			lfoval = lfodepth - ((lfotimer*lfodelta) >> 7);
		else
		lfoval = 0;
		break;
		case 7: // random
		if (lfotimer == 0)
			lfoval = noise() % lfodepth;
		break;
	}

	if (lfotimer < lfofreq/2)
		PORTD |= 1;
	else
		PORTD &= ~(1);

	lfotimer++;
	if (lfotimer >= lfofreq)
		lfotimer = 0;
}

void update_synth_params(){
	changeADCChannel(1);
	pitch = readADC();
	changeADCChannel(2);
	lfodepth = readADC() >> 2;
	changeADCChannel(3);
	lfofreq = readADC() >> 2;
	lfodelta = (lfofreq) ? (lfodepth*256U) / lfofreq : 0;
}

int main()
{	
	DDRF = 0x111;
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRD = 0x80;
	DDRE = 0b00000111;
	DDRG = 0x00;
	adcInit();
	int pressedE7 = 0;
	int pressedE6 = 0;
	wavenum = 0;
	lfowavenum = 0;
	waveform = waveforms[wavenum];
	
	setupTimer();
	
	while(1){
		update_synth_params();
		
		update_pitch();
		update_lfo();
		
		if(PINE & 0x80){
			if(pressedE7 == 0){
				changeWaveform();
				pressedE7 = 1;
			}
		} else { 
			pressedE7 = 0;
		}
		
		if(PINE & 0x40){
			if(pressedE6 == 0){
				changelfoWaveform();
				pressedE6 = 1;
			}
		} else {
			pressedE6 = 0;
		}
				
		_delay_us(100);
	}
	return (1);
}