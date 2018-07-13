/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */


#include "grn_bme280.h"

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
	calib_T2 = bme280_readS16LE(CALIB_T2);
	calib_T3 = bme280_readS16LE(CALIB_T3);
	
	calib_P1 = bme280_read16LE(CALIB_P1);
	calib_P2 = bme280_readS16LE(CALIB_P2);
	calib_P3 = bme280_readS16LE(CALIB_P3);
	calib_P4 = bme280_readS16LE(CALIB_P4);
	calib_P5 = bme280_readS16LE(CALIB_P5);
	calib_P6 = bme280_readS16LE(CALIB_P6);
	
	calib_H1 = bme280_read16LE(CALIB_H1);
	calib_H2 = bme280_readS16LE(CALIB_H2);
	calib_H3 = bme280_read16LE(CALIB_H3);
	calib_H4 = bme280_readS16LE(CALIB_H4);
	calib_H5 = bme280_readS16LE(CALIB_H5);
	calib_H6 = bme280_read8(CALIB_H6);
	
	/*
	calib_H1 = read8(BME280_REGISTER_DIG_H1);
  calib_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
  calib_H3 = read8(BME280_REGISTER_DIG_H3);
  calib_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
  calib_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
  calib_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
	*/
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
/*
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
	data_h = TWIReadACK();
	if(TWIGetStatus() != 0x28)return 12;
	data_l = TWIReadACK();
	if(TWIGetStatus() != 0x28)return 12;
	TWIStop();
	
*/
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
int16_t bme280_readS16LE(uint8_t reg)
{
  return (int16_t)bme280_read16LE(reg);
  
}
float bme280_read_temp(void)
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

float bme280_read_pressure(void)
{
	uint32_t pres;
	int64_t var1, var2, p;
	
	bme280_read_temp();
	
	pres = bme280_read8(REG_PRES_MSB);
	pres <<= 8;
	pres |= bme280_read8(REG_PRES_LSB);
	pres <<= 8;
	pres |= bme280_read8(REG_PRES_XLSB);
	pres >>= 4;
	
	
  
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)calib_P6;
  var2 = var2 + ((var1*(int64_t)calib_P5)<<17);
  var2 = var2 + (((int64_t)calib_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)calib_P3)>>8) +
  ((var1 * (int64_t)calib_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calib_P1)>>33;
  
  if (var1 == 0) {
    return 55;  // avoid exception caused by division by zero
  }
  
  p = 1048576 - pres;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)calib_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)calib_P8) * p) >> 19;
  
  //return var2;
  
  p = ((p + var1 + var2) >> 8) + (((int64_t)calib_P7)<<4);
  
  return (float)p/256;
}

float bme280_read_humidity(void)
{
  
  bme280_read_temp(); // must be done first to get t_fine
  
  int32_t var32;
  int32_t v_x1_u32r;
  
	var32 = bme280_read8(REG_HUM_MSB);
	var32 <<= 8;
	var32 |= bme280_read8(REG_HUM_LSB);
	
  
  v_x1_u32r = (t_fine - ((int32_t)76800));
  
  v_x1_u32r = (((((var32 << 14) - (((int32_t)calib_H4) << 20) -
                  (((int32_t)calib_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
               (((((((v_x1_u32r * ((int32_t)calib_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)calib_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                  ((int32_t)2097152)) * ((int32_t)calib_H2) + 8192) >> 14));
  
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)calib_H1)) >> 4));
  
  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;

}
