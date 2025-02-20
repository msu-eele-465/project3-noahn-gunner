#include "msp430fr2355.h"
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define Max_PWM (1000) // Max brightness
#define Max_Count (40) // Color update interval

// RGB LED states
#define STATE_LOCKED 0
#define STATE_UNLOCKING 1
#define STATE_UNLOCKED 2

volatile uint8_t system_state = STATE_LOCKED; // Initial state

// Function Prototypes
void updateRGB(uint16_t r, uint16_t g, uint16_t b);
void setupButtonInterrupt(void);

// Function to update RGB LED colors
void updateRGB(uint16_t r, uint16_t g, uint16_t b) {
    TB3CCR1 = r;  // Red (P6.0)
    TB3CCR2 = g;  // Green (P6.1)
    TB3CCR3 = b;  // Blue (P6.2)
}

// ISR for Button Press (S1 - P4.1)
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S1(void) {
    system_state = (system_state + 1) % 3; // Cycle through 3 states

    // Set RGB LED color based on state
    switch (system_state) {
        case STATE_LOCKED:
            updateRGB(196 * 8, 62 * 8, 29 * 8);  // #c43e1d
            break;
        case STATE_UNLOCKING:
            updateRGB(196 * 8, 146 * 8, 29 * 8); // #c4921d
            break;
        case STATE_UNLOCKED:
            updateRGB(29 * 8, 162 * 8, 196 * 8); // #1da2c4
            break;
    }

    P4IFG &= ~BIT1; // Clear interrupt flag
}

void setupButtonInterrupt(void) {
    P4DIR &= ~BIT1;  // Set P4.1 (S1) as input
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IES |= BIT1;
    P4IFG &= ~BIT1;
    P4IE |= BIT1;
}

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    //-- Setup Ports for RGB LED
    P6DIR |= BIT0 | BIT1 | BIT2;   // Set P6.0, P6.1, P6.2 as output
    P6SEL0 |= BIT0 | BIT1 | BIT2;  // Assign to Timer3_B7
    P6SEL1 &= ~(BIT0 | BIT1 | BIT2); // Ensure primary function mode
    
    // Disable GPIO high-impedance mode
    PM5CTL0 &= ~LOCKLPM5;

    // Configure Timer3_B7 for PWM
    TB3CCR0 = Max_PWM;
    TB3CCTL1 = OUTMOD_7;
    TB3CCTL2 = OUTMOD_7;
    TB3CCTL3 = OUTMOD_7;

    // Set initial state to "Locked" color
    updateRGB(196 * 4, 62 * 4, 29 * 4); // #c43e1d

    TB3CTL = TBSSEL_2 | MC_1;  // Use SMCLK, Up mode

    setupButtonInterrupt(); // Setup S1 button interrupt

    // Enable low-power mode with global interrupts
    _BIS_SR((LPM0_bits | GIE));

    while (true) {}
}
