LED letter badge

# Part list

### LED matrix 5x7, with 1.9mm pixel (pretty small)

Either common anode or common cathode can work, based on software
- Pretty hard to source, I found 1 source on Aliexpress:
  ["10PCS x 1.9MM 5X7 White Blue White Red Common Cathode/Anode LED Dot Matrix Digital Tube Module 757AS 757BS LED Display Module"]([https://www.aliexpress.us/item/2251801143009542.html)
   - Sold by [KESUN KESUN](https://www.aliexpress.com/store/1100399907)
   - 2025-12 Price was $8.26(red), $11.02 (yellow), $10.83 (green) + shipping
     I paid $30.38 for 20x (Yellow Anode 757BY + Green Cathode 757AG)
- Other option is https://www.aliexpress.us/item/3256807420112720.html - same description, very similar price.
  I suspect it's the same store.

### STC8G1K17-38I-DIP16

In 2025-12, bought 5x for $4.00 (free "choice" shipping), but the listing is already gone:
- [STC8G1K17 STC8G1K17-38I DIP16 STC 8G1K17-38I Single Chip Microcontroller IC Controller Chip STC8G1K17-38I-DIP16 Original](https://www.aliexpress.us/item/3256807299744981.html)
- there is new one even cheaper (5x for $3.75)

### Mechanicals

- 3x buttons (Tactile Switch 3.5x6 2-pin DIP)
   - 2025-12 [example](https://www.aliexpress.us/item/3256801442870232.html?), $2.14/50x, free ship
- 1x slide switch (SS12D00 or equivalent, 3 pins @ 2.5mm/2.54mm spacing)
- Power:
   - For CR2032, "shell" holder (BS-2032A but not all listing say this, look for 20mm inter-pin distance)
   - For 3xAAA, PH2.0 connector + 3xAAA battery box with switch and PH2.0 connector

### Misc parts

- 1x DIP-16 socket
- 7x 1.8W resistors... Perhaps 100 ohm? Need to test brightness, depends on color
- 1x 0.1u capacitor (disc, with 2.54mm pin spacing)
- 2x small diodes (any will work, as long as they fit in the hole)

### Programming

You need to program STC8 chips. They accept regular serial TTL data, so you can get away with generic usb-serial.
The chip listens to instructions for first few mS after power-on, so you'll need a way to temproary disconnect
GND.

For advanced usage, you can get 4-pin 2.54mm clip and have auto-power-off circuit.

# Testing/orders

### Revision 1

Ordered from JLCPCB 2025-12-27
- 5x, green color, "Lead-free HASL", everything else default
- Cost:
   - Merchandise Total:  $3.10
   - Shipping Estimate:  $3.12  (was $7.28, but got some coupon)
   - Sales Tax:          $0.39
   - Grand Total:        $6.61
