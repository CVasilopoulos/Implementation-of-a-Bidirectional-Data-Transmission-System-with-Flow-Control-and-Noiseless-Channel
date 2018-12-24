#include "RingBuffer.h"
#include <string.h>

ring_buffer_t rb_tx;
ring_buffer_t rb_rx;

/*
 * Brief	:	number of items available in the ring buffer
 * Return	:	number of items
 * Parameter: 	ring buffer pointer
 */
static rb_size_t RingBufferNumItems(ring_buffer_t *rb)
{
	return ( (rb->wr_idx - rb->rd_idx) & RING_BUF_SIZE_MASK );
}

/*
 * Brief	:	Check if the ring buffer is empty
 * Return	:	true/false
 * Parameter: 	ring buffer pointer
 */
static int RingBufferIsEmpty(ring_buffer_t *rb)
{
	return ( rb->wr_idx == rb->rd_idx );
}

/*
 * Brief	:	Check if the ring buffer is full
 * Return	:	true/false
 * Parameter: 	ring buffer pointer
 */
static int RingBufferIsFull(ring_buffer_t *rb)
{
	return ( ( rb->wr_idx - rb->rd_idx ) & RING_BUF_SIZE_MASK ) == RING_BUF_SIZE_MASK;
}

/*
 * Brief	:	Check if the ring buffer has reached high level
 * Return	:	true/false
 * Parameter: 	ring buffer pointer
 */
int ring_buffer_hwm_reached(ring_buffer_t *rb)
{
	return ( (rb->wr_idx - rb->rd_idx) & RING_BUF_SIZE_MASK ) >= HWM_BUF_SIZE;
}

/*
 * Brief	:	Check if the ring buffer has reached low level
 * Return	:	true/false
 * Parameter: 	ring buffer pointer
 */
int ring_buffer_lwm_reached(ring_buffer_t *rb)
{
	return ( (rb->wr_idx - rb->rd_idx) & RING_BUF_SIZE_MASK ) <= LWM_BUF_SIZE;
}

/*
 * Brief	:	Ring buffer initialization
 * Return	:	none
 * Parameter: 	ring buffer pointer
 */
void RingBufferInit(ring_buffer_t *rb)
{
	rb->rd_idx = 0;
	rb->wr_idx = 0;
}

/*
 * Brief	:	Push data to ring buffer
 * Return	:	true/false
 * Parameter: 	ring buffer pointer, data to be written
 */
int RingBufferWrite(ring_buffer_t *rb, const unsigned char data)
{
	if (RingBufferIsFull(rb))
		return 0;

	// write data to buf array
	rb->buf[rb->wr_idx] = data;
	++rb->wr_idx;
	rb->wr_idx &= RING_BUF_SIZE_MASK;

	return 1;
}

/*
 * Brief	:	Pull data from ring buffer
 * Return	:	true/false
 * Parameter: 	ring buffer pointer, data read pointer
 */
int RingBufferRead(ring_buffer_t *rb, unsigned char *data)
{
	if (RingBufferIsEmpty(rb))
		return 0;

	// read data from buf array
	*data = rb->buf[rb->rd_idx];
	++rb->rd_idx;
	rb->rd_idx &= RING_BUF_SIZE_MASK;

	return 1;
}

/*
 * Brief	:	Pull array of data from ring buffer
 * Return	:	true/false
 * Parameter: 	ring buffer pointer, data read pointer, length of data
 */
rb_size_t RingBufferReadArray(ring_buffer_t *rb, unsigned char *data, rb_size_t len)
{
	static rb_size_t nItems;
	nItems = RingBufferNumItems(rb); // items that are existed in the rb and can read

	if (nItems > len) // len usually here is the MAX_PAYLOAD_SIZE
		nItems = len;
	else if (nItems == 0)
		return 0;

	memcpy(data, &( rb->buf[rb->rd_idx] ), nItems);
	rb->rd_idx += nItems;
	rb->rd_idx &= RING_BUF_SIZE_MASK;

	return nItems; // returns the len of data read
}
