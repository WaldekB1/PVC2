/* rtc_pcf8583.c
 * Author: Waldemar Barczyk
 *   Data: 2019.01.03
  */

#ifndef COMMON_H_
#define COMMON_H_

//podœwietlenie lcd
#define LCD_LED (1<<PD7)
#define LCD_LED_OFF PORTD &= ~LCD_LED
#define LCD_LED_ON  PORTD |=  LCD_LED
#define LCD_LED_TOG PORTD ^=  LCD_LED
#define LCD_LED_DDR DDRD

//dioda led 1
#define LED 	(1<<PD4)
#define LED_TOG PORTD ^= LED
#define LED_DDR DDRD

//dioda led 2
#define LED2 	 (1<<PD5)
#define LED2_TOG PORTD ^= LED2
#define LED2_DDR DDRD

//adresy PCF
#define VOL_ADR 	0x70		//0x4E
#define INPUT_ADR 	0x40		//0x40


#endif /* COMMON_H_ */
