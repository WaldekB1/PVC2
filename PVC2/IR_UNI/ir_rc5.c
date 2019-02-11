/*
 * ir_rc5.c
 *
 *  Created on: 2011-08-02
 *      Author: Miros³aw Kardaœ
 */
#include <avr/io.h>			// podci¹gniêcie plików nag³ówkowych
#include <avr/interrupt.h>

#include "ir_config.h"
#include "ir_rc5.h"



#if IR_TYP == RC5
volatile uint8_t rc5cnt;
volatile uint8_t last_toggle;
volatile uint8_t toggle_bit;

volatile uint8_t address;
volatile uint8_t command;
volatile uint8_t Ir_key_press_flag;
volatile uint8_t key_time;			// iloœæ powtórzeñ ramki przy wci¹¿ wciœniêtym klawiszu - autorepeat


// wskaŸnik do funkcji callback dla zdarzenia IR_EVENT
static void (*ir_event_callback)(uint8_t address,
		uint8_t command, uint8_t key_time);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu IR_EVENT()
void register_ir_event_callback(void (*callback)(uint8_t address,
				uint8_t command, uint8_t key_time))
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

		if(ir_event_callback) (*ir_event_callback)(address, command, key_time);


		address=0xff;
		command=0xff;

	}
}

/* *******************  INICJALIZACJA OBS£UGI PODCZERWIENI  ************ */
void ir_init(void) {
	/* inicjalizacja IR */
//	IR_DIR  &= ~IR_PIN;		// pin IR jako wejœcie, poniewa¿ jest domyœlnie to pomijamy
	IR_PORT |= IR_PIN;		// podci¹gniêcie wejœcia IR do VCC

	// KONFIGURACJA PRACY PRZERWANIA ICP I TIMERA1
	TCCR1B |= (1<<CS11);	// ustawienie preskalera dla Timer1 = 8
	TCCR1B &= ~(1<<ICES1);	// reakcja na zbocze opadaj¹ce
	TIMSK  |= (1<<TICIE1);	// odblokowanie przerwania ICP

}



ISR(TIMER1_CAPT_vect) {

	#define FRAME_RESTART 0
	#define FRAME_OK 1
	#define FRAME_END 2
	#define FRAME_ERROR 3


  	static uint16_t LastCapture;
  	uint16_t PulseWidth;
  	static uint8_t IrPulseCount;
  	static uint16_t IrData;
	static uint8_t frame_status;


  	PulseWidth = ICR1 - LastCapture;
  	LastCapture = ICR1;

	TCCR1B ^= (1<<ICES1);			// zmiana zbocza wyzwalaj¹cego na przeciwne

	if (PulseWidth > MAX_BIT) rc5cnt = 0;

	if (rc5cnt > 0) frame_status = FRAME_OK;

	if (rc5cnt == 0)
	{
		IrData = 0;
		IrPulseCount = 0;
		TCCR1B |= (1<<ICES1);
		rc5cnt++;
		frame_status = FRAME_END;
	}

	if (frame_status == FRAME_OK)
	{
		if ( PulseWidth < MIN_HALF_BIT ) frame_status = FRAME_RESTART; 	// gdy zak³ócenia (szpilki) - RESTART
		if ( PulseWidth > MAX_BIT ) frame_status = FRAME_RESTART; 		// gdy b³¹d ramki danych (mo¿e inny standard ni¿ RC5) RESTART

		if (frame_status == FRAME_OK)
		{
			if (PulseWidth > MAX_HALF_BIT) rc5cnt++;

			if (rc5cnt > 1)
			if ( (rc5cnt % 2) == 0 )
			{
				IrData = IrData << 1;
				if ( (TCCR1B & (1<<ICES1)) ) IrData |= 0x0001;
				IrPulseCount++;

				if (IrPulseCount > 12)
				{
					if (Ir_key_press_flag == 0)
					{
						command 	= IrData & 0b0000000000111111;
						address 	= (IrData & 0b0000011111000000) >> 6;
						toggle_bit 	= (IrData & 0b0000100000000000) >> 11;
						if (toggle_bit == last_toggle) key_time++;
						else key_time = 0;

						last_toggle = toggle_bit;
					}
					frame_status = FRAME_RESTART;
					Ir_key_press_flag = 1;
				}
			}
			rc5cnt++;
		}
	}

	if (frame_status == FRAME_RESTART)
	{
		rc5cnt = 0;
		TCCR1B &= ~(1<<ICES1);
	}

}
#endif /* IR_TYP = RC */
