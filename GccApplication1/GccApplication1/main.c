#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LCD_E 	3
#define LCD_RS	2

void lcd_strobe_lcd_e(void);
void init_4bits_mode(void);
void lcd_write_string(char *str);
void lcd_write_data(unsigned char byte);
void lcd_write_cmd(unsigned char byte);

void lcd_strobe_lcd_e(void) {
	PORTC |= (1<<LCD_E);	// E high
	_delay_ms(1);			// nodig
	PORTC &= ~(1<<LCD_E);  	// E low
	_delay_ms(1);			// nodig?
}

void init_4bits_mode(void) {
	// PORTC output mode and all low (also E and RS pin)
	DDRC = 0xFF;
	PORTC = 0x00;

	// Step 2 (table 12)
	PORTC = 0x20;	// function set
	lcd_strobe_lcd_e();

	// Step 3 (table 12)
	PORTC = 0x20;   // function set
	lcd_strobe_lcd_e();
	PORTC = 0x80;
	lcd_strobe_lcd_e();

	// Step 4 (table 12)
	PORTC = 0x00;   // Display on/off control
	lcd_strobe_lcd_e();
	PORTC = 0xF0;
	lcd_strobe_lcd_e();

	// Step 4 (table 12)
	PORTC = 0x00;   // Entry mode set
	lcd_strobe_lcd_e();
	PORTC = 0x60;
	lcd_strobe_lcd_e();

}

void lcd_write_string(char *str) {
	for(;*str; str++){
		lcd_write_data(*str);
	}
}

void lcd_write_data(unsigned char byte) {
	// First nibble.
	PORTC = byte;
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();
}

void lcd_write_command(unsigned char byte)
{
	// First nibble.
	PORTC = byte;
	PORTC &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();
}

char bin_to_char(unsigned char bin){
	char returnChar = '0';
	if(bin < 0b00010000){
		switch(bin){
			case 0b00000001 :
			returnChar = '1';
			break;
			case 0b00000010 :
			returnChar = '2';
			break;
			case 0b00000011 :
			returnChar = '3';
			break;
			case 0b00000100 :
			returnChar = '4';
			break;
			case 0b00000101 :
			returnChar = '5';
			break;
			case 0b00000110 :
			returnChar = '6';
			break;
			case 0b00000111 :
			returnChar = '7';
			break;
			case 0b00001000:
			returnChar = '8';
			break;
			case 0b00001001:
			returnChar = '9';
			break;
			case 0b00001010:
			returnChar = 'A';
			break;
			case 0b00001011:
			returnChar = 'B';
			break;
			case 0b00001100:
			returnChar = 'C';
			break;
			case 0b00001101:
			returnChar = 'D';
			break;
			case 0b00001110:
			returnChar = 'E';
			break;
			case 0b00001111:
			returnChar = 'F';
			break;
		}
	}
	return returnChar;	
}

int main( void ) {
	
	DDRD &= ~(1 << 7);
	DDRB = 0xFF;
	TCCR2 = 0b00000111;

	// Init LCD
	init_4bits_mode();
	
	lcd_write_command(0x01);
	
	// Loop forever
	while (1) {	
		lcd_write_command(0x01);

		
		lcd_write_data(bin_to_char(TCNT2 >> 4));
		lcd_write_data(bin_to_char(TCNT2 & 0b1111));

		PORTB = TCNT2;
		_delay_ms( 250 );
	}
	return 1;
}Q