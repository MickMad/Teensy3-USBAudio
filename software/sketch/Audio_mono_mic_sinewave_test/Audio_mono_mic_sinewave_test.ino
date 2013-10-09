#define MAX_BUFFER_SIZE 16
#define SAMPLING_FREQ 16000
#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)
#define NOTE_POT A0
#include "notes.h"
#include "sine_lut_full.h"

IntervalTimer PIT = IntervalTimer();
uint16_t audioBufferFirst[MAX_BUFFER_SIZE];
uint16_t audioBufferLast[MAX_BUFFER_SIZE];
uint8_t bufferLength;
uint8_t sampleSize;
uint8_t sampleCounter = 0;
uint8_t led = 13;
uint8_t counter = 0;
uint8_t wait_for_SOF = 1;
uint8_t readFirstBuffer = 1;
unsigned short phase;
unsigned short tuning;
float freq;
bool active = false;


void initStuff() {
	sampleCounter = 0;
	sampleSize = 2; //2 bytes per sample (16-bit)
	bufferLength = 16; // SAMPLING_FREQ/1000 :: samples per packet

	uint8_t i;
	for (i=0;i<MAX_BUFFER_SIZE;i++) {
		audioBufferFirst[i] = 0x0000;
	}
}

void setup() {
	pinMode(led,OUTPUT);
	initStuff();
	Audio.begin();
}

void loop() {
	if (active) 
	{
		if(!Audio.getAlternateSetting()) 
		{
			//PIT.end();
			active = false;
			blink();
			blink();
		} 
		else 
		{
			if (wait_for_SOF) 
			{
				sendBuffer();
				sampleCounter = 0;
				wait_for_SOF = 0;

				// Get the Frequency
				// Map the pot value to the freq table
				freq = freq_table[map(analogRead(NOTE_POT), 0, 1023, 0, 85)];

				// tuning is equilavent to the phase step
				// phase_step = table_size * (desired_freq/sampling_freq)
				// Not sure why table_size is bit depth instead of sine freq table size
				tuning = 65536*(freq/SAMPLING_FREQ);

			}
			else 
			{

				doAudioStuffISR();
			}
		}
	} 
	else 
	{
		if (Audio.getAlternateSetting()) 
		{
			active = true;
			blink();
		}
	}
}

void sendBuffer() {
	//if (readFirstBuffer) Audio.sendAudio(audioBufferFirst,bufferLength*sampleSize);
	//else 
	Audio.sendAudio(audioBufferLast,bufferLength*sampleSize);
	//Audio.clearSOF();
}

void doAudioStuffISR() {
	int translation = 128; // Half the max amplitude

	if (sampleCounter == bufferLength) {
		wait_for_SOF = 1;
		return;
	}

	phase += tuning;

	// Table sizes available: 8, 10, 12, 13
	// temp should be phase>>(16-num_bits_to_represent_num_points_in_table)
	uint16_t temp = phase>>3; // 13 bit number - 8192

	// Table sizes available: 8, 10, 12, 13
	audioBufferLast[sampleCounter++] = (sineTable13[temp]-translation);

}
void blink() {
	digitalWrite(led,HIGH);
	delay(50);
	digitalWrite(led,LOW);
	delay(50);
}