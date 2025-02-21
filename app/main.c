#include <msp430.h>
#include <stdbool.h>
#include "../src/keypad.c"
#include "intrinsics.h"

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    init_keypad_ports();
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    init_keypad_irqs();
    __enable_interrupt();

    while(true)
    {
        row_cycle();
    }
}

//----Interrupt Service Routines
#pragma vector = PORT1_VECTOR
__interrupt void ISR_Port1_Column(void) {
    
    P1IFG &= ~BIT2;
}