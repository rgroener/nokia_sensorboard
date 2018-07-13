/*


*/
#define F_CPU 16000000UL  // 1 MHz
#include <avr/io.h>
#include "glcd/glcd.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "glcd/fonts/Liberation_Sans15x21_Numbers.h"
#include "glcd/fonts/font5x7.h"
#include <avr/pgmspace.h>
#include "grn_TWI.h"
#include "grn_24C512.h"
#include "grn_rtc.h"
#include "grn_bme280.h"



#define T_RED !(PIND & (1<<PD5)) && (entprell == 0)
#define T_BLUE !(PIND & (1<<PD6)) && (entprell == 0)
#define T_GREEN !(PIND & (1<<PD2)) && (entprell == 0)
#define RELOAD_ENTPRELL 80

#define LED_EIN PORTC |= (1<<PC3)
#define LED_AUS	PORTC &= ~(1<<PC3);					//LED ausschalten


uint8_t test1, test1_alt, aaa,bbb,ccc, ddd, error;
uint16_t zaehler, test2, test2_alt, test3;
int32_t temperatur;
uint32_t pressure, humidity;
uint8_t sekunden, sekunden_alt, minuten, stunden;

// String für Zahlenausgabe
char string[30] = "";

uint8_t ms, ms10,ms100,sec,min,entprell, state;
uint8_t end_ms100, end_sec, end_min;
enum state{WAIT, COUNT, TIME, TIME_WAIT,FLIGHT_TIME};








/* Function prototypes */
static void setup(void);

static void setup(void)
{
	/* Set up glcd, also sets up SPI and relevent GPIO pins */
	glcd_init();
}




int main(void)
{
	/* Backlight pin PL3, set as output, set high for 100% output */
	DDRB |= (1<<PB2);
	//PORTB |= (1<<PB2);
	PORTB &= ~(1<<PB2);
	
	
	DDRC |=(1<<PC3); 	//Ausgang LED
	PORTC |= (1<<PC3);	//Led ein
	
	
	DDRD &= ~((1<<PD6) | (1<<PD2) | (1<<PD5)); 	//Taster 1-3
	PORTD |= ((1<<PD6) | (1<<PD2) | (1<<PD5)); 	//PUllups für Taster einschalten
	
	DDRD &= ~(1<<PD4); //T0 Counter Input
	TCCR0B |= (1<<CS02) | (1<<CS01) | (1<<CS00);//Counter 0 enabled clock on rising edge
	
	//Timer 1 Configuration
	OCR1A = 0x009C;	//OCR1A = 0x3D08;==1sec
	
    TCCR1B |= (1 << WGM12);
    // Mode 4, CTC on OCR1A

    TIMSK1 |= (1 << OCIE1A);
    //Set interrupt on compare match

    TCCR1B |= (1 << CS12) | (1 << CS10);
    // set prescaler to 1024 and start the timer

    sei();
    // enable interrupts
	
	setup();
	rtc_init();
	rtc_start();
	
	
	
	zaehler=222;
	entprell=0;
	test1=0;
	test1_alt=0;
	test2=0;
	test2_alt=0;
	ms10=0;
	ms100=0;
	sec=0;
	min=0;
	sekunden=0;
	sekunden_alt=0;
	minuten=0;
	stunden=0;
	test3=0;
	aaa=255;
	bbb=0;
	ccc=0;
	ddd=0;
	temperatur=0;
	error=0;
	pressure = 0;
	humidity = 0;
	


	
		glcd_tiny_set_font(Font5x7,5,7,32,127);
		glcd_clear_buffer();
		rtc_stop();
		rtc_set_time(0, 0, 0);
		rtc_start();
		
	
	/*
 		EEWrite_8(3, 1, 55);
 		_delay_ms(50);
 		
 		ddd=EERead_8(3, 1);
 	*/	
 		
 		error = bme280_init();
 		test1=bme280_read8(REG_ID);
		glcd_clear();
		
		
		
		/*
		//wechselt die Bitreihenfolge
		test2 = 255;
		test3 = (test2>>8) | (test2<<8);
		*/
		
		
		
		
		while(1) 
		{	
			
	
			sekunden = rtc_read_sec();
			if(sekunden != sekunden_alt)
			{
				sekunden_alt=sekunden;
				minuten = rtc_read_min();
				stunden = rtc_read_hour();
				glcd_clear();
				
				temperatur = bme280_read_temp();
				pressure = bme280_read_pressure();
				humidity = bme280_read_humidity();
		
			
				sprintf(string,"Temp: %d.%1.2d", vor_komma(temperatur), nach_komma(temperatur) );
				glcd_draw_string_xy(0,0,string);
			
				sprintf(string,"pres %lu", pressure);
				glcd_draw_string_xy(0,15,string);
				
				sprintf(string,"hum %lu", humidity);
				glcd_draw_string_xy(0,30,string);
				
				//sprintf(string,"%01d : %01d : %01d",stunden, minuten, sekunden);
			//	glcd_draw_string_xy(0,30,string);
			
			}
			
			
			
				
		
		
		
		if(T_RED)
		{
			LED_AUS;
		}else
		{
			LED_EIN;
		}
		
	
	glcd_write();
	}//End of while
	
	
	return 0;
}//end of main
