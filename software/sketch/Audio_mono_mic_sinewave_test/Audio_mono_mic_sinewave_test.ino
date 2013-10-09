#define MAX_BUFFER_SIZE 96
#define SAMPLING_FREQ 48000
#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)
#define NOTE_POT A0
#include "notes.h"
#include "sine_lut_full.h"

uint16_t audioBufferFirst[MAX_BUFFER_SIZE];
uint8_t bufferLength;
uint8_t sampleSize;
uint8_t sampleCounter = 0;
uint8_t led = 13;
uint8_t wait_for_SOF = 1;
unsigned short phase;
unsigned short tuning;
float freq;
bool active = false;

void initStuff() {
	sampleCounter = 0;
	sampleSize = 2; 					// 2 bytes per sample (16-bit)
	bufferLength = MAX_BUFFER_SIZE; 	// SAMPLING_FREQ/1000 :: samples per packet
	
	uint8_t i;
	for (i=0;i<MAX_BUFFER_SIZE;i++) {
		audioBufferFirst[i] = 0x0000;
	}
	
	phase=0;
	tuning=0;
}

void setup() {
	pinMode(led,OUTPUT);
	initStuff();
	Audio.begin();
}

void loop() {
	if (active) {
		if(!Audio.getAlternateSetting()) {
			active = false;
			//blink();
			//blink();
		} else {
			if (wait_for_SOF) {
				sendBuffer();
				sampleCounter = 0;
				wait_for_SOF = 0;

				// Get the Frequency
				// Play A 440 Hz	// Map the pot value to the freq table
				freq = 440;			// freq_table[map(analogRead(NOTE_POT), 0, 1023, 0, 85)];

				// tuning is equilavent to the phase step
				// phase_step = table_size * (desired_freq/sampling_freq)
				// Not sure why table_size is bit depth instead of sine freq table size
				tuning = 65536*(freq/(SAMPLING_FREQ*2));
			} else {
				doAudioStuffISR();
			}
		}
	} else {
		if (Audio.getAlternateSetting()) {
			active = true;
			//blink();
		}
	}
}

void sendBuffer() {
	Audio.sendAudio(audioBufferFirst,bufferLength*sampleSize);
}

void doAudioStuffISR() {
	uint8_t translation = 128; // Half the max amplitude

	if (sampleCounter == bufferLength) {
		wait_for_SOF = 1;
		return;
	}

	phase += tuning;

	// Table sizes available: 8, 10, 12, 13
	// temp should be phase>>(16-num_bits_to_represent_num_points_in_table)
	// to change table size, just change LUT_INDEX_BITS in sine_lut_full.h
	uint16_t temp = phase>>(16-LUT_INDEX_BITS);
	if (temp>=LUT_INDEX_MAX) temp-=LUT_INDEX_MAX;

	audioBufferFirst[sampleCounter*2] = sineTable[temp]-translation;
	audioBufferFirst[(sampleCounter++)*2+1] = sineTable[temp]-translation;
}
void blink() {
	digitalWrite(led,HIGH);
	delay(50);
	digitalWrite(led,LOW);
	delay(50);
}