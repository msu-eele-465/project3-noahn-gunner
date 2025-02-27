#include "LED_Patterns.h"
#include "targetConfigs/LED_Patterns.h"
#include <msp430.h>


// Timing Stuff
static float base_period = 1.0; // 1 second
static unsigned int pattern_step1 = 0;
static unsigned int pattern_step2 = 0;
static unsigned int pattern_step3 = 0;
static led_pattern_t current_pattern = Pattern_Off;

// -- Patterns
static const unsigned char Pattern_0 = 0b10101010;

static const unsigned char Pattern_1[2] = {     // Toggle pattern
    0b10101010,
    0b01010101
};

static const unsigned char Pattern_3[6] = {     // In & Out
    0b00011000, 
    0b00100100, 
    0b01000010, 
    0b10000001, 
    0b01000010, 
    0b00100100
};

// Timer3_B7 Configuration
void setupTimer(void) {
    TB3CTL |= TBCLR;
    TB3CTL |= TBSSEL__SMCLK;
    TB3CTL |= MC__UP;
    TB3CTL |= CNTL_0;

    TB3CCR0 = 65535;    // ~ 1 sec
    TB3EX0 |= TBIDEX__7;

    TB3CCTL0 |= CCIE;   // Clear ISR flag
    TB3CCTL0 &= ~CCIFG;  // Enable interrupt

    __enable_interrupt();
}

// Function to Update Timer Period Dynamically
void updateTimerPeriod(float new_period) {
    TB3CCR0 |= CCIFG;   // Disable timer interrupt
    base_period = new_period;

    TB3CCR0 = (unsigned int)(new_period * 65535);
    TB3CCTL0 |= CCIE;   // Clr ISR flag
    TB3CCTL0 &= ~CCIFG; // Enable interrupt
}

void increaseTimerPeriod(void) {
    if (base_period < 1) {
        updateTimerPeriod(base_period + 0.25);
    }
}

void decreaseTimerPeriod(void) {
    if (base_period > 0.25) {
        updateTimerPeriod(base_period - 0.25);
    }
}

void array_Off(void) {                              // USE WHEN SYSTEM IS LOCKED
    P3OUT |= 0x00;
    pattern_active = false;
    current_pattern = Pattern_Off
}

void array_init(void) {
    P3DIR |= 0xFF;  // Set P3.0 - P3.7 as outputs
    array_Off();

    setupTimer();
}

void selectPattern(led_pattern_t pattern) {
    if (pattern == current_pattern) {
        switch (current_pattern) {
            case Pattern_1_Toggle:
                pattern_step1 = 0;
                break;
            case Pattern_2_Up:
                pattern_step2 = 0;
                break;
            case Pattern_3_In_Out:
                pattern_step3 = 0;
                break;
            default:
                break;
        }
    } else {
        current_pattern = pattern;
    }
    pattern_active = true;
}

void updatePattern(void) {
    if (!pattern_active) return;

    switch (current_pattern) {
        case Pattern_0_Static:
            P3
    
    }
}

int main(void) {
    while(1){}
}