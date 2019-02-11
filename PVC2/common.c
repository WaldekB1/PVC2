/* main.c
 * Author: Waldemar Barczyk
 *   Data: 2019.02.03
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdbool.h>

#include "LCD/lcd44780.h"
#include "IR_UNI/ir_config.h"
#include "I2C_TWI/i2c_twi.h"
#include "PVC/pvc.h"
#include "RTC_PCF8583/rtc_pcf8583.h"
#include "common.h"

void ustawienie_TIMER0_w_tryb_CTC (void)
{
	TCCR0 |= ( 1<<WGM01 );						// tryb  CTC
	TCCR0 |= ( 1 << CS02 ) | ( 1 << CS00 );		// preskaler = 1024
	OCR0   = F_CPU/1024/100UL;					// przerwanie porównania co ok 8ms (120Hz) dla 8MHz
	TIMSK |= ( 1<<OCIE0 );						// zezwolenie na przerwanie CompareMatch
}

void ustawienie_diod_LED (void)
{
	LED_DDR  |= LED;
	LED2_DDR |= LED2;
}

void ustawienie_diody_LED_wyswietlacza (void)
{
	LCD_LED_DDR |= LCD_LED;
	LCD_LED_ON;
}

void inicjalizacja_wyswietlacza (void)
{
	lcd_init();
	lcd_cls();
	lcd_locate(0,0);
	lcd_str("IN:");
	lcd_locate(1,0);
	lcd_str("VOL:");
}

void ustawienie_szybkosci_I2C (void)
{
	i2cSetBitrate( 100 );
}

void inicjalizacja_odbiornika_IR (void)
{
	ir_init();
}

void inicjalizacja_RTC (void)
{
	init_rtc ();
}

void ustawienia_volume (void)
{
		set_vol();
}

void odblokowanie_przerwan (void)
{
	sei();
}


void ustawienie_czasu_i_daty (void)
{
	// -----------------Sposób 1 >> Ustawianie osobno czasu i daty--------------
	//	set_rtc_date (&datetime, 2019, 1, 3);
	// set_rtc_time (&datetime, 15, 15, 30);

	// ---------------Sposób 2 >> ustawianie razem czasu i daty-------------
//			set_rtc_datetime (&datetime, 2019, 2, 8, 22, 14, 40);
}

