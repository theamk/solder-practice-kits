#include <fw_hal.h>

#include "hw.h"

// game state
uint8_t thing_sel = 1; // thing selector, 1 = left, 2=right
uint8_t house = 2;  // how many LEDs in house
uint8_t food = FIELD_M; // food position, one of MOUTH_ or FIELD_ costants
volatile uint8_t need_move = 0;
volatile uint8_t need_thing_switch = 0;
volatile uint8_t button_pressed = 0;

// interrupt handler state
uint8_t led_phase = 0;
uint16_t count_1s = TIMER0_HZ;
uint16_t count_thing_switch = 1;
uint16_t count_move = 1;
volatile uint8_t button_debounce = 0;

INTERRUPT(Timer0_Routine, EXTI_VectTimer0) {
  // Set LED (we can show only 1 at a time, so use fixed schedule)
  uint8_t led = 0;
  switch (++led_phase) {
  case 1: if (thing_sel) { led = (thing_sel == 2) ? EYE_R : EYE_L; }; break; // eye
  case 2: if (house >= 1) { led = HOUSE_1; }; break;
  case 3: if (house >= 2) { led = HOUSE_2; }; break;
  case 4: if (house >= 3) { led = HOUSE_3; }; break;
  default: // food
    led_phase = 0;
    led = food;
  }
  set_led(led);

  //
  if (!--count_1s) {
    count_1s = TIMER0_HZ;
    //house = (house == 3) ? 0 : (house + 1);
  }

  // Run timers for main game loop
  if (!--count_thing_switch) {
    count_thing_switch = TIMER0_HZ * 2;
    need_thing_switch = 1;
  }

  if (!--count_move) {
    count_move = TIMER0_HZ / 3;
    need_move = 1;
  }

  // deboumce timer only counts if button is released
  if (button_debounce && nBUTTON()) { button_debounce--; }
}

// Those should be in hw.c, but sdcc wants all handlers in main.c
static uint8_t beep_val = 0;
INTERRUPT(Timer1_Routine, EXTI_VectTimer1) {
  BEEP_PIN = (++beep_val) & 1;
}
INTERRUPT(Int4_Routine, EXTI_VectInt4) {
  // register button press
  if (!button_debounce) {  button_pressed = 1; };
  button_debounce = TIMER0_HZ / 100;
  // mix our timer value into random state
  rng_state ^= TL0;
}

// A list of possible moves.
// 0-delimited list of bytes. First byte is original position, rest are possible moves
__CODE const uint8_t food_moves[] = {
  FIELD_M,  FIELD_TR, FIELD_TL, FIELD_BR, FIELD_BL, 0,

  MOUTH_L, FIELD_TL, FIELD_BL, 0,
  MOUTH_R, FIELD_TR, FIELD_BR, 0,

  FIELD_TL, MOUTH_L, FIELD_M, FIELD_TR, 0,
  FIELD_TR, MOUTH_R, FIELD_M, FIELD_TL, 0,

  FIELD_BL, MOUTH_L, FIELD_M, FIELD_BR, 0,
  FIELD_BR, MOUTH_R, FIELD_M, FIELD_BL, 0
};


void main(void) {
  hw_init();

  while (1) {
    // pre-game
    house = 2;
    thing_sel = 0;
    button_pressed = 0;

    // game welcome loop

    while (!button_pressed) {
      if (need_move) {
        need_move = 0;
        food = food ? 0 : FIELD_M;
      }
    }
    button_pressed = 0;

    // Game start!
    house = 0;
    food = FIELD_M;
    thing_sel = 1 + random8() % 2;

    while (1) {
      // Game: switch thing periodically
      if (need_thing_switch) {
        need_thing_switch = 0;
        if (random8() < 80) {
          thing_sel = (thing_sel == 2) ? 1 : 2;
        }
      }

      // Game: move food
      if (need_move) {
        need_move = 0;

        for (uint8_t i=0; i<sizeof(food_moves); i++) {
          // Find the right sequence
          if (food_moves[i] != food) {
            while (food_moves[i] != 0) { i++; }
            continue;
          }
          // Count number of possible moves
          uint8_t cnt = 0;
          for (uint8_t* p = &food_moves[i+1]; *p; p++, cnt++) {};
          // choose a move
          uint8_t idx = random8() % cnt;
          food = food_moves[i + 1 + idx];
          break;
        }
      }

      // Game: handle button
      if (button_pressed) {
        button_pressed = 0;

        if (((food == MOUTH_L) && (thing_sel == 1)) ||
            ((food == MOUTH_R) && (thing_sel == 2))) {
          // Got it!
          if (house < 3) { house++; }
        } else {
          // missed it!
          if (house > 0) { house--; }
        }
      }
    }
  }
}
