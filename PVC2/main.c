/* main.c
 * Author: Waldemar Barczyk
 *   Data: 2019.01.03
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


uint8_t input;
uint8_t dioda_flag;

volatile uint8_t Timer1;

void main_event_ir ( uint8_t address, uint8_t command,uint8_t key_time );
void set_input( uint8_t input );
void input_press( void );
void pokazuj_date_czas( TDATETIME * dt );


void register_dioda_event_callback( void (*callback)(void) );
void DIODA_EVENT( void );
void dioda( void );

void ustawienie_TIMER0_w_tryb_CTC (void);
void ustawienie_diod_LED (void);
void ustawienie_diody_LED_wyswietlacza (void);
void inicjalizacja_wyswietlacza (void);
void ustawienie_szybkosci_I2C (void);
void inicjalizacja_odbiornika_IR (void);
void inicjalizacja_RTC (void);
void ustawienia_volume (void);
void odblokowanie_przerwan (void);
void ustawienie_czasu_i_daty (void);


uint8_t licznik_TIMER0;
uint8_t licznik_RTC;
void tekst_TIMER0 (void); 	//napis demonstarcyjny na wyswietlaczu wyzwalany Timerem procesora


int main( void )
{
	// funkcje startowe przeniesione do common.c dla poprawienia czytelnoœci i dla cwiczenia
	ustawienie_TIMER0_w_tryb_CTC ();
	ustawienie_diod_LED ();
	ustawienie_diody_LED_wyswietlacza ();
	inicjalizacja_wyswietlacza ();
	ustawienie_szybkosci_I2C ();
	inicjalizacja_odbiornika_IR ();
	inicjalizacja_RTC ();

	// --------------------rejestracja zdarzeñ----------------------
	register_ir_event_callback( main_event_ir );
	register_rtc_event_callback( pokazuj_date_czas );
	register_dioda_event_callback( dioda );

	set_input(0);
	ustawienia_volume ();	// ustawianie -90db przy w³¹czaniu zasilania - pierwszy element tablicy kroków
	odblokowanie_przerwan ();
	ustawienie_czasu_i_daty ();


	while(1)
	{
    	IR_EVENT();
    	RTC_EVENT();
    	DIODA_EVENT();
    }

}



// *******************************************************************************************************************************************
void main_event_ir (uint8_t address, uint8_t command,uint8_t key_time)			// obs³uga klawiszy
{
	// vol+
	if ( (command == 16 && !key_time) || (command == 16 && key_time>4 && 0==key_time%3) ) { vol++; set_vol(); }

	// vol-
	if ( (command == 17 && !key_time) || (command == 17 && key_time>4 && 0==key_time%3) ) { vol--; set_vol(); }

	// input+
	if ( (command == 32 && !key_time) || (command == 32 && 0==key_time%3) ) { input++; input_press();	}

	// input-
	if ( (command == 33 && !key_time) || (command == 33 && 0==key_time%3) ) { input--; input_press(); }

	//	mute
	if ( (command == 13 && !key_time) || (command == 13 && key_time>4 && 0==key_time%3) ) { vol=0; set_vol(); }

	//	power off
	if ( (command == 12 && !key_time) || (command == 12 && key_time>4 && 0==key_time%3) ) { vol=0; set_vol(); LCD_LED_TOG; }


	if (command == 1 && !key_time) { input=0; input_press(); }
	if (command == 2 && !key_time) { input=1; input_press(); }
	if (command == 3 && !key_time) { input=2; input_press(); }
	if (command == 4 && !key_time) { input=3; input_press(); }
	if (command == 5 && !key_time) { input=4; input_press(); }
	if (command == 6 && !key_time) { input=5; input_press(); }
	if (command == 7 && !key_time) { input=6; input_press(); }
}

void input_press(void)
{
	if(input < 0 ) input = 0;
	if(input > 6 ) input = 0;
	set_input(input);
}

void set_input( uint8_t input )
{
	TWI_write_int( VOL_ADR, 255 );
	TWI_write_int( INPUT_ADR, 0b01111111 );
	_delay_ms( 10 );
	if( input==0 ) { TWI_write_int ( INPUT_ADR, 0b11111110 ); } // Input nr1
	if( input==1 ) { TWI_write_int ( INPUT_ADR, 0b11111101 ); } // Input nr2
	if( input==2 ) { TWI_write_int ( INPUT_ADR, 0b11111011 ); } // Input nr3
	if( input==3 ) { TWI_write_int ( INPUT_ADR, 0b11110111 ); } // Input nr4
	if( input==4 ) { TWI_write_int ( INPUT_ADR, 0b11101111 ); } // Input nr5
	if( input==5 ) { TWI_write_int ( INPUT_ADR, 0b11011111 ); } // Input nr6
	if( input==6 ) { TWI_write_int ( INPUT_ADR, 0b10111111 ); } // Input nr7
	_delay_ms( 10 );
	TWI_write_int( INPUT_ADR, 0b11111111 );
	set_vol();

	lcd_locate	(0,4);
	lcd_int		(input+1);
}



// *******************************************************************************************************************************************
void pokazuj_date_czas( TDATETIME * dt )
{
	lcd_locate	(1,12);
	lcd_str		(dt->time);
	lcd_locate	(0,10);
	lcd_str		(dt->date);
}




// *******************************************************************************************************************************************
/* uruchomiono diodê led równoczeœnie z napisami "." "x" "X" " " */
/* dioda jest sterowana przerwaniem wewnêtrznym i wyzwalana w pêtli g³ównej programu DIODA_EVENT - z podzia³u nie wychodzi równa sekunda */
/* diodê widac wy³acznie na p³ytce prototypowej - poniewa¿ w pre jej nie ma , natomiast napis pojawia siê na wyœwietlaczu bez przerwy */
// *******************************************************************************************************************************************

static void ( *dioda_event_callback )( void ); 						// wskaŸnik do funkcji callback dla zdarzenia DIODA_EVENT


void register_dioda_event_callback( void ( *callback )( void ) ) 	// funkcja do rejestracji funkcji zwrotnej w zdarzeniu DIODA_EVENT
{
	dioda_event_callback = callback;
}

void DIODA_EVENT(void)
{
	if ( dioda_flag ) {
		if ( dioda_event_callback ) ( *dioda_event_callback )();
		tekst_TIMER0(); 			// dodatkowa funkcjonalnoœc tj wyœwietlenie napisu ".xX "
		dioda_flag=0;
	}
}


void dioda (void)					// miganie diod¹ callback
{
	LED2_TOG;
}


ISR( TIMER0_COMP_vect ) 			// cia³o funkcji obs³ugi przerwania Compare Match Timera0
{
	if ( !Timer1 ) {Timer1 = 100; dioda_flag = 1;
#if USE_RTC_INT == 0
	// FLAGA RTC
	int0_flag = 1;
#endif
	}
	Timer1--;
}


void tekst_TIMER0 (void){
	if ( licznik_TIMER0 == 0 )	{ lcd_locate(0,7); lcd_str(".");}
	if ( licznik_TIMER0 == 1 )	{ lcd_locate(0,7); lcd_str("x");}
	if ( licznik_TIMER0 == 2 )	{ lcd_locate(0,7); lcd_str("X");}
	if ( licznik_TIMER0 == 3 )	{ lcd_locate(0,7); lcd_str(" ");}

	licznik_TIMER0++;
	if ( licznik_TIMER0 > 3 ) licznik_TIMER0 = 0;
}

