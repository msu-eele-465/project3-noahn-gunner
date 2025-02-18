#include <stdio.h>
#include <msp430.h>
#include <stdbool.h>

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // initialize ports
    // rows (outputs for row_cycle)
    P1DIR |= BIT2; // set 1.2 as output
    P1DIR |= BIT3; // set 1.3 as output
    P3DIR |= BIT4; // set 3.4 as output
    P4DIR |= BIT5; // set 4.5 as output

    // columns (inputs for polling)
    P5DIR &= ~BIT2; // set 5.2 as output
    P5REN |= BIT2;  // enable pull up/down resistor
    P5OUT |= BIT2;  // pull up resistor

    P3DIR &= ~BIT6; // set 3.6 as output
    P3REN |= BIT6;  // enable pull up/down resistor
    P3OUT |= BIT6;  // pull up resistor

    P1DIR &= ~BIT7; // set 1.7 as output
    P1REN |= BIT7;  // enable pull up/down resistor
    P1OUT |= BIT7;  // pull up resistor

    P1DIR &= ~BIT6; // set 1.6 as output
    P1REN |= BIT6;  // enable pull up/down resistor
    P1OUT |= BIT6;  // pull up resistor

    PM5CTL0 &= ~LOCKLMP5; //turn on GPIO

    while(true) {
        
    }
}
