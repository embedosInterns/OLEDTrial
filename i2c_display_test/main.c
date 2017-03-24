/*
* i2c_display_test.c
*
* Created: 24-01-2017 12.15.18 PM
* Author : Bhavin
*/

#include <avr/io.h>
#include <util/delay.h>
#include "ssd1306.h"
#include "font.h"

uint8_t display_buffer[SSD1306_BUFFERSIZE];

void display_draw_pixel(uint8_t pos_x, uint8_t pos_y, uint8_t pixel_status)
{
	if (pos_x >= SSD1306_WIDTH || pos_y >= SSD1306_HEIGHT)
	{
		return;
	}

	if (pixel_status)
	{
		display_buffer[pos_x+(pos_y/8)*SSD1306_WIDTH] |= (1 << (pos_y & 7));
	}
	else
	{
		display_buffer[pos_x+(pos_y/8)*SSD1306_WIDTH] &= ~(1 << (pos_y & 7));
	}
}

void display_draw_VLine(uint8_t x, uint8_t y, uint8_t length,uint8_t invert)
{
	for (uint8_t i = 0; i < length; ++i)
	{
		display_draw_pixel(x, i+y, invert ^ 1);
	}
}

void display_draw_HLine(uint8_t x, uint8_t y, uint8_t length, uint8_t invert)
{
	for (uint8_t i = 0; i < length; ++i)
	{
		display_draw_pixel(i+x, y, invert ^ 1);
	}
}

void display_draw_rectangle(uint8_t x, uint8_t y, uint8_t height, uint8_t width, uint8_t fill, uint8_t invert)
{
	uint8_t i, j;
	for (j = 0; j <= height; j++)
	{
		for (i = 0; i <= width; i++)
		{
			if (fill == 1 || (i == 0 || i == width || j == 0 || j == height))
			display_draw_pixel((x + i), (y + j), invert ^ 1);
		}
	}
}

void display_draw_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t fill, uint8_t invert)
{
	uint8_t i = radius;
	uint8_t j = 0;
	int err = 0;

	while (i >= j)
	{
		if(fill == 0)
		{
			display_draw_pixel((x + i), (y + j), invert ^ 1);
			display_draw_pixel((x + j), (y + i), invert ^ 1);
			display_draw_pixel((x - j), (y + i), invert ^ 1);
			display_draw_pixel((x - i), (y + j), invert ^ 1);
			display_draw_pixel((x - i), (y - j), invert ^ 1);
			display_draw_pixel((x - j), (y - i), invert ^ 1);
			display_draw_pixel((x + j), (y - i), invert ^ 1);
			display_draw_pixel((x + i), (y - j), invert ^ 1);
		}
		if(fill == 1)
		{
			display_draw_HLine((x - j), (y + i), (2*j),invert);
			display_draw_HLine((x - i), (y - j), (2*i),invert);
			display_draw_HLine((x - i), (y + j), (2*i),invert);
			display_draw_HLine((x - j), (y - i), (2*j),invert);
		}
		if (err <= 0)
		{
			j += 1;
			err += 2 * j + 1;
		}
		if (err > 0)
		{
			i -= 1;
			err -= 2 * i + 1;
		}
	}
}

void display_clear()
{
	for (uint16_t buffer_location = 0; buffer_location < SSD1306_BUFFERSIZE; buffer_location++)
	{
		display_buffer[buffer_location] = 0x00;
	}
}

uint8_t display_draw_char(uint8_t x, uint8_t y, char ch, uint8_t invert)
{
	int byte_length = 0;
	if ((char_height % 8) == 0)
	{
		byte_length = (char_height / 8);
	}
	else
	{
		byte_length = ((char_height / 8) + 1);
	}
	int glyph = (byte_length * char_width);
	// character glyph buffer
	char _buf[glyph];
	int offset = (int)(ch - char_start) * (glyph + 1);
	uint8_t char_width = pgm_read_byte(&font[offset]);
	for(int j = 0; j < glyph; j++)
	{
		// ignore the first byte of the font table, it is the length of relevant lines
		_buf[j] = pgm_read_byte(&font[offset + j + 1]);
	}
	
	uint8_t cur_mask;
	uint8_t offset_cnt;
	
	for(uint8_t i = 0; i < char_width; ++i)
	{
		offset_cnt = 0;
		cur_mask = 0x01;
		for(uint8_t j = 0; j < char_height; ++j)
		{
			if(_buf[(i * byte_length) + offset_cnt] & cur_mask)
			{
				display_draw_pixel(x + i, y + j, invert ^ 1);
			}
			else
			{
				display_draw_pixel(x + i, y + j, invert);
			}
			if(cur_mask == 0x80)
			{
				cur_mask = 0x01;
				++offset_cnt;
			}
			else
			{
				cur_mask <<= 1;
			}
		}
	}
	return char_width;
}

void display_draw_string(uint8_t x, uint8_t y, char* str, uint8_t invert)
{
	while(*str)
	{	
		x += display_draw_char(x, y, *str, invert) + 1;
		str++;
	}
}

void display_invert(int x, int y, int height, int width)
{
	display_draw_rectangle( x, y, height, width, 1, 0);
}

void display_clear_invert()
{
	display_draw_rectangle(0,0,64,128,1,0);
}

void display_draw_line(int x1, int y1,int x2,int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int d = dy * 2 - dx;

	display_draw_pixel(x1,y1,1);
	while (x1 < x2)
	{
		if (d <= 0)
		{
			d += (dy * 2);
			if(dx > dy)
			x1++;
			else
			y1++;
		}
		else
		{
			d += (dy - dx) * 2;
			x1++;
			y1++;
		}
		display_draw_pixel(x1,y1,1);
	}
}

int main(void)
{
	ssd1306_init();

	display_clear();
	display_draw_string(19,10,"EmbedoS1",0);
	display_draw_string(35,40,"OLED",0);
	ssd1306_draw_buffer(display_buffer);
	_delay_ms(200);
	
/*	display_clear_invert();
	display_draw_string(19,10,"EmbedoS",1);
	display_draw_string(35,40,"OLED",1);
	ssd1306_draw_buffer(display_buffer);
	_delay_ms(100);

	display_clear();
	display_invert(0,32,32,128);
	display_draw_string(19,10,"EmbedoS",0); //(x,y,string,invert (0 = normal, 1 = invert)
	display_draw_string(35,40,"OLED",1);
	ssd1306_draw_buffer(display_buffer);
	_delay_ms(100);

	display_clear();
	display_invert(0,0,32,128);
	display_draw_string(19,10,"EmbedoS",1);
	display_draw_string(35,40,"OLED",0);
	ssd1306_draw_buffer(display_buffer);
	_delay_ms(100);*/

	while (1)
	{
/*		display_clear_invert();
		display_draw_circle(60,30,20,1,1);
		ssd1306_draw_buffer(display_buffer);
		_delay_ms(100);

		display_clear_invert();
		display_draw_circle(60,30,20,0,1);
		ssd1306_draw_buffer(display_buffer);
		_delay_ms(100);

		display_clear();
		display_draw_circle(60,30,20,1,0);
		ssd1306_draw_buffer(display_buffer);
		_delay_ms(100);

		display_clear();
		display_draw_circle(60,30,20,0,0);
		ssd1306_draw_buffer(display_buffer);
		_delay_ms(100);*/
		display_clear();
		display_draw_line(0,0,10,64);
		ssd1306_draw_buffer(display_buffer);
	}
}

