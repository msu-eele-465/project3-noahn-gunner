#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

// LED Array Pins (3.0 - 3.7)
#define LED_PIN0 BIT0
#define LED_PIN1 BIT1
#define LED_PIN2 BIT2
#define LED_PIN3 BIT3
#define LED_PIN4 BIT4
#define LED_PIN5 BIT5
#define LED_PIN6 BIT6
#define LED_PIN7 BIT7

#define Array_Pins (LED_PIN0 | LED_PIN1 | LED_PIN2 | LED_PIN3 | LED_PIN4 | LED_PIN5 | LED_PIN6 | LED_PIN7)

// Pattern Enum
typedef enum {
    Pattern_Off = -1,
    Pattern_0_Static = 0,  // Static
    Pattern_1_Toggle = 1,  // Toggle
    Pattern_2_Up = 2,  // Up Counter
    Pattern_3_In_Out = 3   // In & Out
} led_pattern_t;

// Function Prototypes
void setupTimer(void);
void updateTimerPeriod(float new_period);
void increaseTimerPeriod(void);
void decreaseTimerPeriod(void);
void array_init(void);
void selectPattern(led_pattern_t pattern);

#endif // LED_PATTERNS_H
