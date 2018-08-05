# Home-LED-Strip-Control
Full write up:
So this write up makes a few (easily Googled) assumptions: You know how to control Neopixels with the Arduino IDE, you know how to set up a Raspberry Pi with Raspbian, and you're at least passingly familiar with IFTTT + Home. 
First I had to pick a MCU. I used the Adafruit Feather HUZZAH board. Not because of any specific function of this board, but because it was the only 3.3V MCU I had laying around. This voltage is important so you can talk to the Pi, which is 3.3V. You only need an MCU that runs at 3.3V and can produce 1 PWM signal and can use at least 1 Software Serial connection. 
I programmed this board to control a strip of Neopixels and accept custom commands (set color, affect, etc.) over SoftSerial. I used the FastPixel library since I'm controlling a lot of pixels. If you're not familiar with SoftSerial you can learn more here. Essentially it allows you to use nearly any GPIO as a TX and RX connection (the 2 basic pins needed for serial communication). Please be warned that the ESP8266 needs a special library to use SoftSerial. 
The reason I used SoftSerial instead of the hardware one was so I could debug and flash the MCU over USB to my computer. You could technically use the hardware serial instead, but then you couldn't easily reprogram the board/ debug over USB. 
The MCU will communicate over SoftSerial to the Pi. Now for Pi time!
To program the Pi's GPIO and to communicate with our Home, we need a special IDE. This IDE is online and is called Particle. Install Particle on your Pi and now you can program the GPIO from the particle build website (build.particle.io/build). No we can program the Pi's hardware serial (note you'll have to enable serial in the rasp-config) to send our color/ affect commands. 
Cool. Now the Pi can tell the ESP8266 what color/ affect to show, but how do we tell the Pi what to do? Particle can also react to triggers from IFTTT. 
In the Particle code you can "subscribe" to events in your code. Basically when an event is published, a function in your code will be called. Look at this post for a more detailed look at how to get Particle and IFTTT to communicate. 
Now that Particle is set up to react to IFTTT, now its time to get IFTTT to react to your Google Home. 
I created an account on IFTTT and created my own applet. This applet's "IF" is a Google assistant command, it's "THAT" is Particle. Whenever I say "Hey Google Set String $" it will post an event called "Static_String" which will in turn call the particle function "setStatic" (this is my function and event name, you can name yours how ever you want). That "$" is whatever word(s) I say after the initial "Hey Google Set String", in this case its a color or an affect (Ex. "Hey Google Set String Red"). 

You can add a lot of functions an affects to this system. 
Hope this write helps y'all create something amazing!
