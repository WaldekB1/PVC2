/* rtc_pcf8583.c
 * Author: Waldemar Barczyk
 *   Data: 2018.12.15
 *   Modyfikacja biblioteki Piotra Warysza
 */


#include "pvc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#include "../LCD/lcd44780.h"
#include "../I2C_TWI/i2c_twi.h"

#include "../common.h"

int8_t vol;
void vol_display( void );
void get_vol_db( uint16_t vol );
char vol_buf[8];

const uint16_t pvc_vol[24][2] PROGMEM =
{
		{255,900},
		{254,524},
		{253,476},
		{252,437},
		{251,420},
		{249,398},
		{248,368},
		{247,352},
		{246,343},
		{243,320},
		{241,300},
		{238,272},
		{231,246},
		{224,227},
		{223,200},
		{208,178},
		{192,155},
		{191,125},
		{160,100},
		{128,78},
		{127,45},
		{87,30},
		{59,15},
		{0,0},
};


void set_vol( void ) 			//regulacja g³oœnoœci
{
	if ( vol < 1 ) vol = 0;
	if ( vol > 23 ) vol = 23;
	uint8_t temp = 0;
	temp = pgm_read_byte( &pvc_vol[vol][0] );
	TWI_write_int( VOL_ADR, temp );
	vol_display();
}

void vol_display( void )
{
	uint16_t vol1 = 0;
	vol1=pgm_read_word( &pvc_vol[vol][1] );
		get_vol_db( vol1 );
}

void get_vol_db( uint16_t vol )
{
	uint16_t dig[2];

	dig[0] = vol/100;
	dig[1] = 0;
	dig[2] = 0;
	dig[1] = ( (vol-dig[0]*100)/10 );
	dig[2] = (vol)-100*dig[0]-10*dig[1];

	if ( !dig[0] && !dig[1] && !dig[2] ) sprintf (vol_buf, "    %ddB",  dig[2] );
	else if 	  ( !dig[0] && !dig[1] ) sprintf (vol_buf, " -0.%ddB",  dig[2] );
	else if 			     ( !dig[0] ) sprintf (vol_buf, " -%d.%ddB", dig[1], dig[2] );
	else 								 sprintf (vol_buf, "-%d%d.%ddB",dig[0], dig[1], dig[2] );
	lcd_locate	(1, 4);
	lcd_str 	("        ");
	lcd_locate	(1, 4);
	lcd_str 	(vol_buf);
}
