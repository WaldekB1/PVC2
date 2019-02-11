/*
 * ir_sony.h
 *
 *  Created on: 2011-08-01
 *      Author: Miros³aw Kardaœ
 */

#ifndef IR_SONY_H_
#define IR_SONY_H_

#if IR_TYP == SONY
/* *************************           K O N F I G U R A C J A              ********* */
/* *************************   U S T A W I E N I A   U ¯ Y T K O W N I K A  ********* */

#define IR_PIN (1<<PD6)			// numer pinu wejœcia ICP
#define IR_DIR DDRD
#define IR_PORT PORTD


// podajemy wartoœæ 1 jeœli chcemy uzyæ virtual Toggle bit lub 0 jeœli nie
#define VIRTUAL_TOGGLE 1

// podajemy wartoœæ 1 jeœli chcemy przemapowaæ najwa¿niejsze klawisze na standard RC5
// lub wartoœæ 0 jeœli chcemy oryginalne kody SONY
#define KEYS_REMAP 0

// makro przeliczaj¹ce czasy w us w zale¿noœci od F_CPU (automatycznie)
#define ir_micro_s(num) (((num)*((F_CPU/1000UL)/8))/1000)

/* ******* USTAWIAMY WARTOŒCI SPECYFICZNE DLA SONY SIRCS  *********** */
#define SONY_HEADER ir_micro_s(2440)		// czas pierwszej charakterystycznej czêœci nag³ówka (header'a)

#define SONY_BIT_HIGH ir_micro_s(1240)
#define SONY_BIT_HIGH_MID ir_micro_s(925)		// 3/4 czasu trwania pierwszej po³owy bitu o wartoœci = 1
#define SONY_BIT_LOW ir_micro_s(650)		// czas trwania pierwszej po³owy bitu o wartoœci = 0
#define SONY_TOLERANCE ir_micro_s(150)	// toleracja jak¹ przyjmiemy dla naszego pilota. Jeœli bêdzie za du¿a
											// to procedura mo¿e zacz¹æ odbieraæ niechc¹co czasem dziwne kody
											// z pilotów innych producentów. Jeœli bêdzie za ma³a to mo¿e siê
											// zmniejszyæ zasiêg pilota albo bêdzie gorzej dzia³a³ z odbicia np od œcian itp
											// optymalne wartoœci zwykle zawieraj¹ siê w granicach 100-250

/* ************************* K O N I E C   U S T A W I E Ñ   U ¯ Y T K O W N I K A  ********* */

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
