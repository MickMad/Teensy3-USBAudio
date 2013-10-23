#include "usb_dev.h"
#include "usb_desc.h"
#include "usb_audio.h"
#include "core_pins.h" // for yield(()
#include <string.h>
#include "HardwareSerial.h"

#ifdef AUDIO_CONTROL_INTERFACE

#define TX_PACKET_LIMIT 16
// When the PC isn't listening, how long do we wait before discarding data?
#define TX_TIMEOUT_MSEC 5

#define TRANSMIT_FLUSH_TIMEOUT	10

#if F_CPU == 96000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 596)
#elif F_CPU == 48000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 428)
#elif F_CPU == 24000000
  #define TX_TIMEOUT (TX_TIMEOUT_MSEC * 262)
#endif

static usb_packet_t *tx_packet=NULL;
static usb_packet_t *rx_packet=NULL;
static uint8_t tx_transmit_previous_timeout=0;
static uint8_t tx_noautoflush=0;
uint8_t audio_streaming_tx_alternate_setting = 0;
uint8_t audio_streaming_rx_alternate_setting = 0;
volatile uint8_t audio_SOF_signal = 0;
volatile uint8_t usb_audio_transmit_flush_timer = 0;
							
void usb_audio_set_alternate_setting_tx(uint8_t alt_set){
	audio_streaming_tx_alternate_setting=alt_set;
}
uint8_t usb_audio_get_alternate_setting_tx(){
	return audio_streaming_tx_alternate_setting;
}

void usb_audio_set_alternate_setting_rx(uint8_t alt_set){
	audio_streaming_rx_alternate_setting=alt_set;
}
uint8_t usb_audio_get_alternate_setting_rx(){
	return audio_streaming_rx_alternate_setting;
}

uint8_t usb_audio_write_data(const void *data, uint8_t length){
	uint32_t len;
	uint32_t wait_count;
	const myType *src = (const myType *)data;
	myType *dest;
	tx_noautoflush = 1;
	while (length > 0) {
		if (!tx_packet) {
			while (1) {
				if (!usb_configuration) {
					return -1;
				}
				if (usb_tx_packet_count(AUDIO_STREAMING_TX_ENDPOINT) < TX_PACKET_LIMIT) {
					tx_packet = usb_malloc();
					if (tx_packet) break;
				}
				if (++wait_count > TX_TIMEOUT || tx_transmit_previous_timeout) {
					tx_transmit_previous_timeout = 1;
					return -1;
				}
				yield();
			}
		}
		tx_transmit_previous_timeout = 0;

		len = AUDIO_STREAMING_TX_SIZE - tx_packet->index;
		if (len > length) len = length;
		dest = tx_packet->buf + tx_packet->index;
		tx_packet->index += len;
		length -= len;
		while (len-- > 0) *dest++ = *src++;
		if (tx_packet->index >= AUDIO_STREAMING_TX_SIZE) {
			tx_packet->len = AUDIO_STREAMING_TX_SIZE;
			usb_tx(AUDIO_STREAMING_TX_ENDPOINT, tx_packet);
			tx_packet = NULL;
		}
		
		usb_audio_transmit_flush_timer = TRANSMIT_FLUSH_TIMEOUT;
	}
	tx_noautoflush = 0;
	return 0;
}

uint8_t usb_audio_read_data(const void* data, uint8_t length){
	uint32_t len;
	uint32_t count=0;
	myType *dest = (const myType *)data;
	
	while (length > 0) {
		if (!usb_configuration) break;
		if (!rx_packet) {
			rx:
			rx_packet = usb_rx(AUDIO_STREAMING_RX_ENDPOINT);
			if (!rx_packet) break;
			if (rx_packet->len == 0) {
				usb_free(rx_packet);
				goto rx;
			}
		}
		len = rx_packet->len - rx_packet->index;
		if (len > length) len = length;
		memcpy(dest, rx_packet->buf + rx_packet->index, len);
		dest += len;
		count += len;
		length -= len;
		rx_packet->index += len;
		if (rx_packet->index >= rx_packet->len) {
			usb_free(rx_packet);
			rx_packet = NULL;
		}
	}
	return count;
}

void usb_audio_flush_callback(void){
	if (tx_noautoflush) return;
	if (tx_packet) {
		tx_packet->len = tx_packet->index;
		usb_tx(AUDIO_STREAMING_TX_ENDPOINT, tx_packet);
		tx_packet = NULL;
	} else {
		usb_packet_t *tx = usb_malloc();
		if (tx) {
			usb_tx(AUDIO_STREAMING_TX_ENDPOINT, tx);
		}else usb_audio_transmit_flush_timer = 1;
	}	
}

/* void usb_audio_send_audio(){
	uint16_t wait_count=0;
	tx_noautoflush = 1;
	if (!tx_packet) {
		while (1) {
			if (!usb_configuration) {
				return;
			}
			if (usb_tx_packet_count(AUDIO_STREAMING_TX_ENDPOINT) < TX_PACKET_LIMIT) {
				tx_packet = usb_malloc();
				if (tx_packet) break;
			}
			if (++wait_count > TX_TIMEOUT || tx_transmit_previous_timeout) {
				tx_transmit_previous_timeout = 1;
				return;
			}
			yield();
		}
	}
	tx_transmit_previous_timeout = 0;

	tx_noautoflush = 0;
}

void usb_audio_flush_callback(void){
	if (!audio_streaming_alternate_setting) return;
	if (!tx_packet) {
		tx_packet = usb_malloc();
		serial_print("alloc\n");
	}
	uint8_t i = 0;
	tx_packet->index = 0;
	for (i=0;i<16;i++){
		if (i%2==0) *(tx_packet->buf + i) = 0;
		else *(tx_packet->buf + i) = 128;
	}
	tx_packet->len = AUDIO_STREAMING_TX_SIZE;
	usb_tx(AUDIO_STREAMING_TX_ENDPOINT, tx_packet);
} */

void usb_audio_begin(){
	while (!usb_configuration){
		delay(1);
	}
}
#endif
