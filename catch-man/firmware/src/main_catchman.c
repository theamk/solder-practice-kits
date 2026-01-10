#include <fw_hal.h>
#include <stdio.h>

// pinout:
//   P32, P33, P54, P55 - LED charlielex
//   P30 - button (to GND)
//   P31 - buzzer (active high)

// Set LED via charlieplexing
// Takes number 0..15. treats as 2x 2bit vals: aabb, aa=+pin, bb=-pin
// So numbers 0 (0b0000), 5(0b0101), 10(0b1010), 15(0b1111) all mean "all off"
// (note those nmbrs are unrelated do D1..D12 on schematics)
void set_led(uint8_t val) {
  // Each port is controlled by 3 registers: Pn, PnM0, PnM1. The modes are:
  //  (PnM1=0, PnM0=0) -> QBD/Quasi-bidirectional - we don't use it
  //  (PnM1=0, PnM0=1) -> push/pull, with Pn=1 the only way to output strong pull-up
  //  (PnM1=1, PnM0=0) -> high-Z, used for unconnected pins
  //  (PnM1=1, PnM0=1) -> open drain
  
  // Turn off everything, avoiding pulses
  // (except P3.1, buzzer, which must be always in push-pull)
#define P3_PP_MASK  (1<<1)
  
  // First, set M1=1. Hi-Z pins keep being Hi-Z, push-pull becomes "open drain" (which disables strop pull-up)
  P3M1 = 0xFF ^ P3_PP_MASK; P5M1 = 0xFF;
  // Then, clear M0=0. Now everything is Hi-Z, we can manipulate pins in any way with no danger of glitches
  P3M0 = P3_PP_MASK; P5M0 = 0;
  // Finally, reset all to 1, for consistency. Use bitset on P3 to avoid messing with timer.
  P32 = 1;  P33 = 1;  P5 = 0xFF;

  // Special case common value
  if (val == 0) { return; }
  
  // Lower bits define pin to set to negative. We use open drain mode for less code
  switch (val & 3) {
  case 0: P3M0 |= (1<<2); P32 = 0; break;
  case 1: P3M0 |= (1<<3); P33 = 0; break;
  case 2: P5M0 |= (1<<4); P54 = 0; break;
  case 3: P5M0 |= (1<<5); P55 = 0; break;    
  }
  // Nextg 2 bits define pin to set to positive. We use push/pull, and set PnM0 first so we don't glitchinto QBD mode
  switch ((val >> 2) & 3) {
  case 0: P3M0 |= (1<<2); P3M1 &=~ (1<<2); break;
  case 1: P3M0 |= (1<<3); P3M1 &=~ (1<<3); break;
  case 2: P5M0 |= (1<<4); P5M1 &=~ (1<<4); break;
  case 3: P5M0 |= (1<<5); P5M1 &=~ (1<<5); break;
  }
}

#define nBUTTON()   P30

void hw_init(void) {
  P3 = 0xFF ^ (1<<1); // P3.1 (buzzer) off, rest high
  P3PU = (1<<0); // P3.0 (button) has 4.7K pullup  
  set_led(0);  // set rest of pins
}

// Test sequence
const uint8_t field[] = {
  1, 2, 3, 4, 6, 7, 8, 9, 11, 12, 13, 14
};

void main(void) {
  hw_init();
  
  while (1) {

    // run LED through the field
    for (uint8_t i=0; i<sizeof(field); i++) {
      set_led(field[i]);
      // temp - debug programmer
      //P31 = (i & 8);

      // While the button is held, don't move, beep
      while (nBUTTON() == 0) {
        P31 = 1;
        SYS_Delay(1);
        P31 = 0;
        SYS_Delay(1);
      }
      SYS_Delay(200);
    }

    // A longer delay and loop again
    SYS_Delay(1000);
  }
}
