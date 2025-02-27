#include "RGB_LED.h"
#include "intrinsics.h"
#include <msp430.h>
#include <stdio.h>
#include <stdbool.h>
#include "../src/keypad.c"
#include "intrinsics.h"

// puts together system_state and locked_state
int state_sync(void) {
    if (locked_state == 0) {
        system_state = STATE_LOCKED;
    } else if (locked_state == 1) {
        system_state = STATE_UNLOCKING;
    } else if (locked_state == 2) {
        system_state = STATE_UNLOCKED;
    } else if (locked_state == 3) {
        system_state = STATE_UNLOCKED_2;
    } else if (locked_state == 4) {
        system_state = STATE_UNLOCKED_3;
    } else if (locked_state == 5) {
        system_state = STATE_UNLOCKED_4;
    } else if (locked_state == 6) {
        system_state = STATE_UNLOCKED_5;
    } else if (locked_state == 7) {
        system_state = STATE_UNLOCKED_6;
    } else if (locked_state == 8) {
        system_state = STATE_UNLOCKED_7;
    }
    return 0;
}

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Setup LED
    P1DIR |= BIT0;      // P1.0 (Red LED) as output
    P1OUT &= ~BIT0;     // Clear P1.0

    init_keypad_ports();
    setupRGBLED();  // Initialize RGB LED
    
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;
  
    // Timer B0 Setup
    TB0CTL |= TBCLR;            // Clear timer and dividers
    TB0CTL |= TBSSEL__ACLK;     // ACLK Source
    TB0CTL |= MC__CONTINUOUS;   // Continuous Mode

    // B0 ISR Setup
    TB0CTL &= ~TBIFG;           // Clear TB0 Flag
    TB0CTL |= TBIE;             // Enable TB0 Overflow IRQ
    
    init_keypad_irqs();
    __enable_interrupt();

    int i;
  
    while(true)
    {  
        col_masking();

        for(i=0; i<10000; i++) {}
        
        lock_state();
        state_sync();
        LEDState();

        for(i=0; i<10000; i++) {}
        
        prev_key = current_key;
        
        for(i=0; i<10000; i++) {}
        
        row_cycle();
        
        for(i=0; i<10000; i++) {}

    }
}

//----Interrupt Service Routines
#pragma vector = PORT1_VECTOR
__interrupt void ISR_Port1_Column(void) {
    
    col_masking();
    if (col_1 == 0) {
        if (current_row == 1) {
            current_key = '1';
        } else if (current_row == 2) {
            current_key = '4';
        } else if (current_row == 3) {
            current_key = '7';
        } else if (current_row == 4) {
            current_key = '*';
        }
    } else if (col_2 == 0) {
        if (current_row == 1) {
            current_key = '2';
        } else if (current_row == 2) {
            current_key = '5';
        } else if (current_row == 3) {
            current_key = '8';
        } else if (current_row == 4) {
            current_key = '0';
        }
    } else if (col_3 == 0) {
        if (current_row == 1) {
            current_key = '3';
        } else if (current_row == 2) {
            current_key = '6';
        } else if (current_row == 3) {
            current_key = '9';
        } else if (current_row == 4) {
            current_key = '#';
        }
    } else if (col_4 == 0) {
        if (current_row == 1) {
            current_key = 'A';
        } else if (current_row == 2) {
            current_key = 'B';
        } else if (current_row == 3) {
            current_key = 'C';
        } else if (current_row == 4) {
            current_key = 'D';
        }
    } else {
        current_key = 'N';
    }
    P1IFG &= ~BIT2;
    P1IFG &= ~BIT3;
    P1IFG &= ~BIT6;
    P1IFG &= ~BIT7;
    
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_Overflow(void) {
    P1OUT ^= BIT0;      // Toggle LED1
    TB0CTL &= ~TBIFG;   // Clear TB0 Flag
}
