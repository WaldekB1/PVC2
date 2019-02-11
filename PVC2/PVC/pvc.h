/* rtc_pcf8583.c
 * Author: Waldemar Barczyk
 *   Data: 2018.12.15
 *   Modyfikacja biblioteki Piotra Warysza
 */

#ifndef PVC_PVC_H_
#define PVC_PVC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>


//tablica kroków g³oœnoœci
extern const uint16_t pvc_vol[24][2];

extern int8_t vol;
void set_vol( void );

#endif /* PVC_PVC_H_ */
