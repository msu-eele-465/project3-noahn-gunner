#include "RGB_LED.h"

// Initial state
volatile uint8_t system_state = STATE_LOCKED;

// Function to update RGB LED colors
void updateRGB(uint8_t r, uint8_t g, uint8_t b) {
    // Scale 0-255 to 0-Max_PWM
    TB3CCR1 = (r * Max_PWM) / 255;  // Red (P6.0)
    TB3CCR2 = (g * Max_PWM) / 255;  // Green (P6.1)
    TB3CCR3 = (b * Max_PWM) / 255;  // Blue (P6.2)
}


// ISR for Button Press (S1 - P4.1)
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S1(void) {
    system_state = (system_state + 1) % 3; // Cycle through 3 states

    // Set RGB LED color based on state (scaled from hex values)
    switch (system_state) {
        case STATE_LOCKED:
            updateRGB(196, 62, 29);  // #c43e1d
            break;
        case STATE_UNLOCKING:
            updateRGB(196, 146, 29); // #c4921d
            break;
        case STATE_UNLOCKED:
            updateRGB(29, 162, 196); // #1da2c4
            break;
    }

    P4IFG &= ~BIT1; // Clear interrupt flag
}


// Function to Setup Button Interrupt
void setupButtonInterrupt(void) {
    P4DIR &= ~BIT1;  // Set P4.1 (S1) as input
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IES |= BIT1;
    P4IFG &= ~BIT1;
    P4IE |= BIT1;
}

// Function to Setup RGB LED
void setupRGBLED(void) {
    P6DIR |= BIT0 | BIT1 | BIT2;   // Set P6.0, P6.1, P6.2 as output
    P6SEL0 |= BIT0 | BIT1 | BIT2;  // Assign to Timer3_B7
    P6SEL1 &= ~(BIT0 | BIT1 | BIT2); // Ensure primary function mode
    
    // Configure Timer3_B7 for PWM
    TB3CCR0 = Max_PWM;
    TB3CCTL1 = OUTMOD_7;
    TB3CCTL2 = OUTMOD_7;
    TB3CCTL3 = OUTMOD_7;

    TB3CTL = TBSSEL_2 | MC_1;  // Use SMCLK, Up mode

    // Set initial state to "Locked" color
    updateRGB(196, 62, 29);  // #c43e1d

    setupButtonInterrupt(); // Setup S1 button interrupt
}
