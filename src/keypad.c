#include <stdio.h>
#include <msp430.h>
#include <stdbool.h>

// initialize ports
char current_key = 'N';

int init_keypad_ports(void) {
    // rows (outputs for row_cycle)
    P3DIR |= BIT4; // set 3.4 as output
    P4DIR |= BIT5; // set 4.5 as output 
    P5DIR |= BIT2; // set 5.2 as output
    P3DIR |= BIT6; // set 3.6 as output 
     
    P3OUT &= ~BIT4; // turn 3.4 off
    P4OUT &= ~BIT5; // turn 4.5 off
    P5OUT &= ~BIT2; // turn 5.2 off
    P3OUT &= ~BIT6; // turn 3.6 off

    // columns (inputs for polling)
    P1DIR &= ~BIT2; // set 1.2 as output
    P1REN |= BIT2;  // enable pull up/down resistor
    P1OUT |= BIT2;  // pull up resistor
    P1IES |= BIT2;  // IRQ Sensitivity H to L

    P1DIR &= ~BIT3; // set 1.3 as output
    P1REN |= BIT3;  // enable pull up/down resistor
    P1OUT |= BIT3;  // pull up resistor

    P1DIR &= ~BIT6; // set 1.6 as output
    P1REN |= BIT6;  // enable pull up/down resistor
    P1OUT |= BIT6;  // pull up resistor

    P1DIR &= ~BIT7; // set 1.7 as output
    P1REN |= BIT7;  // enable pull up/down resistor
    P1OUT |= BIT7;  // pull up resistor

    return 0;
}

int init_keypad_irqs(void) {
    P1IFG &= ~BIT2; // clear port 1.2 IRQ flag
    P1IE |= BIT2;   // enable 1.2 IRQ 
    return 0;
}

int row_cycle(void) {
    P3OUT |= BIT4; // turn 3.4 on
    P3OUT &= ~BIT4; // turn 3.4 off
    P4OUT |= BIT5; // turn 4.5 on
    P4OUT &= ~BIT5; // turn 4.5 off
    P5OUT |= BIT2; // turn 5.2 on
    P5OUT &= ~BIT2; // turn 5.2 off
    P3OUT |= BIT6; // turn 3.6 on
    P3OUT &= ~BIT6; // turn 3.6 off

    return 0;
}
