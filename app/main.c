#include "intrinsics.h"
#include <msp430.h>
#include <stdbool.h>

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Setup LED
    P1DIR |= BIT0;      // P1.0 (Red LED) as output
    P1OUT &= ~BIT0;     // Clear P1.0
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Timer B0 Setup
    TB0CTL |= TBCLR;            // Clear timer and dividers
    TB0CTL |= TBSSEL__ACLK;     // ACLK Source
    TB0CTL |= MC__CONTINUOUS;   // Continuous Mode

    // B0 ISR Setup
    TB0CTL &= ~TBIFG;           // Clear TB0 Flag
    TB0CTL |= TBIE;             // Enable TB0 Overflow IRQ
    __enable_interrupt();       // enable maskable IRQs

    while(true) {
        return 0;
    }
}

//-------Interrupt Service Routines--------

#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_Overflow(void) {
    P1OUT ^= BIT0;      // Toggle LED1
    TB0CTL &= ~TBIFG;   // Clear TB0 Flag
}
