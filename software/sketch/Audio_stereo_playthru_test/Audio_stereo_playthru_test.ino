#define MAX_BUFFER_SIZE 96
#define SAMPLING_FREQ 48000

uint16_t audioBufferTX[MAX_BUFFER_SIZE];
uint16_t audioBufferRX[MAX_BUFFER_SIZE];
uint8_t bufferLength;
uint8_t sampleSize;

bool tx_active = false;
bool rx_active = false;

void initTX() {
	uint8_t i;
	for (i=0;i<MAX_BUFFER_SIZE;i++) {
		audioBufferTX[i] = 0x0000;
	}
}

void initRX() {
	uint8_t i;
	for (i=0;i<MAX_BUFFER_SIZE;i++) {
		audioBufferRX[i] = 0x0000;
	}
}

void setup() {
	sampleSize = 2;			
	bufferLength = MAX_BUFFER_SIZE; 
	
	initTX();
	initRX();
	
	Audio.begin();
}

void loop() {
	if (tx_active) {
		if (!Audio.getAlternateSettingTX()) {
			tx_active = false;
		} else if (Audio.getSOF()) {
			Audio.sendAudio(audioBufferRX,sampleSize*bufferLength);
			Audio.clearSOF();
		}
	} else {
		if (Audio.getAlternateSettingTX()) {
			tx_active = true;
		}
	}
	
	if (rx_active) {
		if (!Audio.getAlternateSettingRX()) {
			rx_active = false;
		} else Audio.recvAudio(audioBufferRX,sampleSize*bufferLength);
	} else {
		if (Audio.getAlternateSettingRX()) {
			rx_active = true;
		}
	}
}