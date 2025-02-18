#include <msp430.h> 
int counter = 0;


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//-- Setup Ports
	P1DIR |= BIT0;
	P1OUT |= BIT0;
    // Button S1 & S2 pins as input with pull-up resistors
    P4DIR &= ~BIT1;      // S1 P4.1
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IES |= BIT1;

    P2DIR &= ~BIT3;      // S2 P2.3
    P2REN |= BIT3;
    P2OUT |= BIT3;
    P2IES |= BIT3;

    PM5CTL0 &= ~LOCKLPM5;

    //-- Setup IRQ
    P4IFG &= ~BIT1; // Clear interrupt flag
    P4IE |= BIT1;   // Enable interrupt for S1 (P4.1)

    P2IFG &= ~BIT3; // Clear interrupt flag
    P2IE |= BIT3;   // Enable interrupt for S2 (P2.3)


    //-- Setup Timer
    TB0CTL |= TBCLR;    // Clear timer and dividers
    TB0CTL |= TBSSEL__SMCLK;    // Source = SMCLK
    TB0CTL |= MC__UP;    // Mode = UP
    TB0CCR0 = 1000;
    TB0CCR1 = 250;

    //-- Setup Timer Overflow IRQ
    TB0CCTL0 &= ~CCIFG;       // Clear CCR0 Flag
    TB0CCTL0 |= CCIE;        // Enable TB0 CCR0 Overflow IRQ
    TB0CCTL1 &= ~CCIFG;      // Clear CCR1 Flag
    TB0CCTL1 |= CCIE;        // Enable TB0 CCR1 Overflow IRQ

    __enable_interrupt();   // enable maskable IRQs

    //-- Main loop
    while(1){}

	return 0;
}

//-- Interrupt Service Routines
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    P1OUT |= BIT0;  // toggle LED1 = 1
    TB0CCTL0 &= ~CCIFG;   // clear CCR0 flag
}
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void)
{
    P1OUT &= ~BIT0;  // toggle LED1 = 0
    TB0CCTL1 &= ~CCIFG;   // clear CCR1 flag
}
#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S1(void)
{
    TB0CCR1 += 25;  // Increase duty cycle by 2.5%
    if (TB0CCR1 >= 501)  // To ensure the max limit is 500 (50%)
    {
        TB0CCR1 = 500;
    }
    P4IFG &= ~BIT1;    // Clear interrupt flag
}

#pragma vector = PORT2_VECTOR
__interrupt void ISR_Port2_S2(void)
{
    TB0CCR1 -= 25;  // Decrease duty cycle by 2.5%
    if (TB0CCR1 <= 25)
    {
        TB0CCR1 = 25;
    }
    P2IFG &= ~BIT3;    // Clear interrupt flag
}
