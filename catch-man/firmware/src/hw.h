#ifndef HW_H
#define HW_H
// Hardware definitions

// pinout:
//   P32, P33, P54, P55 - LED charlielex
//   P30 - button (to GND)
//   P31 - buzzer (active high)

// LED index constants for SET_LED
#define EYE_L    6    // eye, left and right
#define EYE_R    12
#define MOUTH_L  9    // LED in the mouth (left/right)
#define MOUTH_R  3
#define FIELD_TL 4    // field: top/bottom left/right
#define FIELD_TR 13
#define FIELD_BR 8
#define FIELD_BL 1
#define FIELD_M  2    // field: middle
#define HOUSE_1  14   // house LEDs
#define HOUSE_2  11
#define HOUSE_3  7


// Set LED via charlieplexing
// Takes number 0..15. treats as 2x 2bit vals: aabb, aa=+pin, bb=-pin
// So numbers 0 (0b0000), 5(0b0101), 10(0b1010), 15(0b1111) all mean "all off"
// (note those nmbrs are unrelated do D1..D12 on schematics)
void set_led(uint8_t val);


#define nBUTTON()   P30
#define BEEP_PIN    P31

#define BEEP_IDLE   0   // 0 if transistor installed, 1 if bypassed

// Timer0 runs at constant speed
#define TIMER0_HZ   1000

void hw_init();

// Set beeper with specific timer divisor, or 0 to disable
// (this way there is no need to divide at runtime)
#define BEEPER_HZ_TO_DIV(x)   (__CONF_FOSC/2/((x)))
void set_beeper(uint16_t div);

// 8-bit random number. Starts predicatably, but every button press is mixed in.
uint8_t random8();
extern uint16_t rng_state;


// sdcc requires all interrupts to be declared (but not neccesary defined) in main()
extern INTERRUPT(Timer1_Routine, EXTI_VectTimer1);

#endif
