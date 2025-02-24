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
        col_masking();
        row_cycle();
    }
}

//----Interrupt Service Routines
#pragma vector = PORT1_VECTOR
__interrupt void ISR_Port1_Column(void) {
    col_masking();
    if (col_1 == 0) {
        current_key = '1';
    } else if (col_2 == 0) {
        current_key = '2';
    } else if (col_3 == 0) {
        current_key = '3';
    } else if (col_4 == 0) {
        current_key = 'A';
    } else {
        current_key = 'N';
    }
    P1IFG &= ~BIT2;
    P1IFG &= ~BIT3;
    P1IFG &= ~BIT6;
    P1IFG &= ~BIT7;
    
}