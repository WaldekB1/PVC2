/*
 * ir_samsung.h
 *
 *  Created on: 2011-08-03
 *      Author: Miros�aw Karda�
 */

// PILOT SAMSUNG  AH59-01778R

#ifndef IR_SAMSUNG_H_
#define IR_SAMSUNG_H_


#if IR_TYP == SAMSUNG


// podajemy warto�� 1 je�li chcemy przemapowa� najwa�niejsze klawisze na standard RC5
// lub warto�� 0 je�li chcemy oryginalne kody SAMSUNG
#define KEYS_REMAP 0

// podajemy warto�� 1 je�li chcemy uzy� virtual Toggle bit lub 0 je�li nie
// parametr nieistotny je�li wybrany typ to RC5
#define VIRTUAL_TOGGLE 1


/* ******* USTAWIAMY WARTO�CI SPECYFICZNE DLA SAMSUNG   *********** */
#define SAMSUNG_HEADER ir_micro_s(4575)		// czas pierwszej charakterystycznej cz�ci nag��wka (header'a)

#define SAMSUNG_BIT_HIGH ir_micro_s(1650)
#define SAMSUNG_BIT_HIGH_MID ir_micro_s(1200)		// �rodek czasu trwania pierwszej po�owy bitu o warto�ci = 1
#define SAMSUNG_BIT_LOW ir_micro_s(489)		// czas trwania pierwszej po�owy bitu o warto�ci = 0
#define SAMSUNG_TOLERANCE ir_micro_s(150)	// toleracja jak� przyjmiemy dla naszego pilota. Je�li b�dzie za du�a
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
#if IR_TYP == SAMSUNG
#if VIRTUAL_TOGGLE == 1
void register_ir_event_callback(void (*callback)(uint8_t address,
				uint8_t command, uint8_t key_time));
#endif

#if VIRTUAL_TOGGLE == 0
void register_ir_event_callback(void (*callback)(uint8_t address, uint8_t command));
#endif
#endif


#endif






#endif /* IR_SAMSUNG_H_ */
