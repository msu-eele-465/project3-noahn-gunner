#include "RGB_LED.h"
#include "intrinsics.h"
#include <msp430.h>
#include <stdio.h>
#include <stdbool.h>
#include "../src/keypad.c"
#include "intrinsics.h"
#include "LED_Patterns.h"
#include "msp430fr2355.h"

// Timing Stuff
static float base_period = 1.0; // 1 second
static unsigned int pattern_step1 = 0;
static unsigned int pattern_step2 = 0;
static unsigned int pattern_step3 = 0;
static led_pattern_t current_pattern = Pattern_Off;
static bool pattern_active = false;

// -- Patterns
static const unsigned char Pattern_0 = 0b10101010;

static const unsigned char Pattern_1[2] = {     // Toggle pattern
    0b10101010,
    0b01010101
};

static const unsigned char Pattern_3[6] = {     // In & Out
    0b00011000, 
    0b00100100, 
    0b01000010, 
    0b10000001, 
    0b01000010, 
    0b00100100
};

// Timer3_B7 Configuration
void setupTimer(void) {
    TB2CTL |= TBCLR;
    TB2CTL |= TBSSEL__SMCLK;
    TB2CTL |= MC__UP;
    TBCTL |= CNTL_0;

    TB2CCR0 = 65535;    // ~ 1 sec
    TB2EX0 |= TBIDEX__7;

    TB2CCTL0 |= CCIE;   // Clear ISR flag
    TB2CCTL0 &= ~CCIFG;  // Enable interrupt

    __enable_interrupt();
}

// Function to Update Timer Period Dynamically
void updateTimerPeriod(float new_period) {
    TB2CCR0 |= CCIFG;   // Disable timer interrupt
    base_period = new_period;

    TB2CCR0 = (unsigned int)(new_period * 65535);
    TB2CCTL0 |= CCIE;   // Clr ISR flag
    TB2CCTL0 &= ~CCIFG; // Enable interrupt
}

void increaseTimerPeriod(void) {
    if (base_period < 1) {
        updateTimerPeriod(base_period + 0.25);
    }
}

void decreaseTimerPeriod(void) {
    if (base_period > 0.25) {
        updateTimerPeriod(base_period - 0.25);
    }
}

void array_Off(void) {                              // USE WHEN SYSTEM IS LOCKED
    P3OUT |= 0x00;
    pattern_active = false;
    current_pattern = Pattern_Off;
}

void array_init(void) {
    P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);  // Set P3.0 - P3.7 as outputs
    P3OUT |= Array_Pins;
    array_Off();     // Start with LEDs off
    setupTimer();    // Initialize Timer
}


void selectPattern(led_pattern_t pattern) {
    if (pattern == current_pattern) {
        switch (current_pattern) {
            case Pattern_1_Toggle:
                pattern_step1 = 0;
                break;
            case Pattern_2_Up:
                pattern_step2 = 0;
                break;
            case Pattern_3_In_Out:
                pattern_step3 = 0;
                break;
        }
    } else {
        current_pattern = pattern;
    }
    pattern_active = true;
}

void updatePattern(void) {
    if (!pattern_active) return;

    switch (current_pattern) {
        case Pattern_0_Static:
            P3OUT = 0xAA;
            break;
        
        case Pattern_1_Toggle:
            P3OUT = (P3OUT & ~Array_Pins) | Pattern_1[pattern_step1];
            pattern_step1 = (pattern_step1 + 1) % 2;    // Toggle every cycle ( every 1.0 "base period" seconds)
            break;

        case Pattern_2_Up:
            P3OUT = (P3OUT & ~Array_Pins) | pattern_step2;
            pattern_step2 = (pattern_step2 + 1) % 256;  // 8-bit Up Counter
            break;

        case Pattern_3_In_Out:
            P3OUT = (P3OUT & ~Array_Pins) | Pattern_3[pattern_step3];
            pattern_step3 = (pattern_step3 + 1) % 6;    // Cycle through 6 steps
            break;

        default:
            P3OUT |= Array_Pins;
            break;    
    }
}


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
    
    array_init();              // Initialize LEDs and timer once
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

// Timer3_B7 ISR (Triggers Pattern Updates)
#pragma vector = TIMER2_B0_VECTOR
__interrupt void Timer2_A0_ISR(void) {
    updatePattern();  // Call the update function on each interrupt
    TB2CCTL0 &= ~CCIFG;  // Clear interrupt flag
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_Overflow(void) {
    P1OUT ^= BIT0;
    TB0CTL &= ~TBIFG;
}
