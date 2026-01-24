#include <fw_hal.h>

#include "hw.h"

// game state
uint8_t thing_sel = 1; // thing selector, 1 = left, 2=right
uint8_t house = 2;  // how many LEDs in house
uint8_t food = FIELD_M; // food position, one of MOUTH_ or FIELD_ costants
uint8_t pause_move = 0;
uint8_t level = 0;
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

__CODE const uint8_t victory_led[] = {
  EYE_L, FIELD_TL, FIELD_TR, EYE_R, 0,
  MOUTH_L, FIELD_M, MOUTH_R, 0, 0,
  FIELD_BL, FIELD_BR, 0, 0, 0,
  HOUSE_1, HOUSE_2, HOUSE_3, 0, 0
};

INTERRUPT(Timer0_Routine, EXTI_VectTimer0) {
  // Set LED (we can show only 1 at a time, so use fixed schedule)
  uint8_t led = 0;

  if (thing_sel >= 10) {
    // Special "win condition" blink
    if (++led_phase > 4) led_phase = 0;
    led = victory_led[(thing_sel - 10) * 5 + led_phase];
  } else {
    // Normal operation
    switch (++led_phase) {
    case 1: if (thing_sel) { led = (thing_sel == 2) ? EYE_R : EYE_L; }; break; // eye
    case 2: if (house >= 1) { led = HOUSE_1; }; break;
    case 3: if (house >= 2) { led = HOUSE_2; }; break;
    case 4: if (house >= 3) { led = HOUSE_3; }; break;
    default: // food
      led_phase = 0;
      led = food;
    }
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
      switch (level) {
      default: // level 0, treat same as 1
      case 1: count_move = TIMER0_HZ / 3; break;
      case 2: count_move = TIMER0_HZ / 4; break;
      case 3: count_move = TIMER0_HZ / 7; break;
      }

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

// A list of possible moves *for random)
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

__CODE const uint8_t food_sequence[] = {
  FIELD_M, FIELD_TR, MOUTH_R, FIELD_BR,
  FIELD_M | 0x80, // displays same as FIELD_M
  FIELD_TL, MOUTH_L, FIELD_BL,
};


void move_food() {
  if (level == 0) {
    // simpler version: move in predictable order
    for (uint8_t i=0; i<(sizeof(food_sequence) - 1); i++) {
      if (food_sequence[i] == food) {
        food = food_sequence[i + 1];
        return;
      };
    }
    food = food_sequence[0];
    return;
  }

  // Move in the random order
  for (uint8_t i=0; i<sizeof(food_moves); i++) {
    // Find the right sequence
    if (food_moves[i] != food) {
      while (food_moves[i] != 0) { i++; }
      continue;
    }
    // Count number of possible moves
    uint8_t cnt = 0;
    for (const uint8_t* p = &food_moves[i+1]; *p; p++, cnt++) {};
    // choose a move
    uint8_t idx = random8() % cnt;
    food = food_moves[i + 1 + idx];
    break;
  }
}


void main(void) {
  hw_init();

  while (1) {
    // pre-game

    if (level == 0) {
      // game welcome loop, 1st run
      house = 0;
      thing_sel = 0;
      button_pressed = 0;

      while (!button_pressed) {
        if (need_move) {
          need_move = 0;
          // blink both eyes
          if (food) {
            food = 0; thing_sel = 0;
          } else {
            food = EYE_R; thing_sel = 1;
          };
          house = (house > 4) ? 0 : (house + 1);
        }
      }
    } else if (level > 3) {
      // game finished loop!
      // Show flashing lights until power down
      uint8_t sound_on = 0;
      thing_sel = 10;

      while (1) {
        // Scroll special blink modes
        if (need_move) {
          need_move = 0;
          thing_sel++;
          if (thing_sel >= 14) { thing_sel = 10; }
        }

        // Play random sound every time button is pressed
        if (button_pressed) {
          button_pressed = 0;
          sound_on = 1;
          switch (random8() % 4) {
          case 0: set_beeper(BEEPER_HZ_TO_DIV(200)); break;
          case 1: set_beeper(BEEPER_HZ_TO_DIV(440)); break;
          case 2: set_beeper(BEEPER_HZ_TO_DIV(880)); break;
          default: set_beeper(BEEPER_HZ_TO_DIV(1200)); break;
          }
        } else if (sound_on && !button_debounce) {
          // button released + debounce interval passed
          sound_on = 0;
          set_beeper(0);
        }
      }

    } else {
      // next lever start loop, after level-up
      uint8_t house_skip = 0;
      while (!button_pressed) {
        // Update rate depends on level
        if (!need_move) { continue; }
        need_move = 0;

        if (thing_sel == 1) {
          food = MOUTH_L; thing_sel = 2;
        } else {
          food = MOUTH_R; thing_sel = 1;
        };
        // House goes up to (new) level, stays there, then resets
        if (house < level) {
          house++;
        } else if (house_skip < 2) {
          house_skip++;
        } else {
          house = 0;
          house_skip = 0;
        }
      }
    }

    button_pressed = 0;

    // Game start!
    house = 0;
    food = FIELD_M;
    pause_move = 3;
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
        if (pause_move) {
          pause_move--;
        } else {
          move_food();
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

    level++;
    button_pressed = 0;
    // loop again and restart
  }
}
