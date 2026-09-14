#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "millis.h"
#include "uart.h"
#include "analogRead.h"

#define IN_PIN PC0

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) 

// B (digital pin 8 to 13)
// C (analog input pins)
// D (digital pins 0 to 7)
// https://wokwi.com/projects/363784064760337409



int main(void)
{
    millis_init();
	init_serial();
	srand(time(NULL));
    sei();

	uint16_t value = 0;
	float celsius = 0;
	int16_t integer = 0;
	int16_t decimals = 0;
	millis_t millis_since_last_print = 0;
	millis_t current_millis = 0;

	while(1) {

		current_millis = millis();
		value = analogRead(IN_PIN);

		// Printa indatan varje sekund
		if (current_millis - millis_since_last_print >= 1000) {
			// Beräkningen är tagen från wokwis beskrivning av komponenten
			celsius = 1 / (log(1 / (1023. / value - 1)) / 3950. + 1.0 / 298.15) - 273.15;
			// Speciallösning då vårat väldigt simpla uart-bibliotek inte kan printa flyttal
			integer = (int)celsius;
			decimals = abs((int)((celsius - integer) * 100));
			printf("Current value: %d.%d\n", integer, decimals);
			millis_since_last_print = current_millis;
		}
	}
	return 0;
}