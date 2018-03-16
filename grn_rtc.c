/*	Library File
 * 
 * 	Microchip MCP79410 RTC
 *
 * 	grn; Apr 15
 * 
 * Bemerkung: 	To set time (rtc_set_time) rtc must be stopped.
 * 
 * 		rtc_stop();
 *		rtc_set_time(0, 0, 0);
 *		rtc_start(); 
 */
 
#define F_CPU 16000000UL  // 1 MHz
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "grn_TWI.h"
#include "grn_rtc.h"

#define SRAM_W 0b11011110
#define SRAM_R 0b11011111
#define RTCSEC 0x00
#define RTCMIN 0x01
#define RTCHOUR 0x02
#define RTCDAY 0x04
#define RTCMONTH 0x05
#define RTCYEAR 0x06
#define RTCWKDAY 0x03
#define CONTROL 0x07
	
uint16_t rtc_init(void)
{
	_delay_ms(1);
	TWIStart();								
	if(TWIGetStatus() != 0x08)return 1; 	
	TWIWrite(SRAM_W);				//SRAM Write		
	if(TWIGetStatus() != 0x18)return 2;		
	TWIWrite(CONTROL);				//angesprochenes Register
	if(TWIGetStatus() != 0x28)return 3;		
	TWIWrite(0x00);					//zu schreibender Wert
	if(TWIGetStatus() != 0x28)return 4;		
	TWIStop();	
	return 0;	
}//End of RTC Write
uint16_t rtc_write(uint8_t rtc_adresse, uint8_t rtc_wert)
{
	_delay_ms(1);
	TWIStart();								
	if(TWIGetStatus() != 0x08)return 1; 	
	TWIWrite(SRAM_W);				//SRAM Write		
	if(TWIGetStatus() != 0x18)return 2;		
	TWIWrite(rtc_adresse);				//nagesprochenes Register
	if(TWIGetStatus() != 0x28)return 3;		
	TWIWrite(rtc_wert);					//zu schreibender Wert
	if(TWIGetStatus() != 0x28)return 4;		
	TWIStop();	
	return 0;	
}//End of RTC Write

