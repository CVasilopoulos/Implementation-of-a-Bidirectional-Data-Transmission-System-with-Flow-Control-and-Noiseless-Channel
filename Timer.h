

#ifndef TIMER_H_
#define TIMER_H_

#include "xparameters.h"
#include "xtmrctr.h"
#include "xil_exception.h"
#include "xil_printf.h"

void Timer0CounterHandler(void *CallBackRef, u8 TmrCtrNumber_0);
void Timer1CounterHandler(void *CallBackRef, u8 TmrCtrNumber_1);
void Timer0_Init(XTmrCtr *TmrCtrInstancePtr_0, u8 TmrCtrNumber_0, u16 DeviceId);
void Timer1_Init(XTmrCtr *TmrCtrInstancePtr_1, u8 TmrCtrNumber_1, u16 DeviceId);


#define TIMER_CNTR_0		 		0
#define TIMER_CNTR_1	 			1
#define TIMER_CNTR_2	 			2
#define TMRCTR_DEVICE_ID_0			XPAR_TMRCTR_0_DEVICE_ID
#define TMRCTR_INTERRUPT_ID_0		XPAR_INTC_0_TMRCTR_0_VEC_ID
#define TMRCTR_DEVICE_ID_1			XPAR_TMRCTR_1_DEVICE_ID
#define TMRCTR_INTERRUPT_ID_1		XPAR_INTC_0_TMRCTR_1_VEC_ID
#define TMRCTR_DEVICE_ID_2			XPAR_TMRCTR_2_DEVICE_ID
#define TMRCTR_INTERRUPT_ID_2		XPAR_INTC_0_TMRCTR_2_VEC_ID

/*
 * The following constant is used to set the reset value of the timer counter,
 * making this number larger reduces the amount of time this example consumes
 * because it is the value the timer counter is loaded with when it is started
 */

#define RESET_0_VALUE	 		0xFFF85EE1
#define RESET_1_VALUE	 		0xFFF85EE1


// 0xFFF85EE1 -> 5 ms

XTmrCtr TimerCounterInst_0;
XTmrCtr TimerCounterInst_1;


#endif /* TIMER_H_ */
