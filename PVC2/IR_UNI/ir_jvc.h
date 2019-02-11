/*
 * ir_jvc.h
 *
 *  Created on: 2011-08-02
 *      Author: Miros�aw Karda�
 */

// PILOT JVC RM-C470

#ifndef IR_JVC_H_
#define IR_JVC_H_

#if IR_TYP == JVC


// podajemy warto�� 1 je�li chcemy przemapowa� najwa�niejsze klawisze na standard RC5
// lub warto�� 0 je�li chcemy oryginalne kody JVC
#define KEYS_REMAP 1

// podajemy warto�� 1 je�li chcemy uzy� virtual Toggle bit lub 0 je�li nie
// parametr nieistotny je�li wybrany typ to RC5
#define VIRTUAL_TOGGLE 0


/* ******* USTAWIAMY WARTO�CI SPECYFICZNE DLA JVC   *********** */
#define JVC_HEADER ir_micro_s(8500)		// czas pierwszej charakterystycznej cz�ci nag��wka (header'a)

#define JVC_BIT_HIGH ir_micro_s(1600)
#define JVC_BIT_HIGH_MID ir_micro_s(1200)		// �rodek czasu trwania pierwszej po�owy bitu o warto�ci = 1
#define JVC_BIT_LOW ir_micro_s(500)		// czas trwania pierwszej po�owy bitu o warto�ci = 0
#define JVC_TOLERANCE ir_micro_s(250)	// toleracja jak� przyjmiemy dla naszego pilota. Je�li b�dzie za du�a
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

void register_ir_event_callback(void (*callback)(uint8_t address, uint8_t command));

#endif



#endif /* IR_JVC_H_ */
