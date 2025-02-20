#include "msp430fr2355.h"
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define Max_PWM (1000) // max brightness
#define Max_Count (40) // color update interval
// Initial PWM values for Red, Green, and Blue
int rv = 500;
int gv = 0;
int bv = 500;

// Change step sizes for each color
int drv = -50;
int dbv = 50;
int dgv = 50;

int c = 0; // Cycle counter for color updates

// Function to update RGB LED colors
inline void update_colors(void) {
    rv += drv;
    bv += dbv;
    gv += dgv;

    if (rv > Max_PWM) { 
        rv = Max_PWM; 
        drv = -drv; 
    }
    else if (rv < 0) { 
        rv = 0; 
        drv = -drv; 
    }
    if (bv > Max_PWM) { 
        bv = Max_PWM; 
        dbv = -dbv; 
    }
    else if (bv < 0) { 
        bv = 0; 
        dbv = -dbv; 
    }
    if (gv > Max_PWM) { 
        gv = Max_PWM; 
        dgv = -dgv; 
    }
    else if (gv < 0) { 
        gv = 0; 
        dgv = -dgv; 
    }

    // Update PWM duty cycle for Timer3_B7
    TB3CCR1 = rv;  // Red (P6.0)
    TB3CCR2 = gv;  // Green (P6.1)
    TB3CCR3 = bv;  // Blue (P6.2)
}


int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    //-- Setup Ports
    P6DIR |= BIT0 | BIT1 | BIT2;    // Set P6.0, P6.1, P6.2 as output
    P6SEL0 |= BIT0 | BIT1 | BIT2;   // Assign to Timer3_B7
    P6SEL1 &= ~(BIT0 | BIT1 | BIT2); // Ensure primary function mode
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    //-- Configure Timer3_B7
    TB3CCR0 = Max_PWM;
    TB3CCTL1 = OUTMOD_7;
    TB3CCTL2 = OUTMOD_7;
    TB3CCTL3 = OUTMOD_7;

    // Set initial dudy cycle
    TB3CCR1 = rv;
    TB3CCR2 = gv;
    TB3CCR3 = bv;

    TB3CTL = TBSSEL_2 | MC_1;  // Use SMCLK, Up mode

    // Enable Timer3_B7 interrupts (TB3.0 overflow)
    TB3CTL |= TBIE;

    // Enable low-power mode with global interrupts
    _BIS_SR((LPM0_bits | GIE));

    while(true)
    {
    }
}

// Interrupt Service Routine for Timer3_B7 Overflow
__attribute__((interrupt(TIMER3_B1_VECTOR)))
void tim3_b1_isr(void) {
    ++c;
    if (c >= Max_Count) {
        update_colors(); // Update RGB values
        c = 0;
    }
    TB3CTL &= ~TBIFG; // Clear interrupt flag
}