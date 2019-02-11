/*
 * ir_config.h
 *
 *  Created on: 2011-08-01
 *      Author: Miros³aw Kardaœ
 */

#ifndef IR_UNI_H_
#define IR_UNI_H_

// makro przeliczaj¹ce czasy w us w zale¿noœci od F_CPU (automatycznie)
#define ir_micro_s(num) (((num)*((F_CPU/1000UL)/8))/1000)

#define SONY 0
#define RC5 1
#define SAMSUNG 2
#define JVC 3

#define IR_TYP RC5


/* *************************           K O N F I G U R A C J A              ********* */
/* *************************   U S T A W I E N I A   U ¯ Y T K O W N I K A  ********* */

/*----------------------------------------PILOT RC5---------------------------------------*/
#define IR_PIN (1<<PD6)			// numer pinu wejœcia ICP
#define IR_DIR DDRD
#define IR_PORT PORTD
/*-----------------------------------------------------------------------------------------*/




// deklaracje zmiennych globalnych na wypadek
// gdyby nie mo¿na by³o pos³u¿yæ siê zdarzeniem EVENT
extern volatile uint8_t address;
extern volatile uint8_t command;
extern volatile uint8_t Ir_key_press_flag;
extern volatile uint8_t key_time;


// DEKLARACJE FUNKCJI GLOBALNYCH
void ir_init(void);

void IR_EVENT(void);

#include "ir_sony.h"
#include "ir_rc5.h"
#include "ir_jvc.h"
#include "ir_samsung.h"

#endif /* IR_UNI_H_ */
