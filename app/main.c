#include <msp430.h>
#include <stdbool.h>
#include "../src/keypad.c"

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    init_keypad_ports();
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    while(true)
    {
        row_cycle();
    }
}
