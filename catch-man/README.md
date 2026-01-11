# "catch-man" soldering game

# Part list

### STC8G1K08A-36I-DIP8

In 2025-12, bought 10x for $3.44 (free choice shipping): [listing](https://www.aliexpress.us/item/3256807473048659.html)

STC8G1K17A-36I-DIP8 would be even better (more FLASH for program), but it was much more expensive.

### UI

- 12x 3mm/5mm LED (red/yellow/green recommended, no blue or white)
- 1x "12x12mm" tactile button
- 1x slide switch (SS12D00 or equivalent, 3 pins @ 2.5mm/2.54mm spacing)
- Buzzer: 2 options, need to test
  - "9024" magnetic (4mm pin spacing),
    [passive buzzer 16ohm 3v](https://www.aliexpress.us/item/3256804172612082.html), $2.16 for 10x in 2025-12
  - unknown, possibly 1508 (15mm dia, 10mm pin spacing) - got sent by mistake
- CR2032, "shell" holder (BS-2032A but not all listing say this, look for 20mm inter-pin distance)

### Misc

- 2N2222 (or any other NPN one). I put metal can (TO-18) footprint because I have huge bag of those.
- 1x DIP-8 socket
- 4x 1/8W resistors for LEDs... Perhaps 100 ohm? Need to test brightness, depends on color
- 1x 1/8W resistor for transistor... anything from 500 to 15K will work, perhaps 1K or 4.7K?
- 1x 0.1u capacitor (disc, with 2.54mm pin spacing)

### Programming

You need to program STC8 chips. They accept regular serial TTL data, so you can get away with generic usb-serial.
The chip listens to instructions for first few mS after power-on, so you'll need a way to temporary disconnect
GND.

Those can be programmed before insertion, or you can use test clip for in-circuit programming
(the LEDs are connected so this is possible)

The easiest way to program is to use `pio run -t upload` in the firmware dir.
And alternative is to install stcgal and execute:
```
stcgal.py -P stc8g -p /dev/ttyUSB0 -t 11059 -a firmware/firmware.hex
```

# Testing/orders

### Revision 1

Ordered from JLCPCB 2025-12-24
- 5x, green color, "Lead-free HASL", everything else default
- Cost:
   - Merchandise Total:  $10.30
   - Shipping Estimate:  $10.90
   - (had some coupons which paid for it, would be about $22.50 otherwise)

Notes for next rev:
- Mark resistor values (since we have 2 kinds)
- Put "catch-man" in large font on front side
- Set "thing" outlines to be much thicker
- Re-route front traces to not intersect house silkscreen.. in general, move more stuff to the back, via's are neat
- Tested green LED + 68 ohm resistors.. looks good enough (at least on 3.3V supply)
- For next time: the timer output is not hooked up to speaker
- For next time: programming pin is hooked to speaker. When speaker is bypassed, this makes programming impossible
- Speaker transistor may be redundant? Outputs claim 20mA, and you are not supposed to draw that much from CR2032...
  Directly-driven piezo speaker sounds just fine from 3.3V
- Can we move eyes be different? rectangular LED, or bigger, or smaller...
