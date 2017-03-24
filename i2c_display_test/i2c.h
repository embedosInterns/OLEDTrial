/*
 * i2c.h
 *
 * Created: 24-01-2017 1.20.06 PM
 *  Author: Bhavin
 */ 


#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>

void i2c_init(uint8_t address);
uint8_t i2c_start();
uint8_t i2c_write(uint8_t data);
void i2c_stop();

#endif /* I2C_H_ */