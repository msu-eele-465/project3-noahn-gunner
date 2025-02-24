#include "msp430fr2355.h"
#include <msp430.h>
#include <stdint.h>

// Constants
#define SMCLK_FREQ 23003000  // 1 MHz system clock
#define DEFAULT_BASE_PERIOD_MS 1000  // Default: 1.0s transition period

// Variables
volatile uint16_t base_period_ms = DEFAULT_BASE_PERIOD_MS;  // Stores the base transition period (in ms)
volatile uint8_t toggle_state = 0b10101010;  // Bit flip pattern state

// Timer3_B7 Configuration
void setupTimer(void) {
    TB3CTL = TBSSEL_2 | ID__8 | MC_1;  // Use SMCLK, divide by 8, Up mode
    TB3CCR0 = (base_period_ms * (SMCLK_FREQ / 8 / 1000));  // Adjusted CCR0
    TB3CCTL0 = CCIE;  // Enable interrupt
}


// Function to Update Timer Period Dynamically
void updateTimerPeriod(uint16_t new_period_ms) {
    base_period_ms = new_period_ms;  // Store new value
    
    TB3CTL &= ~MC_1;   // Stop the timer
    TB3CCR0 = 0;   // Clear the timer (to apply new CCR0)
    TB3CCR0 = (base_period_ms * (SMCLK_FREQ / 8 / 1000));  // Correct scaling
    TB3CTL |= MC_1;    // Restart the timer in Up mode
}




// Function to Set LED Bar Output
void setLEDs(uint8_t pattern) {
    P1OUT = (P1OUT & ~(BIT4 | BIT1)) | ((pattern & 0x01) ? BIT4 : 0) | ((pattern & 0x20) ? BIT1 : 0);
    P5OUT = (P5OUT & ~(BIT3 | BIT1 | BIT0 | BIT4)) |
            ((pattern & 0x02) ? BIT3 : 0) | ((pattern & 0x04) ? BIT1 : 0) |
            ((pattern & 0x08) ? BIT0 : 0) | ((pattern & 0x10) ? BIT4 : 0);
    P3OUT = (P3OUT & ~(BIT5 | BIT1)) | ((pattern & 0x40) ? BIT5 : 0) | ((pattern & 0x80) ? BIT1 : 0);
}




// Main Function
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    P1DIR |= BIT4 | BIT1;
    P5DIR |= BIT3 | BIT1 | BIT0 | BIT4;
    P3DIR |= BIT5 | BIT1;

    setupTimer();  // Initialize Timer3_B7

    P4DIR &= ~BIT1;      // S1 P4.1
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IES |= BIT1;

    

    PM5CTL0 &= ~LOCKLPM5;  // Enable GPIOs

    //-- Setup IRQ
    P4IFG &= ~BIT1; // Clear interrupt flag
    P4IE |= BIT1;   // Enable interrupt for S1 (P4.1)

    __bis_SR_register(GIE | LPM0_bits);  // Enable global interrupts & low-power mode

    while (1) {
        // Example: Change speed after some time (for testing)
        //__delay_cycles(5000000);
        

        

    }
}

// Timer3_B7 ISR (Handles Pattern Updates)
#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_TB3_CCR0(void) {
    toggle_state ^= 0xFF;  // Flip all bits
    setLEDs(toggle_state); // Apply flipped pattern

    TB3CCR0 = base_period_ms * (SMCLK_FREQ / 1000);  // Update dynamically
    TB3CCTL0 &= ~CCIFG;  // Clear interrupt flag
}

#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S1(void)
{
    updateTimerPeriod(500);  // Change to 0.5s and see if toggle speeds up
    P4IFG &= ~BIT1;    // Clear interrupt flag
}