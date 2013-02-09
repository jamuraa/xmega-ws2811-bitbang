
# Pretty Lights controller for the XMega

I recently got a string of WS2811 lights, and a Atmel Xplained A3BU board.  Like peanut butter and chocolate, I put them together to make a controller.

The code is based on the FastSPI library for the bit-banging, but it's not rocket science.  I tweaked the NOP timing for the faster clock speed of the XMega processor.

Right now the demo toggles the red, green, and blue full on and off with each button press of SW0, SW1 and SW2.  It should be pretty easy to see how to change what happens though, the led_* functions should be pretty easy to figure out.

I wasn't trying to be efficient (in power or code size), but just wanted to get something working in an hour or so.

## Short list of non-exaustive TODOs

* Use the LED Display on the XMega to show the current RGB state, and maybe implement a menu.
* [Light switch rave](http://www.homestarrunner.com/sbemail45.html) functions.
* Control the LEDs using a serial console.
 
