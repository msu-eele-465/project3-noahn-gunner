#include "msp430fr2355.h"
#include <msp430.h>
#include <stdint.h>

// Constants
#define SMCLK_FREQ 23003000  // 1 MHz system clock
#define DEFAULT_BASE_PERIOD_MS 1000  // Default: 1.0s transition period

// Variables
volatile uint16_t base_period_ms = DEFAULT_BASE_PERIOD_MS;  // Stores the base transition period (in ms)
volatile uint8_t toggle_state = 0b10101010;  // Bit flip pattern state
volatile uint8_t counter = 0;  // Up counter starting at 0
volatile uint8_t pattern_step = 0;
volatile uint8_t current_pattern = 0;
static const uint8_t steps[] = {0b00011000, 0b00100100, 0b01000010, 0b10000001, 0b01000010, 0b00100100};  // In & Out Pattern
volatile uint8_t step_index = 0;  // Tracks step in Pattern 3

// Timer3_B7 Configuration
void setupTimer(void) {
    TB3CTL = TBSSEL_2 | ID__8 | MC_1;  // Use SMCLK, divide by 8, Up mode
    TB3CCR0 = (base_period_ms * (SMCLK_FREQ / 8 / 1000));  // Adjusted CCR0
    TB3CCTL0 = CCIE;  // Enable interrupt
}


// Function to Update Timer Period Dynamically
void updateTimerPeriod(uint16_t new_period_ms) {
    __bic_SR_register(GIE); // Disable global interrupts

    base_period_ms = new_period_ms;  // Store new value
    
    TB3CTL &= ~MC_1;   // Stop the timer
    TB3CCR0 = 0;   // Clear CCR0 before updating
    TB3CCR0 = (base_period_ms * (SMCLK_FREQ / 8 / 1000));  // Correct scaling
    TB3CTL |= MC_1;    // Restart the timer in Up mode

    __bis_SR_register(GIE); // Re-enable global interrupts
}



// Main Function
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

    P3DIR |= 0xFF;  // Set P3.0 - P3.7 as outputs
    P3OUT = 0x00;   // Start with all LEDs off

    setupTimer();  // Initialize Timer3_B7

    P4DIR &= ~BIT1;      // S1 P4.1
    P4REN |= BIT1;
    P4OUT |= BIT1;
    P4IES |= BIT1;

    

    PM5CTL0 &= ~LOCKLPM5;  // Enable GPIOs

    //-- Setup IRQ
    P4IFG &= ~BIT1; // Clear interrupt flag
    P4IE |= BIT1;   // Enable interrupt for S1 (P4.1)

    __bis_SR_register(GIE | LPM0_bits);  // Enable global interrupts & low-power mode

    while (1) {
         
    }
}

// Timer3_B7 ISR (Handles Pattern Updates)
#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_TB3_CCR0(void) {
    static uint8_t pattern_step = 0;  // Keeps track of steps within base period
    switch (current_pattern) {
        case 0:                         // Static Pattern
            P3OUT = 0b10101010;
            break;
        
        case 1:                         // Toggle Pattern
            toggle_state ^= 0xFF;   // Flip bits
            P3OUT = toggle_state;
            break;
        
        case 2:                         // Up Counter
            P3OUT = counter;
            counter++;
            break;
        
        case 3:                         // In & Out
            P3OUT = steps[step_index];
            step_index = (step_index + 1) % 6;
            break;
    }

    /*    
    if (current_pattern == 2) {  
        if (pattern_step % 2 == 0) {  // Only update every other ISR call (0.5× base period)
            P3OUT = counter;  // Write counter value to LEDs
            counter++;  // Increment counter
        }
        pattern_step++;  // Increment step counter
    }
    */

    TB3CCTL0 &= ~CCIFG;  // Clear interrupt flag
}


#pragma vector = PORT4_VECTOR
__interrupt void ISR_Port4_S1(void) {
    current_pattern = (current_pattern + 1) % 4; // Cycle through 4 patterns (0-3)
    P4IFG &= ~BIT1;  // Clear interrupt flag
}