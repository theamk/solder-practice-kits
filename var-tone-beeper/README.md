Learn-to-solder kit: var-tone-beeper
"variable tone beeper"
=====

Part list:
(prices as of 2025-03)

PCB 10x ($22.46/10x -> $2.24/each)
- black, lead-free

DIP-8 chip socket ($2.84/25 -> $0.11/each)
+ NE555P chip (sold as pair)
- socket optional but recommended
- https://www.aliexpress.us/item/3256805880822598.html
- note: TLC55CP provides a very quiet sound

23mm speaker (($7.50 + $3.16)/10 = $1.07/each)
- 1508/2308/3013 passive direct plug small speaker 8 ohm 1 watt foot distance 10m fingerprint lock speaker 23*8mm 10PCS
- https://www.aliexpress.us/item/2251832605561774.html

3xAAA PC mount battery box ($4.14/10x -> $0.41/each)
- https://www.aliexpress.us/item/3256806088417831.html

Potentiometer, PC mount vertical, 500K "A" taper ($3.12/10x -> $0.31 each)
- "R097 RK097 Precision Potentiometer Vertical Single Pole with Bracket Shaft 15mm Without Notches 3 Pins A5K 50K 100K 500K"
- https://www.aliexpress.us/item/3256806835661913.html
- Not the best - too tall. Would love to find lower profile.

Potentiometer knob ($1.45/20x -> $0.07/each)
- "WH148 Potentiometer Knob Kit Plastic Knobs Cap 15*17MM Shaft Hole 6MM AG2"
- https://www.aliexpress.us/item/3256805921143559.html

Resistor 1K 1/8w (or 1/8w)
- Stock now, future: https://www.aliexpress.com/item/3256802183266089.html

Capacitor 0.1uF, 5mm pin spacing

Button 14mm

Button cap (colored)

2x Screw M2x6 "ultra thin head" + 2x nut M2


total (excluding battery): ??

====

V1 notes:
- We require 1/8W resistor -> increase footprint to accept 1/4W as well
- Potentiometer lugs slots are too small.
- Holes for IC socket pretty small. Workable but maybe increase for beginners?
- Maybe decrease hole size around battery holder if we plan to keep M2 screws?
  (or keep it.. the current holes work great with M3 screws)
- Need to find log (A504) resistor and test it

