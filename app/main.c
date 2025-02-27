#include "LED_Patterns.h"
#include "msp430fr2355.h"
#include <msp430.h>


// Timing Stuff
static float base_period = 1.0; // 1 second
static unsigned int pattern_step1 = 0;
static unsigned int pattern_step2 = 0;
static unsigned int pattern_step3 = 0;
static led_pattern_t current_pattern = Pattern_Off;
static bool pattern_active = false;

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
    TB2CTL |= TBCLR;
    TB2CTL |= TBSSEL__SMCLK;
    TB2CTL |= MC__UP;
    TBCTL |= CNTL_0;

    TB2CCR0 = 65535;    // ~ 1 sec
    TB2EX0 |= TBIDEX__7;

    TB2CCTL0 |= CCIE;   // Clear ISR flag
    TB2CCTL0 &= ~CCIFG;  // Enable interrupt

    __enable_interrupt();
}

// Function to Update Timer Period Dynamically
void updateTimerPeriod(float new_period) {
    TB2CCR0 |= CCIFG;   // Disable timer interrupt
    base_period = new_period;

    TB2CCR0 = (unsigned int)(new_period * 65535);
    TB2CCTL0 |= CCIE;   // Clr ISR flag
    TB2CCTL0 &= ~CCIFG; // Enable interrupt
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
    current_pattern = Pattern_Off;
}

void array_init(void) {
    P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);  // Set P3.0 - P3.7 as outputs
    P3OUT |= Array_Pins;
    //array_Off();     // Start with LEDs off

    
    // Configure S1 (P4.1) as an input with pull-up resistor
    P4DIR &= ~BIT1;  // Set P4.1 as input
    P4REN |= BIT1;   // Enable pull-up/down resistor
    P4OUT |= BIT1;   // Set pull-up resistor

    // Enable Interrupt on S1 (Falling Edge)
    P4IES |= BIT1;   // Trigger on high-to-low transition
    P4IFG &= ~BIT1;  // Clear interrupt flag
    P4IE |= BIT1;    // Enable interrupt for P4.1
    
    setupTimer();    // Initialize Timer
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
            P3OUT = 0xAA;
            break;
        
        case Pattern_1_Toggle:
            P3OUT = (P3OUT & ~Array_Pins) | Pattern_1[pattern_step1];
            pattern_step1 = (pattern_step1 + 1) % 2;    // Toggle every cycle ( every 1.0 "base period" seconds)
            break;

        case Pattern_2_Up:
            P3OUT = (P3OUT & ~Array_Pins) | pattern_step2;
            pattern_step2 = (pattern_step2 + 1) % 256;  // 8-bit Up Counter
            break;

        case Pattern_3_In_Out:
            P3OUT = (P3OUT & ~Array_Pins) | Pattern_3[pattern_step3];
            pattern_step3 = (pattern_step3 + 1) % 6;    // Cycle through 6 steps
            break;

        default:
            P3OUT |= Array_Pins;
            break;    
    }
}



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
    //P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
    array_init();              // Initialize LEDs and timer once

    selectPattern(Pattern_0_Static);  // Start with a default pattern

    //__enable_interrupt();       // Ensure global interrupts are enabled

    while (1) {
        // The ISR will handle LED updates, so no need for manual calls here
        //P3OUT = 0b10101010;
    }
}


// Timer3_B7 ISR (Triggers Pattern Updates)
#pragma vector = TIMER2_B0_VECTOR
__interrupt void Timer2_A0_ISR(void) {
    updatePattern();  // Call the update function on each interrupt
    TB2CCTL0 &= ~CCIFG;  // Clear interrupt flag
}

#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S1(void) {
    P4IFG &= ~BIT1;  // Clear interrupt flag

    // Cycle through patterns
    if (current_pattern == Pattern_3_In_Out) {
        selectPattern(Pattern_0_Static);  // Wrap back to Pattern 0
    } else {
        selectPattern(current_pattern + 1);  // Move to the next pattern
    }
}
