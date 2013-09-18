Teensy3-USBAudio
================

Collection of software and hardware design to transform a Teensy 3.0 in a USB compliant audio device


Software part:

The "software" folder contains modified versions of some Teensy 3.0 USB core library.
First , in /hardware/teensy there's the board.txt file which has some added lines to properly show the "Audio" USB type in the Arduino IDE;
In /hardware/teensy/teensy3 there are some USB core library files for the Teensy 3.0 with my Audio library patch.
The usb_audio.c and usb_audio.h files are the real Audio library, which is going to handle all the audio interface relevant stuff.
The usb_desc.c and usb_desc.h files contain USB descriptors-related stuff; some configuration stuff is in usb_desc.h, while the whole Audio interface descriptor is in usb_audio.c.
The usb_dev.c and usb_dev.h files contain low level USB control stuff; the only "major" modification until now is that there's no toggle between Data0 and Data1 PIDs when sending packets on the Audio Streaming interface, because Isochronous transfers only happen to use Data0 PIDs.
The usb_inst.cpp file contains all the USB application-specific library instances to be used in the sketches.

In "sketch" you'll find a sketch folder, which contains the sketch "BlinkAndTalk.ino"; this sketch instantiates an IntervalTimer, then waits until the host requests the Audio Interface to stream data; then it starts the timer to generate samples and streams them via USB.

I know the name BlinkAndTalk doesn't really fit the Audio functionality, but that started as a pure USB initialization debugging sketch that blinked and "talked" via SPI to an Arduino that was used as a Serial-to-USB converter, to let me know what was actually happening during the USB initialization and interrupt routines :D

Right now, the descriptor configures the Teensy as a device called Teensy Audio (but it may show as Teensyduino MIDI on Linux, don't know why), and it will show up as a Mono Microphone input, 8KHz , 16 bits per sample.§

The sketch SHOULD be sending a 1 KHz square wave, although recording it in Audacity @ 8 KHz shows a 1KHz carrier with a 4 KHz triangle over it, even if a USB packet analyzer shows the same data you see in the sketch (0x8000 - 0x7fff).
I still got to understand why this happens; I've tried filling the packets LSB first, MSB first, don't-care-just-write-stuff first, I just can't get it to show as a square wave.

Though, if you record in Audacity @ 44100 or whatever it's not 8KHz, it shows a nice square wave. Gotta investigate further.

All the stuff inside the software folder must be placed in the same directory found in your arudino folder.
WARNING: MAKE A BACKUP COPY OF THOSE FILES IN YOUR ARDUINO FOLDER BEFORE MAKING ANY CHANGES. I know my mods are harmless, and that they are never ever used unless you explicitly set the Audio USB Type when compiling, but you should backup anyway, I don't want to be blamed for anything :V

Hardware part:

The "hardware" folder contains some hardware (duh) modules that are going to be part of my DIY audio proto system.
The "guitar balanced driver x2" folder contains a slave board based on the LME49724 fully differential op-amp that is used to convert two unbalanced high impedance low level signals  (e.g. guitar/bass) to two balanced low impedance low level signals; necessary to properly feed fully differential ADCs.
The "input selector" folder  contains a master board, whose slave is the guitar balanced driver, based on the ADG333A  analog switches, that gets two balanced/unbalanced signals and routes them either directly to the output (e.g. the input is a line level signal) or through the slave board (e.g. the input is a guitar level signal). It is controlled by the BYPASS lines. The ENABLE control lines are optional lines used to turn on or off a channel's opamp on the slave board (useful for low power consumption).
The "volume control" folder contains a board based on the PGA4311 digital volume control IC, configured as a Stereo Differential IN / Stereo Differential OUT. There will be one volume control board between the input selector and the ADC, and one just after the DAC. They will be daisy-chained (although there's no expansion bus on this board, I'm going to add it).

TO DO:
HW:
Finish the design of an ADC/DAC board, based on WM8786 and WM8740 ICs;
Finish the design of a dual supply board for the analog boards, based on the MEJ1S0505SC DC-DC 5 V to +/5 V converter from Murata.
Find a good place to add a Teensy 3.0 header on one of these two boards.
SW:
Understand how to write properly formed audio data on an isocrhonous endpoint;
Implement audio OUT from the PC to the Teensy (and output it via PWM or I2S)
If needed, modify the packet structure to contain more than 64 bytes per packet, and to contain 16 bits words rather than 8.
