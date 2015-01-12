// функции для работы с дисплеем от телефона Siemens A70 (LCD модуль с маркировкой HP10534)
//


#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


#include "font.h"


// перед инициализацией LCD соответствующие пины необходимо настроить на выход и установить
// на них низкий логический уровень
//
#define DAT_H  PORTC|=(1<<PIN1)  // mosi
#define DAT_L  PORTC&=~(1<<PIN1)

#define SCK_H  PORTD|=(1<<PIN3)  // clock
#define SCK_L  PORTD&=~(1<<PIN3)

#define DC_H   PORTD|=(1<<PIN4)  // data/command
#define DC_L   PORTD&=~(1<<PIN4)

#define RST_H  PORTD|=(1<<PIN5)  // reset
#define RST_L  PORTD&=~(1<<PIN5)

#define CS_H   PORTC|=(1<<PIN2)  // chip select
#define CS_L   PORTC&=~(1<<PIN2)


#define SET_BIT(reg, bit) reg|=(1<<bit)
#define CLEAR_BIT(reg, bit) reg&=(~(1<<bit))


void lcd_write(uint8_t mode, uint8_t data);
void lcd_data(uint8_t data);
void lcd_command(uint8_t command);
void lcd_init(void);
void lcd_goto(uint8_t line, uint8_t row);
void lcd_clear(void);
void lcd_char(uint8_t sign, uint8_t v, uint8_t h);
void lcd_string(uint8_t v, uint8_t h, char *str);


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_write(uint8_t mode, uint8_t data)
{
uint8_t s=0x80;

DAT_L;
SCK_L;
if(mode) DC_L; //command
else DC_H; //data
CS_L;

if(mode) _delay_us(10);

for(uint8_t i=0; i<8; i++)
	{
	if(data & s) DAT_H;
	else DAT_L;
	s = s>>1;
	SCK_H;
	SCK_L;
	}
CS_H;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_command(uint8_t command)
{
lcd_write(1,command);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_data(uint8_t data)
{
lcd_write(0,data);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_init(void)
{
RST_L;
_delay_us(10);
RST_H;

lcd_command(0x21); // Function set: extended instruction set
lcd_command(0x14); // Bias System
lcd_command(0x0A); // HV-gen stages
lcd_command(0x05); // Temperature Control
lcd_command(0xCC); // Contrast
lcd_command(0x20); // Function set: standard instruction set
lcd_command(0x11); // VLCD programming range: high
lcd_command(0x0C); // Display control: normal (inverted = 0x0D)
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_goto(uint8_t line, uint8_t row)  //линия (строка) 0..7 //ряд (пикселей) 0..100
{
lcd_command(0x28);
lcd_command(0x40+line);
lcd_command(0x80+row);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_clear(void)
{
lcd_goto(0,0);
for(uint16_t i=0; i<((101*64)/8+8); i++) lcd_data(0x00);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_char(uint8_t sign, uint8_t v, uint8_t h)
{
if((v<9) && (h<13))
	{
	lcd_goto(v-1, 100-h*8);
	if(sign<0x20) sign=0x20;
	int16_t pos = 8*(sign-0x20);
	for(uint8_t y=0; y<8; y++) lcd_data(pgm_read_byte(&font8x8[pos++]));
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void lcd_string(uint8_t v, uint8_t h, char *str) //выводим строку //макс длина 12 символов
{
for(uint8_t c=0; ((c<12) && (*(str+c) != '\0')); c++) lcd_char(*(str+c),v,h+c);
}
