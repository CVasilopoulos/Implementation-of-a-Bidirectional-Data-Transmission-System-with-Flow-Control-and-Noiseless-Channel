

#include "Timer.h"
#include "Interrupts.h"
#include "RingBuffer.h"
#include "UART.h"
#include "xuartlite_l.h"


void Timer0_Init(XTmrCtr *TmrCtrInstancePtr_0, u8 TmrCtrNumber_0, u16 DeviceId)
{
	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	XTmrCtr_Initialize(TmrCtrInstancePtr_0, DeviceId);

	/*
	 * Setup the handler for the timer counter that will be called from the
	 * interrupt context when the timer expires, specify a pointer to the
	 * timer counter driver instance as the callback reference so the handler
	 * is able to access the instance data
	 */
	XTmrCtr_SetHandler(TmrCtrInstancePtr_0, Timer0CounterHandler, TmrCtrInstancePtr_0);

	/*
	 * Enable the interrupt of the timer counter so interrupts will occur
	 * and use auto reload mode such that the timer counter will reload
	 * itself automatically and continue repeatedly, without this option
	 * it would expire once only
	 */
	XTmrCtr_SetOptions(TmrCtrInstancePtr_0, TmrCtrNumber_0,	XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

	/*
	 * Set a reset value for the timer counter such that it will expire
	 * earlier than letting it roll over from 0, the reset value is loaded
	 * into the timer counter when it is started
	 */
	XTmrCtr_SetResetValue(TmrCtrInstancePtr_0, TmrCtrNumber_0, RESET_0_VALUE);

	/*
	 * Start the timer counter such that it's incrementing by default,
	 * then wait for it to timeout a number of times
	 */
//	XTmrCtr_Start(TmrCtrInstancePtr_0, TmrCtrNumber_0);

}

void Timer1_Init(XTmrCtr *TmrCtrInstancePtr_1, u8 TmrCtrNumber_1, u16 DeviceId)
{
	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	XTmrCtr_Initialize(TmrCtrInstancePtr_1, DeviceId);

	/*
	 * Setup the handler for the timer counter that will be called from the
	 * interrupt context when the timer expires, specify a pointer to the
	 * timer counter driver instance as the callback reference so the handler
	 * is able to access the instance data
	 */
	XTmrCtr_SetHandler(TmrCtrInstancePtr_1, Timer1CounterHandler, TmrCtrInstancePtr_1);

	/*
	 * Enable the interrupt of the timer counter so interrupts will occur
	 * and use auto reload mode such that the timer counter will reload
	 * itself automatically and continue repeatedly, without this option
	 * it would expire once only
	 */
	XTmrCtr_SetOptions(TmrCtrInstancePtr_1, TmrCtrNumber_1,	XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

	/*
	 * Set a reset value for the timer counter such that it will expire
	 * earlier than letting it roll over from 0, the reset value is loaded
	 * into the timer counter when it is started
	 */
	XTmrCtr_SetResetValue(TmrCtrInstancePtr_1, TmrCtrNumber_1, RESET_1_VALUE);

	/*
	 * Start the timer counter such that it's incrementing by default,
	 * then wait for it to timeout a number of times
	 */
//	XTmrCtr_Start(TmrCtrInstancePtr_1, TmrCtrNumber_1);

}

/*
 * Brief	:	Timer Interrupt Handler
 * Brief	:	RTS_ON if the ring buffer has reached low level 
 * Return	:	void
 * Parameter: 	Callback reference to struct of timer, timer ID
 */
void Timer0CounterHandler(void *CallBackRef, u8 TmrCtrNumber_0)
{
	if ( ring_buffer_lwm_reached(&rb_tx) )
	{
		REG_WRITE(UART1_RTS_ADDR,RTS_ON);
		enter_once = 0;
		REG_WRITE(LED_ADDRESS,0x0);
		XTmrCtr_Stop((XTmrCtr *)CallBackRef, TmrCtrNumber_0);
	}
}

/*
 * Brief	:	Timer Interrupt Handler
 * Brief	:	XON if the ring buffer has reached low level 
 * Return	:	void
 * Parameter: 	Callback reference to struct of timer, timer ID
 */
void Timer1CounterHandler(void *CallBackRef, u8 TmrCtrNumber_1)
{
	if ( ring_buffer_lwm_reached(&rb_rx) )
	{
		prev_state_fsm_tc = state_fsm_tc;
		state_fsm_tc = send_xon;
		scheduling_priority = 1;
		XTmrCtr_Stop((XTmrCtr *)CallBackRef, TmrCtrNumber_1);
	}
}
