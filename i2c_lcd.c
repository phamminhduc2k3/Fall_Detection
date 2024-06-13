#include "i2c_lcd.h"
#include "systick.h"

void add_slave(uint8_t i2c, char RW){
	i2c_add(i2c, Slave_Address, RW);
}
void trans_slave(uint8_t i2c, char data){
	i2c_start(i2c);
	add_slave(i2c, 0);
	i2c_data(i2c, data);
	i2c_stop(i2c);
}
void lcd_i2c_data(uint8_t i2c, unsigned char data){
	// rs high, rw low
	trans_slave(i2c, 0x09);
	delay_us(10);
	// e high
	trans_slave(i2c, 0x0D);
	delay_us(5);

	// trans data
	trans_slave(i2c, ((data & 0x00f0) | 0x0D));
	delay_us(10);
	// e low
	trans_slave(i2c, ((data & 0x00f0) | 0x09));
	delay_us(20);
	
	// e high
	trans_slave(i2c, 0x0D);
	delay_us(5);
	trans_slave(i2c, (((data << 4) & 0x00f0) | 0x0D));
	delay_us(10);
	trans_slave(i2c, (((data << 4) & 0x00f0) | 0x09));
}


void lcd_i2c_cmd(uint8_t i2c, unsigned char data){
	trans_slave(i2c, 0x08);
	delay_us(10);
	trans_slave(i2c, 0x0C);
	delay_us(5);
	// data 
	trans_slave(i2c, ((data & 0x00f0) | 0x0C)); // 00001100
	delay_us(10);
	trans_slave(i2c, ((data & 0x00f0) | 0x08)); // 00001000
	delay_us(20);
	trans_slave(i2c, 0x0C); //00001100
	delay_us(5);

	trans_slave(i2c, (((data << 4) & 0x00f0) | 0x0C));
	delay_us(10);
	trans_slave(i2c, (((data << 4) & 0x00f0) | 0x08));
}

void lcd_i2c_init(uint8_t i2c){
	//d7-d6-d5-d4-light-en-rw-rs
  i2c_init(i2c, I2C_FM);
	delay_ms(20);
	trans_slave(i2c, 0x08); // 00001000
	delay_us(10);
	// enable high
	trans_slave(i2c, 0x0C); // 00001100
	delay_us(5);	
	trans_slave(i2c, 0x3C); // 00111100
	delay_us(10);
	trans_slave(i2c, 0x38); // 00111000
	delay_ms(1);
	trans_slave(i2c, 0x08); // 00001000
	delay_us(10);
	trans_slave(i2c, 0x0C);
	delay_us(5);
	trans_slave(i2c, 0x3C);
	delay_us(10);
	trans_slave(i2c, 0x38);
	delay_ms(1);
	trans_slave(i2c, 0x08);
	delay_us(10);
	trans_slave(i2c, 0x0C);
	delay_us(5);
	trans_slave(i2c, 0x3C);
	delay_us(10);
	trans_slave(i2c, 0x38);
	delay_ms(1);
	trans_slave(i2c, 0x08);
	delay_us(10);
	trans_slave(i2c, 0x0C);
	delay_us(5);
	trans_slave(i2c, 0x2C); //00101100
	delay_us(10);
	trans_slave(i2c, 0x28); // 00101000
	lcd_i2c_cmd(i2c, 0x2C); //00101100 // 4 bit communication mode // 2 lines-display N = 1
	delay_ms(5);
	lcd_i2c_cmd(i2c, 0x0C); // Display ON
	delay_ms(5);
	lcd_i2c_cmd(i2c, 0x01); // Clear Display // rs = 1	
	delay_ms(5);
	lcd_i2c_cmd(i2c, 0x02); // Get back to initial address
	delay_ms(5);
}
void lcd_i2c_send(uint8_t i2c,  char str[]){
	int i = 0;
		while(str[i])
		{
			lcd_i2c_data(i2c, str[i]);
			i++;
			delay_us(100);
		}
}
void lcd_i2c_msg(uint8_t i2c, unsigned char line, unsigned char col, char msg[]){
	short pos = 0;
	if(line==1)
	{
		pos = 0x80;
	}
	else if(line==2)
	{
		pos = 0xC0;
	}
	lcd_i2c_cmd(i2c,pos + col);
	delay_us(100);
	lcd_i2c_send(i2c, msg);
}