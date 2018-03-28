
//
//  Testlibrary for BME280 Sensor
//
//  
//
//  
//


#define F_CPU 16000000UL  // 1 MHz
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "grn_TWI.h"

#define BME280_WRITE 0b11101110
#define BME280_READ 0b11101111
#define REG_ID 			0xD0 	//contains 0x60
#define	REG_RESET		0xE0
#define	REG_CTRL_HUM	0xF2
#define	REG_STATUS		0xF3
#define	REG_CTRL_MEAS 	0xF4
#define	REG_CONFIG		0xF5
#define	REG_PRES_MSB	0xF7
#define	REG_PRES_LSB	0xF8
#define	REG_PRES_XLSB	0xF9
#define	REG_TEMP_MSB	0xFA
#define REG_TEMP_LSB	0xFB
#define REG_TEMP_XLSB	0xFC	//Bit 7,6,5,4
#define REG_HUM_MSB		0xFD
#define REG_HUM_LSB		0xFE

// Calib Data Temperature
#define CALIB_T1		0x88
#define CALIB_T2		0x8A
#define CALIB_T3		0x8C

uint16_t calib_T1;
int16_t calib_T2, calib_T3;
uint32_t t_fine;

//Calib Data Pressure
#define CALIB_P1		0x8E
#define CALIB_P2		0x90
#define CALIB_P3		0x92
#define CALIB_P4		0x94
#define CALIB_P5		0x96
#define CALIB_P6		0x98
#define CALIB_P7		0x9A
#define CALIB_P8		0x9C
#define CALIB_P9		0x9E
/*
//Calib Data Humidity
#define CALIB_H1		0xA1




#define CALIB_1			0x89
#define CALIB_3			0x8B
#define CALIB_5			0x8D
#define CALIB_7			0x8F
#define CALIB_9			0x91
#define CALIB_11		0x93
#define CALIB_13		0x95
#define CALIB_15		0x97




#define CALIB_23		0x9F
#define CALIB_24		0xA0

#define CALIB_17		0x99
#define CALIB_19		0x9B
#define CALIB_21		0x9D

*/


uint8_t bme280_init(void);				//initialisation of bme280
uint8_t bme280_read8(uint8_t reg);		//read 8 bit value
uint16_t bme280_read16(uint8_t reg);	//read 16bit value
uint16_t bme280_read16LE(uint8_t reg);	//change byte order
uint16_t bme280_readS16(uint8_t reg);	//unsigned to signed

uint32_t bme280_read_temp(void);			//read raw temp data (20 bit)
uint8_t vor_komma(uint32_t value);
uint8_t nach_komma(uint32_t value);

