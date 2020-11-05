# dance-pad
Code and schematics for Dance Dance Revolution - DDR - FSR sensor upgrade.

# Plan
Use one MEGA238P per arrow to read & convert the 4 FSR values into a pressed/not-pressed signal. 
Use 25 SK6812 LEDs to light up the panel when pressed and show animations when idle.

# Parts
Arduino-pro-mini clone
MCP2125 - CAN - interface with Teensy
Teensy 3.2 - interface with DDR cabinet
SK6812 - 'Neopixel' LEDs
FSR - x32 - 4 per tile

# Why
I have a DDR cabinet, and I am tired of dealing with stuck sensors. This will let me 
calibrate the dance pad on the fly and never have to deal with it again. I've 
wanted to create a CANBus network for a while, and this gives me an excuse to use 
the box of pro-mini clones that I've had for a while now.