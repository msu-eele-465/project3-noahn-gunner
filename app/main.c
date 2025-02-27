#include "RGB_LED.h"

int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Disable GPIO high-impedance mode
    PM5CTL0 &= ~LOCKLPM5;

    setupRGBLED();  // Initialize RGB LED

    // Enable low-power mode with global interrupts
    _BIS_SR((LPM0_bits | GIE));

    while (true) {}
}
