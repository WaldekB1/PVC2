/*
 * ir_jvc.c
 *
 *  Created on: 2011-08-02
 *      Author: Miros�aw Karda�
 */

#include <avr/io.h>			// podci�gni�cie plik�w nag��wkowych
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ir_config.h"
#include "ir_jvc.h"

//#include "../LCD/lcd44780.h"



#if IR_TYP == JVC

// zmienne o dost�pie globalnym - ich deklaracje znajduj� si� w ir_sony.h
volatile uint8_t address;
volatile uint8_t command;
volatile uint8_t Ir_key_press_flag;

//--------------------------------



// wska�nik do funkcji callback dla zdarzenia IR_EVENT
static void (*ir_event_callback)(uint8_t address, uint8_t command);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu IR_EVENT()
void register_ir_event_callback(void (*callback)(uint8_t address, uint8_t command))
{
	ir_event_callback = callback;
}



// Zdarzenie do obs�ugi podczerwieni, nast�puje w nim wykrycie ustawionej flagi
// oraz jej automatyczne kasowanie, ��cznie ze skasowaniem warto�ci command i addres do 0xff
// u�ytkownik nie musi za ka�dym razem pami�ta� o kasowaniu flagi czy wpisywaniu warunku if
void IR_EVENT(void) {

	if( Ir_key_press_flag ) {

		Ir_key_press_flag=0;

		// wywo�anie w�asnej funkcji obs�ugi u�ytkownika je�li
		// uprzednio zosta�a ona zarejestrowana


		if(ir_event_callback) (*ir_event_callback)(address, command);


		address=0xff;
		command=0xff;

	}
}

/* *******************  INICJALIZACJA OBS�UGI PODCZERWIENI  ************ */
void ir_init(void) {
	/* inicjalizacja IR */
//	IR_DIR &= ~IR_PIN;		// pin IR jako wej�cie, poniewa� jest domy�lnie to pomijamy
	IR_PORT |= IR_PIN;		// podci�gni�cie wej�cia IR do VCC

	// KONFIGURACJA PRACY PRZERWANIA ICP I TIMERA1
	TCCR1B |= (1<<CS11);	// ustawienie preskalera dla Timer1 = 8
	TCCR1B &= ~(1<<ICES1);	// reakcja na zbocze opadaj�ce
	TIMSK |= (1<<TICIE1);	// odblokowanie przerwania ICP
}




/* ************** OBS�UGA PRZERWANIA ICP DLA TIMER1 *************  */
ISR(TIMER1_CAPT_vect) {

		// zmienne na potrzeby tylko przerwania
	  static uint16_t LastCapture;
	  uint16_t PulseWidth;
	  static uint8_t IrPulseCount;
	  static uint16_t IrData;
	  static uint8_t step;
	  static uint8_t frame_status;


	  // ci�g�e obliczanie d�ugo�ci impulsu
	  PulseWidth = ICR1 - LastCapture;
	  LastCapture = ICR1;


	  // cykliczna zmiana zbocza wykrywanego sygna�u nadlatuj�cej ramki
	  TCCR1B ^= (1<<ICES1);


	  // je�li nadlecia� nag��wek (header) JVC - to sygna� do rozpocz�cia dekodowania ramki
	  if( PulseWidth > (uint16_t)JVC_HEADER-JVC_TOLERANCE && PulseWidth < (uint16_t)JVC_HEADER+JVC_TOLERANCE ) step = 1;

	  // je�li ju� wykryty HEADER (step>0) to status OK dla ramki
	  if (step > 2) frame_status = FRAME_OK;

	  // inicjalizacja ramki
		if (step == 1)
		{
			IrData = 0;
			IrPulseCount = 0;
			step++;
			frame_status = FRAME_END;	// zako�cz przerwanie
		}
		else if( step == 2 ) {

			step=3;
		}

		// je�li status ramki OK
		if (frame_status == FRAME_OK) {

			// sprawdzenie czy nie s� to jakie� sygna�y z obcego pilota albo zak��cenia
			if( PulseWidth > (uint16_t)JVC_BIT_HIGH+JVC_TOLERANCE ) frame_status = FRAME_RESTART;
			if( PulseWidth < (uint16_t)JVC_BIT_LOW-JVC_TOLERANCE ) frame_status = FRAME_RESTART;

			// je�li ramka ok
			if( frame_status == FRAME_OK ) {

				// sprawdzaj pierwsz� po��wk� ka�dego bitu
				if( (step%2)==0 ) {
					  // przesu� zawarto�� odebranej paczki w prawo (bity nadchodz� od MSB (najstarszego)
				      IrData = IrData >> 1;

				      // je�li po��wka wskazuje na bit=1 to wstaw jedynk� w miejsce odebranego bitu
				      if (PulseWidth > JVC_BIT_HIGH_MID ) IrData = IrData | 0x8000;

				      // zwi�ksz licznik odebranych bit�w
				      IrPulseCount++;
				      if (IrPulseCount == 16) { 	// je�li odebrano 16 bit�w (0 do 15)
				    	  address = IrData & 0b0000000011111111;	// ustal warto�� command
				      	  command = IrData >> 8;							// ustal warto�� address


				      	#if KEYS_REMAP == 1	// je�li w��czone remapowanie kod�w na standard RC5
				      					      		// zmiana adresu TV = 1 na adres = 0 [a tak�e adresu 3 na 0 (niekt�re klawisze TV do translacji maj� adres=3) ] taki jak w standardzie RC5
				      					      		 if (address == 3) address = 0;

				      					      		 // przemapowanie kod�w klawiszy numerycznych JVC 0-9 na
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


					      // ustaw FLAG� odbioru kompletnej ramki podczewieni dla p�tli g��wnej
					      Ir_key_press_flag = 1;
					      frame_status = FRAME_RESTART;			// wykonaj restart ramki
					      //TCCR1B &= ~(1<<ICES1);  // mo�e by� konieczne dla innej ilo�ci bit�w
				      }
				}
				step++;
			}
		}

		// restart ramki czyli ustawienie pierwszego kroku - wykrywanie nag��wka (header'a)
		if (frame_status == FRAME_RESTART) {
			step = 0;
		}

// koniec obs�ugi przerwania ICP - Timer1
}


#endif /* IR_TYP = JVC end */

