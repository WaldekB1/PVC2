/* rtc_pcf8583.c
 * Author: Waldemar Barczyk
 *   Data: 2018.12.15
 *   Na podstawie instrukcji Miros³awa Kardasia
 */

#ifndef RTC_PCF8583_RTC_PCF8583_H_
#define RTC_PCF8583_RTC_PCF8583_H_

#define DATETIME_AS_STRING	1
#define DATE_SEPARATOR  '.'

#define PCF8583_ADDR 0xA0  	// gdy A1 --> GND

#define USE_RTC_INT  0


typedef enum { pon, wto, sro, czw, pia, sob, nie} TDAYS;

typedef struct
{
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
	uint16_t YY;
	uint8_t MM;
	uint8_t DD;
	TDAYS weekday;
	uint8_t dst;
		char time [9];
		char date [11];
	uint8_t pcf_buf [5];
}TDATETIME;

extern TDATETIME datetime;
extern const char dni[];
extern volatile uint8_t int0_flag;

void init_rtc (void);

void register_rtc_event_callback( void (*callback)( TDATETIME * dt  ) );
void RTC_EVENT		 ( void );
void get_rtc_datetime( TDATETIME *dt );
void set_rtc_time	 ( TDATETIME *dt,  uint8_t hh, uint8_t mm, uint8_t ss );
void set_rtc_date	 ( TDATETIME *dt, uint16_t YY, uint8_t MM, uint8_t DD );
void set_rtc_datetime( TDATETIME *dt, uint16_t YY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss );

uint8_t oblicz_week_day( uint8_t dzien, uint8_t miesiac, uint16_t year );

uint8_t dec2bcd( uint8_t dec );
uint8_t bcd2dec( uint8_t bcd );

#endif /* RTC_PCF8583_RTC_PCF8583_H_ */
