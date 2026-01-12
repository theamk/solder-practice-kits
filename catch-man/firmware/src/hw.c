#include <fw_hal.h>

#include "hw.h"

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
  // Next 2 bits define pin to set to positive. We use push/pull, and set PnM0 first so we don't glitchinto QBD mode
  switch ((val >> 2) & 3) {
  case 0: P3M0 |= (1<<2); P3M1 &=~ (1<<2); break;
  case 1: P3M0 |= (1<<3); P3M1 &=~ (1<<3); break;
  case 2: P5M0 |= (1<<4); P5M1 &=~ (1<<4); break;
  case 3: P5M0 |= (1<<5); P5M1 &=~ (1<<5); break;
  }
}

void hw_init(void) {
  SFRX_ON(); // Set P_SW2.EXFR, enabling access to special function register (XFR)
  // On devices with <64KB if RAM, this has no downside, but SDK often turns it off for some reason

  // GPIO
  P3 = (0xFF ^ (1<<1)) | (BEEP_IDLE << 1); // P3.1 (buzzer) idle, rest high
  P3PU = (1<<0); // P3.0 (button) has 4.7K pullup
  set_led(0);  // set rest of pins

  // Timer 0 is for timing and LED refresh, runnng at TIMER0_HZ
  // Timer 1 is for beeper only, running at 2x beeper frequency

  TMOD = 0; // T0 and T1 are in timer mode 0 (16 bit auto-reload)
  AUXR |= (1<<6)|(1<<7); // T0x12=1, T0x12=1 -> clock source is SYSClk/1
  ET0 = 1; ET1 = 1; // interrupts enabled on both
  uint16_t val = 65536 - __CONF_FOSC/TIMER0_HZ;
  TH0 = val >> 8; TL0 = val & 0xFF;

  SFR_DUAL_SET(IP, IPH, 1, 1); // T0 is priority "1" (lower), a small delay is OK
  SFR_DUAL_SET(IP, IPH, 3, 3); // T1 is priority "3" (highest), we don't want our sound to be choppy

  TR0 = 1; // timer 0 on
  TR1 = 0; // timer 1 off

  INTCLKO |= (1<<6); // enable INT4 interrupt, triggered by button (for random update)
  SFR_DUAL_SET(IP2, IP2H, 4, 2); // priority 2 (higher)

  EA = 1; // enable interrupts

  set_beeper(0);
}


static uint8_t beep_val = 0;
INTERRUPT(Timer1_Routine, EXTI_VectTimer1) {
  BEEP_PIN = (++beep_val) & 1;
}

// Set beeper with specific timer divisor
// (this way there is no need to divide at runtime)
void set_beeper(uint16_t div) {
  TR1 = 0; BEEP_PIN = BEEP_IDLE; // timer off, output off
  if (div == 0) { return; }
  uint16_t val = 65535 - div + 1;
  TH1 = val >> 8; TL1 = val & 0xFF;
  TR1 = 1;
}


uint16_t rng_state = 0x1234;
uint8_t random8() {
  // 16-bit xorshift
  // http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
  rng_state ^= (rng_state << 7);
  rng_state ^= (rng_state >> 9);
  rng_state ^= (rng_state << 8);

  // auto-fix: we don't want to be stuck @ 0
  if (!rng_state) { rng_state = 0xABCD; }

  return rng_state & 0xFF;
}
