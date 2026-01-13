# "catch-man" game

# Part list

### STC8G1K08A-36I-DIP8

In 2025-12, bought 10x for $3.44 (free choice shipping): [listing](https://www.aliexpress.us/item/3256807473048659.html)

STC8G1K17A-36I-DIP8 would be even better (more FLASH for program), but it was much more expensive.

### UI

- 12x 3mm/5mm LED (red/yellow/green recommended, no blue or white)
- 1x "12x12mm" tactile button
- 1x slide switch (SS12D00 or equivalent, 3 pins @ 2.5mm/2.54mm spacing)
- Piezo buzzer (passive, "1407", pin spacing 7.5mm or 7.6mm)
- CR2032, "shell" holder (BS-2032A but not all listing say this, look for 20mm inter-pin distance)

### Misc

- 1x DIP-8 socket
- 4x 1/8W resistors for LEDs... Perhaps 68 ohm for green, 100 ohm for red (maybe?)
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
- See "Changes" for Revision 2 below
- Tested green LED + 68 ohm resistors.. looks good enough (on battery and 3.3v supply)
- For next time: the timer output is not hooked up to speaker
- Programming pin is hooked to speaker. When speaker is bypassed, this makes in-circuit programming impossible
  Either move to a different pin, or add a "mute" switch
- Speaker transistor is redundant, tested with directly-driven piezo speaker, sounds just fine
    - Piezo speaker model unknown, measured: D=14mm H=7mm Spacing=7.6mm C=13.6nF
    - Dynamic speaker (say 16 ohm) is 190mA, way too much for CR2032 anyway
- Let's make eyes be different - rectangular LED, or bigger, or smaller...

### Revision 1.1

Changes:
- (TODO) Put "catch-man" in large font on front side
- Set "thing" outlines to be much thicker
- Re-route front traces to not intersect silkscreen lines.. in general, move more stuff to the back, via's are neat
- Removed transistor, added mute switch instead
- Updated speaker spacing to 7.6mm
- Added a single mounting hole in the very center
