#include <fw_hal.h>

#include "hw.h"

// game state
uint8_t thing_sel = 1; // thing selector, 1 = left, 2=right
uint8_t house = 2;  // how many LEDs in house
uint8_t food = FIELD_M; // food position, one of MOUTH_ or FIELD_ costants
uint8_t pause_move = 0;
volatile uint8_t need_move = 0;
volatile uint8_t need_thing_switch = 0;
volatile uint8_t button_pressed = 0;
volatile uint8_t cs_timer = 1; // centisecond (1/100) timer

// interrupt handler state
uint8_t led_phase = 0;
uint16_t count_thing_switch = 1;
uint16_t count_move = 1;
uint16_t count_cs_timer = 1;
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

  // deboumce timer only counts if button is released
  if (button_debounce && nBUTTON()) { button_debounce--; }

  if (cs_timer) {
    // Music playing
    if (!--count_cs_timer) {
      count_cs_timer = TIMER0_HZ / 100;
      cs_timer--;
    }
  } else {
    // Music not playing. Run timers for main game loop
    if (!--count_thing_switch) {
      count_thing_switch = TIMER0_HZ * 2;
      need_thing_switch = 1;
    }

    if (!--count_move) {
      count_move = TIMER0_HZ / 3;
      need_move = 1;
    }
  }
}

// Button press handler
INTERRUPT(Int4_Routine, EXTI_VectInt4) {
  // register button press
  if (!button_debounce) {  button_pressed = 1; };
  button_debounce = TIMER0_HZ / 100;
  // mix our timer value into random state
  rng_state ^= TL0;
}


void beep_with_div(uint8_t duration_cs, uint16_t div) {
  set_beeper(div);
  cs_timer = duration_cs;
  while (cs_timer) {};
  set_beeper(0);
}

#define BEEP(freq, duration_ms) beep_with_div( (duration_ms)/10, BEEPER_HZ_TO_DIV((freq)))
#define SLEEP(duration_ms) beep_with_div( (duration_ms)/10, 0 )

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
        // blink both eyes
        if (food) {
          food = 0; thing_sel = 0;
        } else {
          food = EYE_R; thing_sel = 1;
        };
      }
    }
    button_pressed = 0;

    // Game start!
    house = 0;
    food = FIELD_M;
    pause_move = 5;
    thing_sel = 1 + random8() % 2;

    // starting tune
    BEEP(500, 100);
    BEEP(800, 100);
    BEEP(1000, 100);

    button_pressed = 0;
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
          if (pause_move) { pause_move--; break; }
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

        if (((food == MOUTH_L) && (thing_sel == 1)) ||
            ((food == MOUTH_R) && (thing_sel == 2))) {
          food = 0; // Food was eaten, hide it.
          // Got it!
          house++;
          BEEP(1000, 100);  SLEEP(100);
          BEEP(1500, 100);  SLEEP(100);
          BEEP(1500, 100);
          // Make a new food in center, and make it not move for a short bit.
          food = FIELD_M;
          pause_move = 3;
        } else {
          // missed it!
          if (house > 0) { house--; }
          BEEP(300, 200);  SLEEP(200);
          BEEP(200, 200);  SLEEP(200);
          BEEP(100, 200);
        }

        button_pressed = 0;
        if (house >= 4) { break; }// game won!
      }
    }

    // game won!
    BEEP(1000, 100);  SLEEP(100);
    BEEP(1200, 200);  SLEEP(100);
    BEEP(1400, 200);  SLEEP(100);
    BEEP(1600, 200);

    while (!button_pressed) {
      if (thing_sel == 1) {
        food = MOUTH_L; thing_sel = 2;
      } else {
        food = MOUTH_R; thing_sel = 1;
      };
      house = (house >= 5) ? 0 : (house + 1);
      SLEEP(100);
    }

    button_pressed = 0;
    // loop again and restart
  }
}
