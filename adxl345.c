#include "adxl345.h"
#include "i2c_lcd.h"
#include "systick.h"
#include "i2c.h"
#include "stdio.h"

#define SCALE 256.0  // Scale factor for ±2g range
#define G 9.81  // Gravity constant for converting g to m/s²

float alpha = 0.81;
	

void ADXL345_Init(uint8_t i2c) {
    // Initialize I2C if not already initialized
    i2c_init(i2c, I2C_FM);
	
    ADXL345_WriteReg(i2c, ADXL345_POWER_CTL, 0x08);

    // Set data format to full resolution with ±2g range
    ADXL345_WriteReg(i2c, ADXL345_DATA_FORMAT, 0x08);
//    lcd_i2c_cmd(I2C_1, 0x01);
    ADXL345_WriteReg(i2c, ADXL345_BW_RATE, 0x0A);
}

void ADXL345_WriteReg(uint8_t i2c, uint8_t reg, uint8_t value) {
    i2c_start(i2c);
    i2c_add(i2c, ADXL345_ADDRESS << 1, 0); // Write mode
    i2c_data(i2c, reg);
    i2c_data(i2c, value);
    i2c_stop(i2c);
}

void ADXL345_ReadXYZ(uint8_t i2c, float* x, float* y, float* z, float *g_x, float *g_y, float *g_z) {
    uint8_t data[6];
    int16_t raw_x, raw_y, raw_z;

    data[0] = ADXL345_ReadReg(i2c, ADXL345_DATAX0);
    data[1] = ADXL345_ReadReg(i2c, ADXL345_DATAX1);
    data[2] = ADXL345_ReadReg(i2c, ADXL345_DATAY0);
    data[3] = ADXL345_ReadReg(i2c, ADXL345_DATAY1);
    data[4] = ADXL345_ReadReg(i2c, ADXL345_DATAZ0);
    data[5] = ADXL345_ReadReg(i2c, ADXL345_DATAZ1);

    raw_x = ((int16_t)data[1] << 8) | data[0];
    raw_y = ((int16_t)data[3] << 8) | data[2];
    raw_z = ((int16_t)data[5] << 8) | data[4];

    *x = (raw_x / SCALE) * G;
    *y = (raw_y / SCALE) * G;
    *z = (raw_z / SCALE) * G;
    
		*g_x = *g_x * alpha + (1 - alpha)*(*x);
    *g_y = *g_y * alpha + (1 -alpha)*(*y);
    *g_z = (*g_z) *  alpha + (1 - alpha)*(*z);
		
		*x = *x - (*g_x);
		*y = *y - (*g_y);
		*z = *z - (*g_z);
		
}

uint8_t ADXL345_ReadReg(uint8_t i2c, uint8_t reg) {
    uint8_t value;
    
    i2c_start(i2c);
    i2c_add(i2c, ADXL345_ADDRESS << 1, 0); // Write mode
    i2c_data(i2c, reg);
    i2c_start(i2c);
    i2c_add(i2c, ADXL345_ADDRESS << 1, 1); // Read mode
    value = i2c_read(i2c, 0);
    i2c_stop(i2c);
    
    return value;
}
