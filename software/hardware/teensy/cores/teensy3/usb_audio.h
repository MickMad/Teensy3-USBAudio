#ifndef USBaudio_h_
#define USBaudio_h_

#if defined (USB_AUDIO)

#include <inttypes.h>
#include "usb_dev.h"

#ifdef __cplusplus
extern "C"{
#endif
uint8_t usb_audio_write_data(const void *data, uint8_t length);
uint8_t usb_audio_read_data(const void *data, uint8_t length);
void usb_audio_begin();
extern uint8_t audio_streaming_tx_alternate_setting;
extern uint8_t audio_streaming_rx_alternate_setting;
extern volatile uint8_t usb_audio_transmit_flush_timer;
extern volatile uint8_t audio_SOF_signal;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class usb_audio_class{
	public:
		void begin(){
			usb_audio_begin();
		};
		uint8_t sendAudio(const void *data, uint8_t length){ 
			return usb_audio_write_data(data,length); 
		};
		uint8_t recvAudio(const void *data, uint8_t length){
			return usb_audio_read_data(data,length);
		};
		uint8_t getAlternateSettingTX(){ 
			return audio_streaming_tx_alternate_setting; 
		};
		uint8_t getAlternateSettingRX(){ 
			return audio_streaming_rx_alternate_setting; 
		};
		uint8_t getSOF(){
			return audio_SOF_signal;
		};
		uint8_t clearSOF(){
			audio_SOF_signal = 0;
		};
};

extern usb_audio_class Audio;

#endif // __cplusplus 

#endif // USB_AUDIO
#endif // USBaudio_h_