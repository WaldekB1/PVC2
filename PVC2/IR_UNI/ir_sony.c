/*
 * ir_sony.c
 *
 *  Created on: 2011-08-01
 *      Author: Miros³aw Kardaœ
 */
#include <avr/io.h>			// podci¹gniêcie plików nag³ówkowych
#include <avr/interrupt.h>

#include "ir_config.h"
#include "ir_sony.h"


#if IR_TYP == SONY

// zmienne o dostêpie globalnym - ich deklaracje znajduj¹ siê w ir_sony.h
volatile uint8_t address;
volatile uint8_t command;
volatile uint8_t Ir_key_press_flag;

volatile uint8_t step;

#if VIRTUAL_TOGGLE == 1
volatile uint8_t key_time;			// iloœæ powtórzeñ ramki przy wci¹¿ wciœniêtym klawiszu - repeat
volatile uint8_t virtual_toggle;	// virtual toggle bit
#endif

//--------------------------------

#if VIRTUAL_TOGGLE == 1
// wskaŸnik do funkcji callback dla zdarzenia IR_EVENT
static void (*ir_event_callback)(uint8_t address,
		uint8_t command, uint8_t key_time);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu IR_EVENT()
void register_ir_event_callback(void (*callback)(uint8_t address,
				uint8_t command, uint8_t key_time))
{
	ir_event_callback = callback;
}
#endif

#if VIRTUAL_TOGGLE == 0
// wskaŸnik do funkcji callback dla zdarzenia IR_EVENT
static void (*ir_event_callback)(uint8_t address, uint8_t command);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu IR_EVENT()
void register_ir_event_callback(void (*callback)(uint8_t address, uint8_t command))
{
	ir_event_callback = callback;
}
#endif


// Zdarzenie do obs³ugi podczerwieni, nastêpuje w nim wykrycie ustawionej flagi
// oraz jej automatyczne kasowanie, ³¹cznie ze skasowaniem wartoœci command i addres do 0xff
// u¿ytkownik nie musi za ka¿dym razem pamiêtaæ o kasowaniu flagi czy wpisywaniu warunku if
void IR_EVENT(void) {

	if( Ir_key_press_flag ) {

		Ir_key_press_flag=0;

		// wywo³anie w³asnej funkcji obs³ugi u¿ytkownika jeœli
		// uprzednio zosta³a ona zarejestrowana
#if VIRTUAL_TOGGLE == 1
		if(ir_event_callback) (*ir_event_callback)(address, command, key_time);
#endif

#if VIRTUAL_TOGGLE == 0
		if(ir_event_callback) (*ir_event_callback)(address, command);
#endif

		address=0xff;
		command=0xff;

	}
}

/* *******************  INICJALIZACJA OBS£UGI PODCZERWIENI  ************ */
void ir_init(void) {
	/* inicjalizacja IR */
//	IR_DIR &= ~IR_PIN;		// pin IR jako wejœcie, poniewa¿ jest domyœlnie to pomijamy
	IR_PORT |= IR_PIN;		// podci¹gniêcie wejœcia IR do VCC

	// KONFIGURACJA PRACY PRZERWANIA ICP I TIMERA1
	TCCR1B |= (1<<CS11);	// ustawienie preskalera dla Timer1 = 8
	TCCR1B &= ~(1<<ICES1);	// reakcja na zbocze opadaj¹ce
	TIMSK |= (1<<TICIE1);	// odblokowanie przerwania ICP

	// jeœli korzystamy z mechanizmu Virtual Toggle Bit
#if VIRTUAL_TOGGLE == 1
	TIMSK |= (1<<TOIE1);	// odblokowanie przerwania Timer1 Overflow
#endif
}


#if VIRTUAL_TOGGLE == 1
// obs³uga przerwania przepe³nienia dla Timer1
ISR( TIMER1_OVF_vect) {
	step=0;					// wyzeruj krok
	virtual_toggle ^= 1;	// zmieñ stan TOGGLE
	TIMSK &= ~(1<<TOIE1);	// zablokuj przerwanie OVF
}
#endif

