#include "msp430fr2355.h"
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#define Max_PWM = 1000 // max brightness

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    //-- Setup Ports
    P6DIR |= (BIT0 | BIT1 | BIT2);
    P6SEL |= (BIT0 | BIT1 | BIT2);
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    //-- Configure Timers



    while(true)
    {
    }
}
