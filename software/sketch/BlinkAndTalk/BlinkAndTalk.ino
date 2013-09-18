#define MAX_BUFFER_SIZE 32
#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)

IntervalTimer PIT = IntervalTimer();
uint16_t audioBuffer[MAX_BUFFER_SIZE];
uint8_t bufferLength;
uint8_t sampleSize;
uint8_t sampleCounter = 0;
uint8_t led = 13;
uint8_t counter = 0;
bool active=false;
void initStuff(){
	sampleCounter = 0;
	sampleSize = 2; //2 bytes per sample
	bufferLength = 8; //8 samples per buffer/packet
	uint8_t i;
	for (i=0;i<MAX_BUFFER_SIZE;i++){
		audioBuffer[i] = 0x0000;
	}
}
void setup() {
	pinMode(led,OUTPUT);
	initStuff();
	Audio.begin();
}

void loop() {
	if(!Audio.getAlternateSetting()){
		if (active) {
			PIT.end();
			active=false;
			blinkTwoTimes();
			blinkTwoTimes();
		}
	}
	else if (Audio.getAlternateSetting()) {
		active=true;
		blinkTwoTimes();
		PIT.begin(doAudioStuffISR, 100);  // 48 khz
	}
}

void doAudioStuffISR(){
	//if (sampleCounter<4) audioBuffer[sampleCounter] = 0x0080;
	//else audioBuffer[sampleCounter] = 0xff7f;
	if (sampleCounter<4) audioBuffer[sampleCounter] = 0x8000;
	else audioBuffer[sampleCounter] = 0x7fff;
	sampleCounter++;
	if (sampleCounter%2) digitalWrite(led,LOW);
	else digitalWrite(led,HIGH);
	if (sampleCounter>bufferLength) {
		Audio.sendAudio(audioBuffer,bufferLength*sampleSize);
		sampleCounter = 0;
	}
}
void blinkTwoTimes(){
	digitalWrite(led,HIGH);
	delay(100);
	digitalWrite(led,LOW);
	delay(100);
	digitalWrite(led,HIGH);
	delay(100);
	digitalWrite(led,LOW);
	delay(100);
}