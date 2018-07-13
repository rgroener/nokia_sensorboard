#include <stdio.h>
#include <avr/io.h>


/*    Grundfunktionen TWI Schnittstelle
 *    grn, 14.4.2015
 *     
 */

void TWIInit(void)
{
 /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
  
  TWSR =0;                         /* no prescaler */
  TWBR = ((20000000/400000)-16)/2;  /* (F_CPU / F_TWI) must be > 10 for stable operation */
}
 
void TWIStart(void)
{
	TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
	while((TWCR & (1<<TWINT)) == 0);
}

void TWIStop(void)
{
	TWCR = ((1<<TWINT) | (1<<TWSTO) | (1<<TWEN));
} 

void TWIWrite(uint8_t u8data)
{
	TWDR = u8data;
	TWCR = ((1<<TWINT) | (1<<TWEN));
	while((TWCR & (1<<TWINT)) == 0);
}

uint8_t TWIReadACK(void)
{
	TWCR = ((1<<TWINT) | (1<<TWEN) | (1<<TWEA));
	while((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t TWIReadNACK(void)
{
	TWCR = ((1<<TWINT) | (1<<TWEN));
	while((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t TWIGetStatus(void)
{
	/*  0x08   	Start condition sent
	 *  0x10   	repeated start condition sent
	 *  0x18   	SLA+W transmitted ACK received
	 *  0x20   	SLA+W transmitted NACK received
	 *  0x28   	data byte sent ACK received
	 *  0x30   	data byte sent NACK received
	 *  0x38   	Arbitration in SLA+W lost
	 * 	0x40	TW_MR_SLA_ACK
	 *  0x50
	 * 	0x58
	 */
	 
	uint8_t status;
	//mask status
	status = TWSR & 0xF8;
	return status;
}
