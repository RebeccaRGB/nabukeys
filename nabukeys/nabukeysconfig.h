#ifndef NABUKEYSCONFIG_H
#define NABUKEYSCONFIG_H

// Pin connected to MOSFET gate to control keyboard power.
#define NK_PIN_Q 2

// Pin connected to SN75176AP pin 1 (R). Keyboard serial RX.
#define NK_PIN_R 3

// Pin connected to SN75176AP pins 2 and 3 (#RE and DE).
// Low enables receive. High enables transmit.
#define NK_PIN_E 4

// Pin connected to SN75176AP pin 4 (D). Keyboard serial TX.
#define NK_PIN_D 5

#endif
