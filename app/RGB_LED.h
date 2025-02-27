#ifndef RGB_LED_H
#define RGB_LED_H

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

// RGB LED states
#define STATE_LOCKED 0
#define STATE_UNLOCKING 1
#define STATE_UNLOCKED 2

// PWM Configurations
#define Max_PWM (1000) // Max brightness
#define Max_Count (40) // Color update interval

// Global State Variable
extern volatile uint8_t system_state;

// Function Prototypes
void updateRGB(uint8_t r, uint8_t g, uint8_t b);
void setupButtonInterrupt(void);
void setupRGBLED(void);

#endif // RGB_LED_H
