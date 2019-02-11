/*
 * ir_jvc.h
 *
 *  Created on: 2011-08-02
 *      Author: Miros³aw Kardaœ
 */

// PILOT JVC RM-C470

#ifndef IR_JVC_H_
#define IR_JVC_H_

#if IR_TYP == JVC


// podajemy wartoœæ 1 jeœli chcemy przemapowaæ najwa¿niejsze klawisze na standard RC5
// lub wartoœæ 0 jeœli chcemy oryginalne kody JVC
#define KEYS_REMAP 1

// podajemy wartoœæ 1 jeœli chcemy uzyæ virtual Toggle bit lub 0 jeœli nie
// parametr nieistotny jeœli wybrany typ to RC5
#define VIRTUAL_TOGGLE 0


/* ******* USTAWIAMY WARTOŒCI SPECYFICZNE DLA JVC   *********** */
#define JVC_HEADER ir_micro_s(8500)		// czas pierwszej charakterystycznej czêœci nag³ówka (header'a)

#define JVC_BIT_HIGH ir_micro_s(1600)
#define JVC_BIT_HIGH_MID ir_micro_s(1200)		// œrodek czasu trwania pierwszej po³owy bitu o wartoœci = 1
#define JVC_BIT_LOW ir_micro_s(500)		// czas trwania pierwszej po³owy bitu o wartoœci = 0
#define JVC_TOLERANCE ir_micro_s(250)	// toleracja jak¹ przyjmiemy dla naszego pilota. Jeœli bêdzie za du¿a
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

void register_ir_event_callback(void (*callback)(uint8_t address, uint8_t command));

#endif



#endif /* IR_JVC_H_ */
