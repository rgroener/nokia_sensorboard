/*
 * 24C256 und 24C512 sind beide kompatibel bezüglich Adressierung
 * 
 * Achtung!!!!  Wenn auf EEPROM geschrieben und gleich wieder gelesen 
 * wird, kann es zu fehlerhaften Ausgaben kommen. Unbedingt ein paar 
 * Milisekunden Delay einbauen bevor zurückgelesen wird...
 * 
 * 		EEWrite_8(1, 1, 123);
 *		_delay_ms(50);
 *		test=EERead_8(1, 1);
 *		lcd_print(test);
 * 
 * 
 * */
#define F_CPU 16000000UL  // 1 MHz
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "grn_TWI.h"

/*
 * Device Addressing
 * 
 * 1010				000 			0
 * Control code		Chip select		W/R (0=Write; 1=Read)
 * */
#define EEPROM_1_W 0b10100000	//EEPROM Adresse und schreiben
#define EEPROM_1_R 0b10100001 //EEPROM Adresse und lesen
#define EEPROM_2_W 0b10100010	//EEPROM Adresse und schreiben
#define EEPROM_2_R 0b10100011 //EEPROM Adresse und lesen
#define EEPROM_3_W 0b10100100	//EEPROM Adresse und schreiben
#define EEPROM_3_R 0b10100101 //EEPROM Adresse und lesen
#define EEPROM_4_W 0b10100110	//EEPROM Adresse und schreiben
#define EEPROM_4_R 0b10100111 //EEPROM Adresse und lesen

uint8_t EEWrite_8(uint16_t u16addr, uint8_t EENumber, uint8_t u8data)
{
	uint8_t addr_l, addr_h, ew;
	addr_l = u16addr;
	addr_h = (u16addr>>8);
	ew=0;
	
	
	switch(EENumber)
	{
		case 1: ew = EEPROM_1_W;
				break;
		case 2: ew = EEPROM_2_W;
				break;
		case 3: ew = EEPROM_3_W;
				break;
		case 4: ew = EEPROM_4_W;
				break;
	}
	
	TWIStart();
	if(TWIGetStatus() != 0x08)return 1; 
	TWIWrite(ew);
	if(TWIGetStatus() != 0x18)return 2;
	TWIWrite(addr_l);
	if(TWIGetStatus() != 0x28)return 3;
	TWIWrite(addr_h);
	if(TWIGetStatus() != 0x28)return 4;
	TWIWrite(u8data);
	if(TWIGetStatus() != 0x28)return 5;
	TWIStop();
	_delay_ms(5);	//EEPROM braucht Zeit zum schreiben
	
	return 0;	
}

uint8_t EERead_8(uint16_t u16addr, uint8_t EENumber)
{
	uint8_t addr_l, addr_h,er, ew, retdata;
	addr_l = u16addr;
	addr_h = (u16addr>>8);
	ew=0;
	er=0;
	retdata=0;//Data to be returned
	
	/*
	 * richtige schreib- / leseadresse
	 * gemäss gewähltem EEprom 
	 */
	switch(EENumber)	
	{
		case 1: ew = EEPROM_1_W;
				er = EEPROM_1_R;
				break;
		case 2: ew = EEPROM_2_W;
				er = EEPROM_2_R;
				break;
		case 3: ew = EEPROM_3_W;
				er = EEPROM_3_R;
				break;
		case 4: ew = EEPROM_4_W;
				er = EEPROM_4_R;
				break;
	}
	TWIStart();
	if(TWIGetStatus() != 0x08)return 6; //0x08 = Start condition
	TWIWrite(ew);
	if(TWIGetStatus() != 0x18)return 7;
	TWIWrite(addr_l);
	if(TWIGetStatus() != 0x28)return 8;
	TWIWrite(addr_h);
	if(TWIGetStatus() != 0x28)return 9;
	TWIStart();
	if(TWIGetStatus() != 0x10)return 10;
	TWIWrite(er);
	if(TWIGetStatus() != 0x40)return 11;
	retdata = TWIReadNACK();
	if(TWIGetStatus() != 0x58)return 12;
	TWIStop();
	
	return retdata;	
}
uint8_t EEWrite_16(uint16_t u16addr, uint8_t EENumber, uint16_t u16data)
{
	uint8_t data_l, data_h;
	
	data_h = (u16data>>8);
	data_l = u16data;
	
	EEWrite_8(u16addr, EENumber, data_h);
	EEWrite_8((u16addr+1), EENumber, data_l);
	return 0;
}

uint16_t EERead_16(uint16_t u16addr, uint8_t EENumber)
{
	uint8_t data_l, data_h;
	uint16_t data;
	
	data_h= EERead_8(u16addr, EENumber);
	data_l=EERead_8((u16addr+1), EENumber);
	data = (data_l | (data_h<<8));	
	
	return data;
}
