#include <stdio.h>
#include <msp430.h>
#include <stdbool.h>

// initialize ports
int init_keypad_ports(void) {
    // rows (outputs for row_cycle)
    P1DIR |= BIT2; // set 1.2 as output
    P1DIR |= BIT3; // set 1.3 as output
    P3DIR |= BIT4; // set 3.4 as output
    P4DIR |= BIT5; // set 4.5 as output 
     
    P1OUT &= ~BIT2; // turn 1.2 off
    P1OUT &= ~BIT3; // turn 1.3 off
    P3OUT &= ~BIT4; // turn 3.4 off
    P4OUT &= ~BIT5; // turn 4.5 off

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

    return 0;
}

int row_cycle(void) {
    P1OUT |= BIT2; // turn 1.2 on
    P1OUT &= ~BIT2; // turn 1.2 off
    P1OUT |= BIT3; // turn 1.3 on
    P1OUT &= ~BIT3; // turn 1.3 off
    P3OUT |= BIT4; // turn 3.4 on
    P3OUT &= ~BIT4; // turn 3.4 off
    P4OUT |= BIT5; // turn 4.5 on
    P4OUT &= ~BIT5; // turn 4.5 off

    return 0;
}
