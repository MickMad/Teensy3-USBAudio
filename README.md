Teensy3-USBAudio
================

Transform a Teensy 3.0 in a USB compliant audio device


Software part
-------------------------

The `software` folder contains modified versions of some Teensy 3.0 USB core library.

First, in `/hardware/teensy` there's the board.txt file which has some added lines to properly show the "Audio" USB type in the Arduino IDE;

In `/hardware/teensy/teensy3` there are some USB core library files for the Teensy 3.0 with my Audio library patch.

The `usb_audio.c` and `usb_audio.h` files are the real Audio library, which is going to handle all the audio interface relevant stuff.

The `usb_desc.c` and `usb_desc.h` files contain USB descriptors-related stuff; some configuration stuff is in `usb_desc.h`, while the whole Audio interface descriptor is in `usb_audio.c`.

The `usb_dev.c` and `usb_dev.h` files contain low level USB control stuff; the only "major" modification until now is that there's no toggle between Data0 and Data1 PIDs when sending packets on the Audio Streaming interface, because Isochronous transfers only happen to use Data0 PIDs.

The `usb_inst.cpp` file contains all the USB application-specific library instances to be used in the sketches.

In `sketch` you'll find a sketch folder, which contains the sketch `BlinkAndTalk.ino`; this sketch instantiates an IntervalTimer, then waits until the host requests the Audio Interface to stream data; then it starts the timer to generate samples and streams them via USB.

I know the name BlinkAndTalk doesn't really fit the Audio functionality, but that started as a pure USB initialization debugging sketch that blinked and "talked" via SPI to an Arduino that was used as a Serial-to-USB converter, to let me know what was actually happening during the USB initialization and interrupt routines :D

Right now, the descriptor configures the Teensy as a device called Teensy Audio (but it may show as Teensyduino MIDI on Linux, don't know why), and it will show up as a Mono Microphone input, 8KHz , 16 bits per sample.
The sketch SHOULD be sending a 1 KHz square wave, although recording it in Audacity @ 8 KHz shows a 1KHz carrier with a 4 KHz triangle over it, even if a USB packet analyzer shows the same data you see in the sketch (0x8000 - 0x7fff).
I still got to understand why this happens; I've tried filling the packets LSB first, MSB first, don't-care-just-write-stuff first, I just can't get it to show as a square wave. Though, if you record in Audacity @ 44100 or whatever it's not 8KHz, it shows a nice square wave. Gotta investigate further.

All the stuff inside the software folder must be placed in the same directory found in your arudino folder.

***WARNING: MAKE A BACKUP COPY OF THOSE FILES IN YOUR ARDUINO FOLDER BEFORE MAKING ANY CHANGES.***

I know my mods are harmless, and that they are never ever used unless you explicitly set the Audio USB Type when compiling, but you should backup anyway, I don't want to be blamed for anything :V