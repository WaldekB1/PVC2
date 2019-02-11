/*
 * ir_sony.h
 *
 *  Created on: 2011-08-01
 *      Author: Miros�aw Karda�
 */

#ifndef IR_SONY_H_
#define IR_SONY_H_

#if IR_TYP == SONY
/* *************************           K O N F I G U R A C J A              ********* */
/* *************************   U S T A W I E N I A   U � Y T K O W N I K A  ********* */

#define IR_PIN (1<<PD6)			// numer pinu wej�cia ICP
#define IR_DIR DDRD
#define IR_PORT PORTD


// podajemy warto�� 1 je�li chcemy uzy� virtual Toggle bit lub 0 je�li nie
#define VIRTUAL_TOGGLE 1

// podajemy warto�� 1 je�li chcemy przemapowa� najwa�niejsze klawisze na standard RC5
// lub warto�� 0 je�li chcemy oryginalne kody SONY
#define KEYS_REMAP 0

// makro przeliczaj�ce czasy w us w zale�no�ci od F_CPU (automatycznie)
#define ir_micro_s(num) (((num)*((F_CPU/1000UL)/8))/1000)

/* ******* USTAWIAMY WARTO�CI SPECYFICZNE DLA SONY SIRCS  *********** */
#define SONY_HEADER ir_micro_s(2440)		// czas pierwszej charakterystycznej cz�ci nag��wka (header'a)

#define SONY_BIT_HIGH ir_micro_s(1240)
#define SONY_BIT_HIGH_MID ir_micro_s(925)		// 3/4 czasu trwania pierwszej po�owy bitu o warto�ci = 1
#define SONY_BIT_LOW ir_micro_s(650)		// czas trwania pierwszej po�owy bitu o warto�ci = 0
#define SONY_TOLERANCE ir_micro_s(150)	// toleracja jak� przyjmiemy dla naszego pilota. Je�li b�dzie za du�a
											// to procedura mo�e zacz�� odbiera� niechc�co czasem dziwne kody
											// z pilot�w innych producent�w. Je�li b�dzie za ma�a to mo�e si�
											// zmniejszy� zasi�g pilota albo b�dzie gorzej dzia�a� z odbicia np od �cian itp
											// optymalne warto�ci zwykle zawieraj� si� w granicach 100-250

/* ************************* K O N I E C   U S T A W I E �   U � Y T K O W N I K A  ********* */

#define FRAME_RESTART 0
#define FRAME_OK 1
#define FRAME_END 2


/* *************************************************************************** */



// DEKLARACJE FUNKCJI GLOBALNYCH
void ir_init(void);

void IR_EVENT(void);

#if VIRTUAL_TOGGLE == 1
void register_ir_event_callback(void (*callback)(uint8_t address,
				uint8_t command, uint8_t key_time));
#endif

#if VIRTUAL_TOGGLE == 0
void register_ir_event_callback(void (*callback)(uint8_t address, uint8_t command));
#endif

#endif

#endif /* IR_SONY_H_ */
