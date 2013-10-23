/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows 
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _usb_mem_h_
#define _usb_mem_h_

#define MAX_PACKET_BUFFER_SIZE 96  // used to set the dimension of the buf[] field in usb_packet_struct
#define MAX_PACKET_BUFFER_SAMPLE_SIZE 2

#if MAX_PACKET_BUFFER_SAMPLE_SIZE == 1
#define myType uint8_t
#elif MAX_PACKET_BUFFER_SAMPLE_SIZE == 2
#define myType uint16_t //used to set the type of the content of the buf[] field in usb_packet_struct
#endif

#include <stdint.h>

typedef struct usb_packet_struct {
	uint16_t len;
	uint16_t index;
	struct usb_packet_struct *next;
	myType buf[MAX_PACKET_BUFFER_SIZE];
} usb_packet_t;

#ifdef __cplusplus
extern "C" {
#endif

usb_packet_t * usb_malloc(void);
void usb_free(usb_packet_t *p);

#ifdef __cplusplus
}
#endif


#endif
