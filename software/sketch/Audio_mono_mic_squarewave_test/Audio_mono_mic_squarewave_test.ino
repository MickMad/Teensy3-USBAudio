#define MAX_BUFFER_SIZE 16
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
	bufferLength = 16; //8 samples per buffer/packet
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
	if (active){
		if(!Audio.getAlternateSetting()){
			active=false;
			blinkTwoTimes();
			blinkTwoTimes();
		}else{
			if (Audio.getSOF()) doAudioStuff();
		}
	} else {
		if (Audio.getAlternateSetting()){
			active=true;
			blinkTwoTimes();
		}
	}
}

void doAudioStuff(){
	for (sampleCounter=0;sampleCounter<bufferLength;sampleCounter++){
		if (sampleCounter<8){
			audioBuffer[sampleCounter] = 0x7FFF;
			digitalWrite(led,HIGH);
		}else{
			audioBuffer[sampleCounter] = 0x8000;
			digitalWrite(led,LOW);
		}
	}
	Audio.sendAudio(audioBuffer,bufferLength*sampleSize);
	Audio.clearSOF();
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