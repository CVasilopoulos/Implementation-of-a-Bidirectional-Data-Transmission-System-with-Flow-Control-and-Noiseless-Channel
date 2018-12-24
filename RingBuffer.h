#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "string.h"
// defines

// Ring Buffer must be power of 2 
// because of its implementation
#define RING_BUF_SIZE 		256
#define RING_BUF_SIZE_MASK (RING_BUF_SIZE-1)
#define HWM_BUF_SIZE 		153
#define LWM_BUF_SIZE 		51

typedef unsigned short rb_size_t; // caution this holds indices e.t.c (max limit 65535)

typedef struct {
	unsigned char buf[RING_BUF_SIZE];
	volatile unsigned short wr_idx;
	volatile unsigned short rd_idx;
}ring_buffer_t;

// functions declarations
int ring_buffer_hwm_reached(ring_buffer_t *rb);
int ring_buffer_lwm_reached(ring_buffer_t *rb);
void RingBufferInit(ring_buffer_t *rb);
int RingBufferWrite(ring_buffer_t *rb, const unsigned char data);
int RingBufferRead(ring_buffer_t *rb,  unsigned char *data);
rb_size_t RingBufferReadArray(ring_buffer_t *rb, unsigned char *data, rb_size_t len);

extern ring_buffer_t rb_tx;
extern ring_buffer_t rb_rx;

#endif /* RINGBUFFER_H_ */
