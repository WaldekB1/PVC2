/* rtc_pcf8583.c
 * Author: Waldemar Barczyk
 *   Data: 2018.12.15
 *   Na podstawie instrukcji Miros³awa Kardasia
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../I2C_TWI/i2c_twi.h"
#include "rtc_pcf8583.h"
#include "../common.h"


volatile uint8_t s1_cnt;
volatile uint8_t int0_flag;

TDATETIME datetime;

static void (*rtc_callback)( TDATETIME *dt );							// WskaŸnik do funkcji callback dla zdarzenia RTC_EVENT()

void register_rtc_event_callback ( void (*callback)( TDATETIME *dt ) ) 	// funkcja do rejestracji funkcji zwrotnej w zdarzeniu RTC_EVENT()
{
	rtc_callback = callback;
}

void init_rtc ( void )
{
#if USE_RTC_INT == 1
	// Przerwanie INT0
	MCUCR |= (1<<ISC01);	// wyzwalanie zboczem opadaj¹cym
	GICR |= (1<<INT0);		// odblokowanie przerwania
	PORTD |= (1<<PD2);		// podci¹gniêcie pinu INT0 do VCC
#endif
}

void set_rtc_time ( TDATETIME *dt, uint8_t hh, uint8_t mm, uint8_t ss )
{
	dt->pcf_buf[0] = dec2bcd(ss);
	dt->pcf_buf[1] = dec2bcd(mm);
	dt->pcf_buf[2] = dec2bcd(hh);
	TWI_write_buf( PCF8583_ADDR, 0x02, 3, dt->pcf_buf );
}

void set_rtc_date ( TDATETIME *dt, uint16_t YY, uint8_t MM, uint8_t DD )
{
	dt->pcf_buf[3] = dec2bcd( DD ) | ( ( YY & 0x03 )<<6 );
	dt->pcf_buf[4] = dec2bcd( MM ) ;
	TWI_write_buf( PCF8583_ADDR, 0x05, 2, &dt->pcf_buf[3]);
	TWI_write_buf( PCF8583_ADDR, 0x10, 2, (uint8_t*)&YY );
}

void set_rtc_datetime ( TDATETIME *dt, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss )
{
	set_rtc_date ( dt, YY, MM, DD );
	set_rtc_time ( dt, hh, mm, ss );
}

void RTC_EVENT ( void )
{
	if (int0_flag)
	{
		get_rtc_datetime ( &datetime );
		if( rtc_callback ) rtc_callback (&datetime);
		int0_flag=0;
	}
}

void get_rtc_datetime ( TDATETIME *dt )
{
	TWI_read_buf ( PCF8583_ADDR, 0x02, 5, dt->pcf_buf );
	TWI_read_buf ( PCF8583_ADDR, 16, 2, (uint8_t*)&dt->YY );

	int8_t i;
	uint8_t * wsk = dt->pcf_buf;
		char * znak = dt->time;

	for (i=2; i>-1; i--)
	{
		*(znak++) = ( (*(wsk+i) & (2==i? 0x3F : 0x7F) ) >> 4 ) + '0';
		*(znak++) = ( *(wsk+i) & 0x0F) +'0';
		*(znak++) = i? ':': 0;
		*( (uint8_t*)dt +2-i) = bcd2dec( *(wsk + i) );
	}

	dt->DD = bcd2dec( *(wsk+3) & 0x3F );
	uint8_t yr = ( *(wsk+3) >>6 );


	dt->MM = bcd2dec( *(wsk+4) & 0x1F );
	dt->weekday = ( *(wsk+4) >> 5 );


	znak = dt->date;
	if (dt->YY < 10) 	*(znak++ ) = '0';
	if (dt->YY < 100) 	*(znak++ ) = '0';
	if (dt->YY < 1000) 	*(znak++ ) = '0';
	itoa( dt->YY, znak, 10);

	znak = dt->date;
	znak += 4;

	*(znak++) = DATE_SEPARATOR;
	*(znak++) = ( (wsk[4] &0x1F ) >> 4) + '0';
	*(znak++) = ( (wsk[4] &0x0F ) ) + '0';
	*(znak++) = DATE_SEPARATOR;
	*(znak++) = ( (wsk[3] &0x1F ) >> 4) + '0';
	*(znak++) = ( (wsk[3] &0x0F ) ) + '0';
	*znak = 0;
}


uint8_t dec2bcd( uint8_t dec )					// konwersja procedura  dziesiêtnej na BCD
{
	return( (dec/10) << 4 ) | (dec % 10);
}

uint8_t bcd2dec( uint8_t bcd )					// konwersja liczby BCD na dziesiêtn¹
{
	return( (( (bcd) >> 4 ) & 0x0f ) *10 ) + ( (bcd) & 0x0F );
}

//ISR(INT0_vect)									// procedura obs³ugi przerwania INT0
//{
//	int0_flag = 1;
//	if (++s1_cnt==2)s1_cnt=0;
//	 LED_TOG;
//}

#if USE_RTC_INT == 1
// procedura obs³ugi przerwania INT0
ISR(INT0_vect)
{
	int0_flag = 1;


	if (++s1_cnt==2)s1_cnt=0;

	 LED_TOG;

//	PORTC ^= (1<<PC6);
}
#endif
