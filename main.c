
///////////////////////////////////////////////////////////
//   TimeTask v1.0                                       //
//   =============                                       //
//   Copyright© 2017 by Felix Knobl, FH Technikum Wien   //
//   Partial Code base by see below...					 //
///////////////////////////////////////////////////////////

/**!
 * @file     main.c
 * @version  V0.2
 * @date     JUL 17
 * @author   R. Beneder
 *
 * @mainpage	Structured Timer example
 *
 *  \brief Function _delayus() implements a blocking delay for a given
 *         number of microseconds
 *  \param us the number of microseconds to delay in the range 1 to 99
 *  \returns 0 upon success, >0 upon error
 *
 * ***uint8_t _delayus ( uint8_t us );
 *
 *  \brief Function _delay() implements a blocking delay for a given
 *         number of minutes, seconds, and milliseconds
 *  \param min the number of minutes to delay in the range 0 to 59
 *  \param sec the number of seconds to delay in the range 0 to 59
 *  \param ms the number of milliseconds to delay in the range 1 to 99
 *  \returns 0 upon success, >0 upon error
 *
 *	 ***uint8_t _delay ( uint8_t min, uint8_t sec, uint8_t ms );
 *  \brief Function _timeout() implements a non-blocking delay for a given
 *         number of minutes, seconds, and milliseconds. After the given
 *         timeout the callback function func() shall be invoked.
 *  \param min the number of minutes to delay in the range 0 to 59
 *  \param sec the number of seconds to delay in the range 0 to 59
 *  \param ms the number of milliseconds to delay in the range 1 to 99
 *  \returns 0 upon success, >0 upon error
 *
 *  ***uint8_t _timeout ( uint8_t min, uint8_t sec, uint8_t ms, 
 *                        void (* func )( void ) );
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <xmc4500_timer_lib.h>
#include "GPIO.h"

#define LED1ON 				(PORT1->OMR = 0x00000002UL)
#define LED1OFF 			(PORT1->OMR = 0x00020000UL)

#define LED2ON 				(PORT1->OMR = 0x00000001UL)
#define LED2OFF 			(PORT1->OMR = 0x00010000UL)

void _timeoutfunction (void);

uint8_t pwmValue = 0;
uint8_t seconds = 30;
bool directionUp = true;

void _timeoutfunction()
{
	// If full brightness, dim down
	if (pwmValue == 100)
	{
		directionUp = false;
	}

	// If leds off, dim up
	if (pwmValue == 0)
	{
		directionUp = true;
	}

	// Change dim direction
	if (directionUp)
	{
		pwmValue = pwmValue + 10;
	}
	else
	{
		pwmValue = pwmValue - 10;
	}

	// Timeout counter
	seconds--;

	// Stop after 30s?
	if (seconds > 0)
	{
		// Restart the timer to fire again in 1s
		_timeout (0, 1, 1, _timeoutfunction);
	}
	else
	{
		// Time is up. Turn leds off and don't start the timer again
		pwmValue = 0;
	}
}

int main (void)
{
	// Setup timer
	setup_timer();
	setup_timer_timeout();

	// Set up LED ports
	P1_0_set_mode(OUTPUT_PP_GP);
	P1_0_set_driver_strength(STRONG);
	P1_0_reset();

	P1_1_set_mode(OUTPUT_PP_GP);
	P1_1_set_driver_strength(STRONG);
	P1_1_reset();

	// Initial timer start
	_timeout (0, 1, 1, _timeoutfunction);

	// Endless loop
	while (1)
	{
		// Check pwmValue
		switch (pwmValue)
		{
			case 0:
				// No need to turn on leds when brightness = 0
				LED1OFF;
				LED2OFF;
				break;

			case 100:
				// No need to turn off leds when brightness = 100
				LED1ON;
				LED2ON;
				break;

			default:
				// Software PWM routine
				LED1ON;
				LED2ON;
				_delayus (pwmValue);

				LED1OFF;
				LED2OFF;
				_delayus (100 - pwmValue);
				break;

		}
	}
}