uint8_t rtc_read(uint8_t rtc_adresse)
{
	uint8_t rtc_data=0;
	_delay_ms(1);
	TWIStart();								
	if(TWIGetStatus() != 0x08)return 1; 	
	TWIWrite(SRAM_W);				//SRAM Write		
	if(TWIGetStatus() != 0x18)return 2;		
	TWIWrite(rtc_adresse);				//nagesprochenes Register
	if(TWIGetStatus() != 0x28)return 3;		
	TWIStart();							//Restart
	if(TWIGetStatus() != 0x10)return 10;
	TWIWrite(SRAM_R);				//SRAM Read
	if(TWIGetStatus() != 0x40)return 11;
	rtc_data = TWIReadNACK();
	if(TWIGetStatus() != 0x58)return 12;
	TWIStop();
	return rtc_data;	
}//End of RTC Read
void rtc_stop(void)
{	
	uint8_t temp;
	temp=0;
	temp = rtc_read(RTCSEC);//aktueller Wert lesen
	temp = temp & ~0b10000000;//Oszollator ausschalten
	rtc_write(RTCSEC, temp); //Gleicher Wert aber mit Osz gestoppt zurückschreiben
}//End of rct_stop
void rtc_start(void)
{	
	uint8_t temp;
	temp=0;
	temp = rtc_read(RTCSEC);
	temp |= 0b10000000;//Oszollator einschalten
	rtc_write(RTCSEC, temp); 
	temp = rtc_read(RTCWKDAY);
	temp |= 0b00001000;//Enable Backupbattery
	rtc_write(RTCWKDAY, temp); 
}//End of rct_start
void rtc_set_time(uint8_t stunde, uint8_t minute, uint8_t sekunde)
{
	uint8_t stundez, minutez, sekundez, rtcsec,rtcmin,rtchour, tt;
	rtcsec=0;
	rtcmin=0;
	rtchour=0;
	tt=stunde;
	
	stundez=tt/10;
	rtchour=(stundez<<4);
	rtchour |= stunde%10;
	
	tt=minute;
	minutez=tt/10;
	rtcmin=(minutez<<4);
	rtcmin |= minute%10;
	
	tt=sekunde;
	sekundez=tt/10;
	rtcsec=(sekundez<<4);
	rtcsec |= sekunde%10;
			
	rtc_write(RTCSEC, rtcsec);//Oz on, werte 0
	rtc_write(RTCMIN, rtcmin);
	rtc_write(RTCHOUR, rtchour);//Modus:24, AM Werte 0
}//End of rct_set_time
void rtc_set_date(uint8_t jahr, uint8_t monat, uint8_t tag)
{
	uint8_t jahrz, monatz, tagz, rtcyear,rtcmonth,rtcday, tt;
			
	rtcyear=0;
	rtcmonth=0;
	rtcday=0;
	
	tt=jahr;
	jahrz=tt/10;
	rtcyear=(jahrz<<4);
	rtcyear |= jahr%10;
	
	tt=monat;
	monatz=tt/10;
	rtcmonth=(monatz<<4);
	rtcmonth |= monat%10;
	
	tt=tag;
	tagz=tt/10;
	rtcday=(tagz<<4);
	rtcday |= tag%10;
			
	rtc_write(RTCDAY, rtcday);//Oz on, werte 0
	rtc_write(RTCMONTH, rtcmonth);
	rtc_write(RTCYEAR, rtcyear);//Modus:24, AM Werte 0
}//End of rct_set_date
uint8_t rtc_read_sec(void)
{
	uint8_t sec, temp, ret;
	sec=0;	//Sekunden einer
	temp=0; //Sekunden Zehner
	ret=0; //Return Wert
		
	sec = rtc_read(RTCSEC);
	temp = sec;
	sec = sec & ~0b11110000;	//nur hintere 4 Bits geben Sekunden Einer an
	temp = temp & ~0b10001111;	//Sekunden Zehner Bits maskieren
	temp = 10*(temp >> 4);
	ret = temp+sec;
	return ret;
}//End of rtc_sec
uint8_t rtc_read_min(void)
{
	uint8_t min, temp, ret;
	min=0;	//Minute einer
	temp=0; //Minute Zehner
	ret=0; //Return Wert
		
	min = rtc_read(RTCMIN);
	temp = min;
	min = min & ~0b11110000;	//nur hintere 4 Bits geben Minuten Einer an
	temp = temp & ~0b10001111;	//Minuten Zehner Bits maskieren
	temp = 10*(temp >> 4);
	ret = temp+min;
	return ret;
}
uint8_t rtc_read_hour(void)
{
	uint8_t hour, temp, ret;
	hour=0;	//Sekunden einer
	temp=0; //Sekunden Zehner
	ret=0; //Return Wert
		
	hour = rtc_read(RTCHOUR);
	temp = hour;
	hour = hour & ~0b11110000;	//nur hintere 4 Bits geben Stunden Einer an
	temp = temp & ~0b11001111;	//Stunden Zehner Bits maskieren
	temp = 10*(temp >> 4);
	ret = temp+hour;
	return ret;
}
uint8_t rtc_read_year(void)
{
	uint8_t year, temp, ret;
	year=0;	//Sekunden einer
	temp=0; //Sekunden Zehner
	ret=0; //Return Wert
		
	year = rtc_read(RTCYEAR);
	temp = year;
	year = year & ~0b11110000;	
	temp = temp & ~0b00001111;	
	temp = 10*(temp >> 4);
	ret = temp+year;//return wert nur 15 nicht 2015
	return ret;
}//End of rtc_read_year

uint8_t rtc_read_month(void)
{
	uint8_t month, temp, ret;
	month=0;	//Sekunden einer
	temp=0; //Sekunden Zehner
	ret=0; //Return Wert
		
	month = rtc_read(RTCMONTH);
	temp = month;
	month = month & ~0b11110000;	
	temp = temp & ~0b11101111;	
	temp = 10*(temp >> 4);
	ret = temp+month;//return wert nur 15 nicht 2015
	return ret;
}//End of rtc_read_month

uint8_t rtc_read_day(void)
{
	uint8_t day, temp, ret;
	day=0;	//Sekunden einer
	temp=0; //Sekunden Zehner
	ret=0; //Return Wert
		
	day = rtc_read(RTCDAY);
	temp = day;
	day = day & ~0b11110000;	
	temp = temp & ~0b11001111;	
	temp = 10*(temp >> 4);
	ret = temp+day;//return wert nur 15 nicht 2015
	return ret;
}//End of rtc_read_day
