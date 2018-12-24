#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "xil_printf.h"
#include "Interrupts.h"
#include "UART.h"
#include "Timer.h"
#include "RingBuffer.h"

int main()
{
	int Status;
    init_platform();
	REG_WRITE(UART1_RTS_ADDR,RTS_OFF);
    xil_printf("Application Started\n\r");

    RingBufferInit(&rb_tx);
    RingBufferInit(&rb_rx);

	Timer0_Init(&TimerCounterInst_0, TIMER_CNTR_0, XPAR_TIMER_0_DEVICE_ID);
    Timer1_Init(&TimerCounterInst_1, TIMER_CNTR_1, XPAR_TIMER_1_DEVICE_ID);
    // Initialize the UART devices
    UART_Init(&UART_Inst_Ptr_1, UART_1_DEV_ID,  &UART_Inst_Ptr_2, UART_2_DEV_ID);

    // Initialize Interrupt Controller, Enable interrupts in devices and connect them to the Interrupt Controller
    Status = SetupInterruptSystem(&UART_Inst_Ptr_1, UART_1_INT_IRQ_ID, &UART_Inst_Ptr_2, UART_2_INT_IRQ_ID, &TimerCounterInst_0, TMRCTR_INTERRUPT_ID_0, &TimerCounterInst_1, TMRCTR_INTERRUPT_ID_1);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	REG_WRITE(LED_ADDRESS,0x0);
	REG_WRITE(UART1_RTS_ADDR,RTS_ON);
	volatile unsigned char flag = 1;

	while (flag)
	{
		// Send Data to Channel (UART_2)
		SendToChannel();
		
		// Send Data to UART_1 (matlab Sink)
		SendToUART_1();
	}

    REG_WRITE(UART1_RTS_ADDR,RTS_OFF);
    cleanup_platform();
    return 0;
}
