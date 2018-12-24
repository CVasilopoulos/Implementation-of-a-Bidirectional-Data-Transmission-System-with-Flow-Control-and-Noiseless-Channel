

#include "UART.h"
#include "xuartlite_l.h"
#include "Interrupts.h"
#include "RingBuffer.h"
#include "Timer.h"
#include "xil_printf.h"

static unsigned char RxBuffer_1[1];
static unsigned char TxBuffer_1[1];
volatile unsigned char enter_once = 0;
volatile unsigned char send_once = 0;
volatile unsigned char scheduling_priority = 0;
static volatile unsigned char full_hw_fifo = 0;
volatile state_fsm_tc_t state_fsm_tc = normal;
volatile state_fsm_tc_t prev_state_fsm_tc;

/*
 * Brief	:	UART initialization
 * Return	:	true/false
 * Parameter: 	uartlite struct pointers and device IDs
 */
int UART_Init(XUartLite *UART_Inst_Ptr_1, u16 Uart_1_Dev_ID,  XUartLite *UART_Inst_Ptr_2, u16 Uart_2_Dev_ID)
{
	int Status;

	/* Initialize the UART drivers so that they are ready to use. */
	Status = XUartLite_Initialize(UART_Inst_Ptr_1, Uart_1_Dev_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XUartLite_Initialize(UART_Inst_Ptr_2, Uart_2_Dev_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XUartLite_ResetFifos(UART_Inst_Ptr_1);
	XUartLite_ResetFifos(UART_Inst_Ptr_2);
	/*
	 * Setup the handlers for the UartLite that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the UartLite driver instance as the callback reference so
	 * that the handlers are able to access the instance data.
	 */
	XUartLite_SetSendHandler(UART_Inst_Ptr_1, SendHandler_UART_1, UART_Inst_Ptr_1);
	XUartLite_SetRecvHandler(UART_Inst_Ptr_1, RecvHandler_UART_1, UART_Inst_Ptr_1);

	XUartLite_SetSendHandler(UART_Inst_Ptr_2, SendHandler_UART_2, UART_Inst_Ptr_2);
	XUartLite_SetRecvHandler(UART_Inst_Ptr_2, RecvHandler_UART_2, UART_Inst_Ptr_2);

	/*
	 * Enable the interrupt of the UartLite so that interrupts will occur.
	 */
	XUartLite_EnableInterrupt(UART_Inst_Ptr_1);
	XUartLite_EnableInterrupt(UART_Inst_Ptr_2);

	return XST_SUCCESS;

}

/*
 * Brief	:	Send Handler UART_1
 * Return	:	void
 * Parameter: 	Callback Reference, length of data sended
 */
void SendHandler_UART_1(void *CallBackRef, unsigned int EventData)
{
}

/*
 * Brief	:	Receive Handler UART_1
 * Brief	:	Data from matlab char Gen written to buffer
 * Brief	:	High level buffer checking and RTS control 
 * Return	:	void
 * Parameter: 	Callback Reference, length of data sended
 */
void RecvHandler_UART_1(void *CallBackRef, unsigned int EventData)
{
	XUartLite_Recv(&UART_Inst_Ptr_1, RxBuffer_1, BUF_SIZE);
	if (RingBufferWrite(&rb_tx, RxBuffer_1[0]));
	else
		xil_printf("uart1 fail \r\n");

	if ( (ring_buffer_hwm_reached(&rb_tx) ) && (!enter_once) )
	{
		REG_WRITE(UART1_RTS_ADDR,RTS_OFF);
		enter_once = 1;
		REG_WRITE(LED_ADDRESS,0x3);
		XTmrCtr_Start(&TimerCounterInst_0, TIMER_CNTR_0);
	}
}

/*
 * Brief	:	Send data to Channel (UART_2)
 * Brief	:	XON/XOFF control
 * Return	:	void
 * Parameter: 	void
 */
void SendToChannel()
{
	static unsigned char tx_ch;
	switch (state_fsm_tc)
	{
		case normal:
			while ( (full_hw_fifo) && (state_fsm_tc != do_not_send) )
			{
				if (!XUartLite_IsTransmitFull(UART2_BASE_ADDR)) {
					XUartLite_WriteReg(UART2_BASE_ADDR, XUL_TX_FIFO_OFFSET, tx_ch);
					full_hw_fifo = 0;
				}
			}
			while ( ( !XUartLite_IsTransmitFull(UART2_BASE_ADDR) ) && (state_fsm_tc != do_not_send) && (RingBufferRead(&rb_tx, &tx_ch)) )
			{
//				if ( RingBufferRead(&rb_tx, &tx_ch) )
//				{
					if ( (tx_ch == XON) || (tx_ch == XOFF) || (tx_ch == DEL) )
					{
						XUartLite_WriteReg(UART2_BASE_ADDR, XUL_TX_FIFO_OFFSET, DEL);
						tx_ch ^= XOR_VAL;
						if (!XUartLite_IsTransmitFull(UART2_BASE_ADDR))
							XUartLite_WriteReg(UART2_BASE_ADDR, XUL_TX_FIFO_OFFSET, tx_ch);
						else
						{
//							REG_WRITE(LED_ADDRESS,0xFF);
//							xil_printf("fail \r\n");
							full_hw_fifo = 1;
							return;
						}
					}
					else
						XUartLite_WriteReg(UART2_BASE_ADDR, XUL_TX_FIFO_OFFSET, tx_ch);
//				}
//				else
//					return;
			}

			break;
		case send_xon:
			if (XUartLite_IsTransmitFull(UART2_BASE_ADDR))
				xil_printf("prob xon \r\n");
			else
				XUartLite_WriteReg(UART2_BASE_ADDR, XUL_TX_FIFO_OFFSET, XON);

			scheduling_priority = 0;
			send_once = 0;
//			state_fsm_tc = normal;
			state_fsm_tc = prev_state_fsm_tc;
//			xil_printf("xon \r\n");
//			goto TEST;
			break;

		case send_xoff:
			if (XUartLite_IsTransmitFull(UART2_BASE_ADDR))
				xil_printf("prob xoff \r\n");
			else
				XUartLite_WriteReg(UART2_BASE_ADDR, XUL_TX_FIFO_OFFSET, XOFF);

			scheduling_priority = 0;
//			state_fsm_tc = normal;
			state_fsm_tc = prev_state_fsm_tc;
//			xil_printf("xoff \r\n");
//			goto TEST;
			break;

		case do_not_send:
			break;

		default:
			break;
	}

}

/*
 * Brief	:	Send Handler UART_2
 * Return	:	void
 * Parameter: 	Callback Reference, length of data sended
 */
void SendHandler_UART_2(void *CallBackRef, unsigned int EventData)
{
}

/*
 * Brief	:	Receive Handler UART_2
 * Brief	:	Data from channel written to buffer
 * Brief	:	XON/XOFF control 
 * Return	:	void
 * Parameter: 	Callback Reference, length of data sended
 */
void RecvHandler_UART_2(void *CallBackRef, unsigned int EventData)
{
	static unsigned char char_cntrl = 0;
	static unsigned char rx_char;

	XUartLite_Recv(&UART_Inst_Ptr_2, &rx_char, BUF_SIZE);

	if(char_cntrl)
	{
		rx_char ^= XOR_VAL;
		if (RingBufferWrite(&rb_rx,rx_char));
		else
			xil_printf("Uart2 fail \r\n");
//			REG_WRITE(LED_ADDRESS,0x80);
		char_cntrl = 0;
		goto CHECK_HERE;
	}

	if ( rx_char == DEL )
	{
		char_cntrl = 1;
		return;
	}
	else if ( rx_char == XOFF )
	{
		prev_state_fsm_tc = state_fsm_tc;
		state_fsm_tc = do_not_send;
		REG_WRITE(LED_ADDRESS,0xC);
	}
	else if ( rx_char == XON )
	{
		prev_state_fsm_tc = state_fsm_tc;
		state_fsm_tc = normal;
		REG_WRITE(LED_ADDRESS,0xF0);
	}
	else
	{
		if (RingBufferWrite(&rb_rx,rx_char));
		else
			xil_printf("Uart2 fail \r\n");
//			REG_WRITE(LED_ADDRESS,0x80);
	}

	// Check for XOFF control, if the buffer is on high level then send XOFF
	// and start a timer to check for buffer low level to send XON
CHECK_HERE:
	if ( ring_buffer_hwm_reached(&rb_rx) && !send_once )
	{
		send_once = 1;
		prev_state_fsm_tc = state_fsm_tc;
		state_fsm_tc = send_xoff;
		scheduling_priority = 1;

		XTmrCtr_Start(&TimerCounterInst_1, TIMER_CNTR_1);
	}
}

/*
 * Brief	:	Send Data to UART_1
 * Brief	:	CTS checking to check if matlab Sink is available
 * Brief	:	Register level handling of UART - fast push to UART 
 * Brief	:	CTS checking to check if matlab Sink is available
 * Return	:	void
 * Parameter: 	void
 */
void SendToUART_1()
{
	while( !scheduling_priority && ( REG_READ(UART1_CTS_ADDR) == ((unsigned int)CTS_ON) ) && (!XUartLite_IsTransmitFull(UART1_BASE_ADDR)) && (RingBufferRead(&rb_rx, TxBuffer_1)) )
	{
		XUartLite_WriteReg(UART1_BASE_ADDR, XUL_TX_FIFO_OFFSET, TxBuffer_1[0]);
	}
}




