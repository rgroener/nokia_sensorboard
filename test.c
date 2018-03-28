/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */


#include "test.h"

uint8_t bme280_init()
{
	TWIStart();
	if(TWIGetStatus() != 0x08)return 1; 	
	TWIWrite(BME280_WRITE);
	if(TWIGetStatus() != 0x18)return 2;
	
	TWIWrite(REG_CTRL_HUM);
	if(TWIGetStatus() != 0x28)return 8;
	TWIWrite(0x01);							//Humidity sampling x1
	if(TWIGetStatus() != 0x28)return 8;
	
	TWIWrite(REG_CTRL_MEAS);
	if(TWIGetStatus() != 0x28)return 8;
	TWIWrite(0x27);							//Temp and Pres sampling x1 normal mode 0b00100111
	if(TWIGetStatus() != 0x28)return 8;
	
	TWIStop();
	//TWIWrite(REG_ID);
	//if(TWIGetStatus() != 0x28)return 8;
	//read_calib
	
	calib_T1 = bme280_read16LE(CALIB_T1);
	calib_T2 = bme280_read16LE(CALIB_T2);
	calib_T3 = bme280_read16LE(CALIB_T3);
	
	
	
	return 0;
	
}
uint8_t bme280_read8(uint8_t reg)
{
  	uint8_t retdata;
	
	retdata=0;//Data to be returned

	TWIStart();
	if(TWIGetStatus() != 0x08)return 6; //0x08 = Start condition
	TWIWrite(BME280_WRITE);
	if(TWIGetStatus() != 0x18)return 7;
	TWIWrite(reg);
	if(TWIGetStatus() != 0x28)return 8;
	TWIStart();
	if(TWIGetStatus() != 0x10)return 10;
	TWIWrite(BME280_READ);
	if(TWIGetStatus() != 0x40)return 11;
	retdata = TWIReadNACK();
	if(TWIGetStatus() != 0x58)return 12;
	TWIStop();
	
	return retdata;	
}
uint16_t bme280_read16(uint8_t reg)
{
	uint8_t data_l, data_h;
	uint16_t retdata;
	
	data_h = bme280_read8(reg);
	data_l =bme280_read8(reg+1);
	retdata = (data_l | (data_h<<8));	
	
	return retdata;
}
uint16_t bme280_read16LE(uint8_t reg)
{
	//Change byte order
	uint16_t temp=0;
	temp = bme280_read16(reg);
	return ((temp>>8) | (temp<<8));
}
uint16_t bme280_readS16(uint8_t reg)
{
	//Unsigned to signed conversion
	return ((int16_t)bme280_read16(reg));
}
uint32_t bme280_read_temp(void)
{
	uint32_t var32;
	
	
	var32 = bme280_read8(REG_TEMP_MSB);
	var32 <<= 8;
	var32 |= bme280_read8(REG_TEMP_LSB);
	var32 <<= 8;
	var32 |= bme280_read8(REG_TEMP_XLSB);
	var32 >>= 4;
	
	int32_t var1, var2;
  
    
  var1  = ((((var32>>3) - ((int32_t)calib_T1 <<1))) *
           ((int32_t)calib_T2)) >> 11;
  
  var2  = (((((var32>>4) - ((int32_t)calib_T1)) *
             ((var32>>4) - ((int32_t)calib_T1))) >> 12) *
           ((int32_t)calib_T3)) >> 14;
  
  t_fine = var1 + var2;
  
  float T  = (t_fine * 5 + 128) >> 8;

  return T;
}
uint8_t vor_komma(uint32_t value)
{
	uint8_t temp;
	temp = value/100;
	return temp;
	
}
uint8_t nach_komma(uint32_t value)
{
	uint8_t temp;
	temp = value/100;
	return value-(temp*100);
	
	
}
