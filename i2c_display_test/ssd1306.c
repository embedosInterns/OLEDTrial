/*
 * ssd1306.c
 *
 * Created: 24-01-2017 1.21.41 PM
 *  Author: Bhavin
 */ 
#include "i2c.h"
#include "ssd1306.h"

void send_command(uint8_t command)
{
    i2c_start();
    i2c_write(0);
    i2c_write(command);
    i2c_stop();
}

void ssd1306_init()
{
    i2c_init(SSD1306_DEFAULT_ADDRESS);
    
    // Turn display off
    send_command(SSD1306_DISPLAYOFF);

    send_command(SSD1306_SETDISPLAYCLOCKDIV);
    send_command(0x80);

    send_command(SSD1306_SETMULTIPLEX);
    send_command(0x3F);
    
    send_command(SSD1306_SETDISPLAYOFFSET);
    send_command(0x00);
    
    send_command(SSD1306_SETSTARTLINE | 0x00);
    
    // We use internal charge pump
    send_command(SSD1306_CHARGEPUMP);
    send_command(0x14);
    
    // Horizontal memory mode
    send_command(SSD1306_MEMORYMODE);
    send_command(0x00);
    
    send_command(SSD1306_SEGREMAP | 0x1);

    send_command(SSD1306_COMSCANDEC);

    send_command(SSD1306_SETCOMPINS);
    send_command(0x12);

    // Max contrast
    send_command(SSD1306_SETCONTRAST);
    send_command(0xCF);

    send_command(SSD1306_SETPRECHARGE);
    send_command(0xF1);

    send_command(SSD1306_SETVCOMDETECT);
    send_command(0x40);

    send_command(SSD1306_DISPLAYALLON_RESUME);

    // Non-inverted display
    send_command(SSD1306_NORMALDISPLAY);

    // Turn display back on
    send_command(SSD1306_DISPLAYON);
}

void ssd1306_draw_buffer(const uint8_t *buffer)
{
    send_command(SSD1306_COLUMNADDR);
    send_command(0x00);
    send_command(0x7F);

    send_command(SSD1306_PAGEADDR);
    send_command(0x00);
    send_command(0x07);

    // We have to send the buffer as 16 bytes packets
    // Our buffer is 1024 bytes long, 1024/16 = 64
    // We have to send 64 packets
    for (uint8_t packet = 0; packet < 64; packet++) {
        i2c_start();
        i2c_write(0x40);
        for (uint8_t packet_byte = 0; packet_byte < 16; ++packet_byte) {
            i2c_write(buffer[packet*16+packet_byte]);
        }
        i2c_stop();
    }
}