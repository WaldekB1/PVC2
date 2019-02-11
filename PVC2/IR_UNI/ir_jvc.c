/*
 * ir_jvc.c
 *
 *  Created on: 2011-08-02
 *      Author: Miros³aw Kardaœ
 */

#include <avr/io.h>			// podci¹gniêcie plików nag³ówkowych
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ir_config.h"
#include "ir_jvc.h"

//#include "../LCD/lcd44780.h"



#if IR_TYP == JVC

// zmienne o dostêpie globalnym - ich deklaracje znajduj¹ siê w ir_sony.h
volatile uint8_t address;
volatile uint8_t command;
volatile uint8_t Ir_key_press_flag;

//--------------------------------



// wskaŸnik do funkcji callback dla zdarzenia IR_EVENT
static void (*ir_event_callback)(uint8_t address, uint8_t command);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu IR_EVENT()
void register_ir_event_callback(void (*callback)(uint8_t address, uint8_t command))
{
	ir_event_callback = callback;
}



// Zdarzenie do obs³ugi podczerwieni, nastêpuje w nim wykrycie ustawionej flagi
// oraz jej automatyczne kasowanie, ³¹cznie ze skasowaniem wartoœci command i addres do 0xff
// u¿ytkownik nie musi za ka¿dym razem pamiêtaæ o kasowaniu flagi czy wpisywaniu warunku if
void IR_EVENT(void) {

	if( Ir_key_press_flag ) {

		Ir_key_press_flag=0;

		// wywo³anie w³asnej funkcji obs³ugi u¿ytkownika jeœli
		// uprzednio zosta³a ona zarejestrowana


		if(ir_event_callback) (*ir_event_callback)(address, command);


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
}




/* ************** OBS£UGA PRZERWANIA ICP DLA TIMER1 *************  */
ISR(TIMER1_CAPT_vect) {

		// zmienne na potrzeby tylko przerwania
	  static uint16_t LastCapture;
	  uint16_t PulseWidth;
	  static uint8_t IrPulseCount;
	  static uint16_t IrData;
	  static uint8_t step;
	  static uint8_t frame_status;


	  // ci¹g³e obliczanie d³ugoœci impulsu
	  PulseWidth = ICR1 - LastCapture;
	  LastCapture = ICR1;


	  // cykliczna zmiana zbocza wykrywanego sygna³u nadlatuj¹cej ramki
	  TCCR1B ^= (1<<ICES1);


	  // jeœli nadlecia³ nag³ówek (header) JVC - to sygna³ do rozpoczêcia dekodowania ramki
	  if( PulseWidth > (uint16_t)JVC_HEADER-JVC_TOLERANCE && PulseWidth < (uint16_t)JVC_HEADER+JVC_TOLERANCE ) step = 1;

	  // jeœli ju¿ wykryty HEADER (step>0) to status OK dla ramki
	  if (step > 2) frame_status = FRAME_OK;

	  // inicjalizacja ramki
		if (step == 1)
		{
			IrData = 0;
			IrPulseCount = 0;
			step++;
			frame_status = FRAME_END;	// zakoñcz przerwanie
		}
		else if( step == 2 ) {

			step=3;
		}

		// jeœli status ramki OK
		if (frame_status == FRAME_OK) {

			// sprawdzenie czy nie s¹ to jakieœ sygna³y z obcego pilota albo zak³ócenia
			if( PulseWidth > (uint16_t)JVC_BIT_HIGH+JVC_TOLERANCE ) frame_status = FRAME_RESTART;
			if( PulseWidth < (uint16_t)JVC_BIT_LOW-JVC_TOLERANCE ) frame_status = FRAME_RESTART;

			// jeœli ramka ok
			if( frame_status == FRAME_OK ) {

				// sprawdzaj pierwsz¹ po³ówkê ka¿dego bitu
				if( (step%2)==0 ) {
					  // przesuñ zawartoœæ odebranej paczki w prawo (bity nadchodz¹ od MSB (najstarszego)
				      IrData = IrData >> 1;

				      // jeœli po³ówka wskazuje na bit=1 to wstaw jedynkê w miejsce odebranego bitu
				      if (PulseWidth > JVC_BIT_HIGH_MID ) IrData = IrData | 0x8000;

				      // zwiêksz licznik odebranych bitów
				      IrPulseCount++;
				      if (IrPulseCount == 16) { 	// jeœli odebrano 16 bitów (0 do 15)
				    	  address = IrData & 0b0000000011111111;	// ustal wartoœæ command
				      	  command = IrData >> 8;							// ustal wartoœæ address


				      	#if KEYS_REMAP == 1	// jeœli w³¹czone remapowanie kodów na standard RC5
				      					      		// zmiana adresu TV = 1 na adres = 0 [a tak¿e adresu 3 na 0 (niektóre klawisze TV do translacji maj¹ adres=3) ] taki jak w standardzie RC5
				      					      		 if (address == 3) address = 0;

				      					      		 // przemapowanie kodów klawiszy numerycznych JVC 0-9 na
				      					      		 // kody klawiszy numerycznych RC5 0->1, 1->2 .... 8->9 oraz 9->0
				      					      		 // JVC klawisz "1" = 0 natomiast w RC5 klawisz "1" = 1
				      					      		 // JVC klawisz "2" = 1 natomiast w RC5 klawisz "2" = 2
				      					      		if ( (command >= 32) && (command <= 41) ) command -= 32;
				      					      		else
				      					      		if (command == 30) command = 16;	// vol +
				      					      		else
				      					      		if (command == 31) command = 17;	// vol -
				      					      		else
				      					      		if (command == 25) command = 32;	// pr +
				      					      		else
				      					      		if (command == 24) command = 33;	// pr -
				      					      		else
				      					      		if (command == 23) command = 12;	// PWR
				      					      		else
				      					      		if (command == 122) command = 36;	// MENU
				      					      		else
				      					      		if (command == 28) command = 13;	// MUTE
				      					      		else
				      					      		if (command == 157) command = 10;	// -/--
				      					      		else
				      					      		if (command == 121) command = 59;	// EXECUTE - button_ok
				      					      		else
				      					      		if (command == 91) command = 17;	// Cursor LEFT
				      					      		else
				      					      		if (command == 90) command = 16;	// Cursor RIGHT
				      					      		else
				      					      		if (command == 124) command = 32;	// Cursor UP
				      					      		else
				      					      		if (command == 123) command = 33;	// Cursor DOWN
				      	#endif


					      // ustaw FLAGÊ odbioru kompletnej ramki podczewieni dla pêtli g³ównej
					      Ir_key_press_flag = 1;
					      frame_status = FRAME_RESTART;			// wykonaj restart ramki
					      //TCCR1B &= ~(1<<ICES1);  // mo¿e byæ konieczne dla innej iloœci bitów
				      }
				}
				step++;
			}
		}

		// restart ramki czyli ustawienie pierwszego kroku - wykrywanie nag³ówka (header'a)
		if (frame_status == FRAME_RESTART) {
			step = 0;
		}

// koniec obs³ugi przerwania ICP - Timer1
}


#endif /* IR_TYP = JVC end */