ISR(TIMER1_CAPT_vect) {

	static uint8_t IrPulseCount;
	static uint16_t IrData;
	static uint8_t step, start;
	static uint8_t frame_status;

#if VIRTUAL_TOGGLE == 1
	static uint8_t last_vbit;

	TIFR |= (1<<TOV1);	// kasuj flagê przerwania
	TIMSK |= (1<<TOIE1);// odblokuj przerwanie
#endif

	TCCR1B ^= (1<<ICES1);	// zmieñ zbocze na przeciwne

	// jeœli ju¿ wykryty HEADER (step>0) to status OK dla ramki
	if (start == 1 && step == 2) frame_status = FRAME_OK;

	// inicjalizacja ramki
	if (step == 1 && TCNT1 > (uint16_t)SONY_HEADER-SONY_TOLERANCE && TCNT1 < (uint16_t)SONY_HEADER+SONY_TOLERANCE ) {
		IrData = 0;
		IrPulseCount = 0;
		frame_status = FRAME_END;	// zakoñcz przerwanie
		start=1;
	}

	// jeœli status ramki OK
	if (frame_status == FRAME_OK) {

		// sprawdzenie czy nie s¹ to jakieœ sygna³y z obcego pilota albo zak³ócenia
		if( TCNT1 > (uint16_t)SONY_BIT_HIGH+SONY_TOLERANCE ) frame_status = FRAME_RESTART;
		if( TCNT1 < (uint16_t)SONY_BIT_LOW-SONY_TOLERANCE ) frame_status = FRAME_RESTART;

		// jeœli ramka ok
		if( frame_status == FRAME_OK ) {

			// sprawdzaj pierwsz¹ po³ówkê ka¿dego bitu
			if( (step%2)==1 ) {

				  // przesuñ zawartoœæ odebranej paczki w prawo (bity nadchodz¹ od MSB (najstarszego)
			      IrData = IrData >> 1;

			      // jeœli po³ówka wskazuje na bit=1 to wstaw jedynkê w miejsce odebranego bitu
			      if (TCNT1 > (uint16_t)SONY_BIT_HIGH_MID) IrData = IrData | 0x8000;

			      // zwiêksz licznik odebranych bitów
			      IrPulseCount++;
			      if (IrPulseCount == 12) { 	// jeœli odebrano 12 bitów
			    	  command = ((IrData >> 4) & 0b0000000001111111);	// ustal wartoœæ command
			      	  address = (IrData >> 11);							// ustal wartoœæ address

#if KEYS_REMAP == 1	// jeœli w³¹czone remapowanie kodów na standard RC5
			      		// zmiana adresu TV = 1 na adres = 0 [a tak¿e adresu 3 na 0 (niektóre klawisze TV do translacji maj¹ adres=3) ] taki jak w standardzie RC5
			      		 if (address == 1 || address == 3) address = 0;

			      		 // przemapowanie kodów klawiszy numerycznych SONY 0-9 na
			      		 // kody klawiszy numerycznych RC5 0->1, 1->2 .... 8->9 oraz 9->0
			      		 // SONY klawisz "1" = 0 natomiast w RC5 klawisz "1" = 1
			      		 // SONY klawisz "2" = 1 natomiast w RC5 klawisz "2" = 2
			      		if ( (command >= 0) && (command <= 8) ) command++;
			      		else
			      		if (command == 9) command = 0;
			      		else
			      		if (command == 18) command = 16;	// vol +
			      		else
			      		if (command == 19) command = 17;	// vol -
			      		else
			      		if (command == 16) command = 32;	// pr +
			      		else
			      		if (command == 17) command = 33;	// pr -
			      		else
			      		if (command == 21) command = 12;	// PWR
			      		else
			      		if (command == 96) command = 36;	// MENU
			      		else
			      		if (command == 20) command = 13;	// MUTE
			      		else
			      		if (command == 29) command = 10;	// -/--
			      		else
			      		if (command == 101) command = 59;	// EXECUTE - button_ok
			      		else
			      		if (command == 76) command = 55;	// Teletext RED
			      		else
			      		if (command == 77) command = 54;	// Teletext GREEN
			      		else
			      		if (command == 78) command = 50;	// Teletext YELLOW
			      		else
			      		if (command == 79) command = 52;	// Teletext BLUE
			      		else
			      		if (command == 52) command = 17;	// Cursor LEFT
			      		else
			      		if (command == 51) command = 16;	// Cursor RIGHT
			      		else
			      		if (command == 116) command = 32;	// Cursor UP
			      		else
			      		if (command == 117) command = 33;	// Cursor DOWN
#endif


#if VIRTUAL_TOGGLE == 1		// jeœli w³¹czona obs³uga Virtual Toggle bit

				      if( last_vbit == virtual_toggle ) key_time++;
				      else key_time=0;						// ustal wartoœæ key_time

				      last_vbit = virtual_toggle;			// zapamiêtaj ostatni stan VirtualBitu
#endif
				      // ustaw FLAGÊ odbioru kompletnej ramki podczewieni dla pêtli g³ównej
				      Ir_key_press_flag = 1;
				      frame_status = FRAME_RESTART;			// wykonaj restart ramki
			      }
			}
		}
	}
	// restart ramki czyli ustawienie pierwszego kroku - wykrywanie nag³ówka (header'a)
	if (frame_status == FRAME_RESTART) {
		step = 0;
		start = 0;
	}

	step++;
	TCNT1 = 0;

}

#endif









