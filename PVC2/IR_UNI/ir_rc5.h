/*
 * ir_rc5.h
 *
 *  Created on: 2011-08-02
 *      Author: Miros�aw Karda�
 */


#ifndef IR_RC5_H_
#define IR_RC5_H_


#if IR_TYP == RC5

#define TOLERANCE 200
#define MIN_HALF_BIT 	ir_micro_s(889 	- 		TOLERANCE)
#define MAX_HALF_BIT 	ir_micro_s(889 	+ 		TOLERANCE)
#define MAX_BIT 	  ir_micro_s((889+889) 	+ 		TOLERANCE)

// podajemy warto�� 1 je�li chcemy uzy� virtual Toggle bit lub 0 je�li nie
// parametr nieistotny je�li wybrany typ to RC5
#define VIRTUAL_TOGGLE 1

#endif


// DEKLARACJE FUNKCJI GLOBALNYCH

#if IR_TYP == RC5
void register_ir_event_callback(void (*callback)(uint8_t address,
				uint8_t command, uint8_t key_time));
#endif

#endif /* IR_RC5_H_ */
