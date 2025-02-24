#include <stdio.h>
#include <msp430.h>
#include <stdbool.h>

// initialize ports
char current_key = 'N';
volatile int col_1 = BIT2;
volatile int col_2 = BIT3;
volatile int col_3 = BIT6;
volatile int col_4 = BIT7;
bool row_1 = false;
bool row_2 = false;
bool row_3 = false;
bool row_4 = false;

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
    P1IFG &= ~BIT3; // clear port 1.3 IRQ flag
    P1IE |= BIT3;   // enable 1.3 IRQ 
    P1IFG &= ~BIT6; // clear port 1.6 IRQ flag
    P1IE |= BIT6;   // enable 1.6 IRQ 
    P1IFG &= ~BIT7; // clear port 1.7 IRQ flag
    P1IE |= BIT7;   // enable 1.7 IRQ 
    return 0;
}

int row_cycle(void) {
    row_1 = true;
    P3OUT |= BIT4; // turn 3.4 on
    row_1 = false;
    P3OUT &= ~BIT4; // turn 3.4 off
    row_2 = true;
    P4OUT |= BIT5; // turn 4.5 on
    row_2 = false;
    P4OUT &= ~BIT5; // turn 4.5 off
    row_3 = true;
    P5OUT |= BIT2; // turn 5.2 on
    row_3 = false;
    P5OUT &= ~BIT2; // turn 5.2 off
    row_4 = true;
    P3OUT |= BIT6; // turn 3.6 on
    row_4 = false;
    P3OUT &= ~BIT6; // turn 3.6 off

    return 0;
}

int col_masking(void) {
    col_1 =  P1IN;
    col_1 &= BIT2;
    col_2 =  P1IN;
    col_2 &= BIT3;
    col_3 =  P1IN;
    col_3 &= BIT6;
    col_4 =  P1IN;
    col_4 &= BIT7;

    return 0;
}
