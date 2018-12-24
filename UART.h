

#ifndef UART_H_
#define UART_H_

#include "xuartlite.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "Interrupts.h"

/************************** UART Definitions and Initializations *****************************/

/* Define UARTs' IDs (from xparameters.h) */
#define UART_1_DEV_ID	  	XPAR_UARTLITE_1_DEVICE_ID
#define UART_1_INT_IRQ_ID	XPAR_INTC_0_UARTLITE_1_VEC_ID
#define UART_2_DEV_ID	  	XPAR_UARTLITE_2_DEVICE_ID
#define UART_2_INT_IRQ_ID	XPAR_INTC_0_UARTLITE_2_VEC_ID
#define TIMER_1_DEV_ID


/************************** Address Definitions *****************************/
/* BASE ADDRESSES */
#define UART1_BASE_ADDR						XPAR_UARTLITE_1_BASEADDR
#define UART2_BASE_ADDR						XPAR_UARTLITE_2_BASEADDR

#define UART1_RTS_ADDR						XPAR_GPIO_1_BASEADDR
#define UART1_CTS_ADDR						XPAR_GPIO_1_BASEADDR + 0x8

#define RTS_ON		0x00
#define RTS_OFF 	0x01
#define CTS_ON		0x00
#define CTS_OFF		0x01

/* OFFSETS */
#define RECV_FIFO							0x00000000
#define TRAN_FIFO							0x00000004
#define STAT_REG							0x00000008
#define CONT_REG							0x0000000C

/************************** Debug Messages*****************************/
#define UART_DBG							0 	// 0: OFF, 1: ON

/************************** Function Prototypes ******************************/
int UART_Init(XUartLite *UART_Inst_Ptr_1, u16 Uart_1_Dev_ID,  XUartLite *UART_Inst_Ptr_2, u16 Uart_2_Dev_ID);
void RecvHandler_UART_1(void *CallBackRef, unsigned int EventData);
void SendHandler_UART_1(void *CallBackRef, unsigned int EventData);
void RecvHandler_UART_2(void *CallBackRef, unsigned int EventData);
void SendHandler_UART_2(void *CallBackRef, unsigned int EventData);


void SendToChannel();
void SendToUART_1();

/************************** I/O Buffer Declarations *****************************/
#define BUF_SIZE							1
#define LED_ADDRESS 			XPAR_GPIO_0_BASEADDR
/************************** Device Pointer Declarations *****************************/
XUartLite UART_Inst_Ptr_1;            /* The instance of the UartLite Device */
XUartLite UART_Inst_Ptr_2;            /* The instance of the UartLite Device */

#define XON			((unsigned char)0x14)
#define XOFF		((unsigned char)0x15)
#define DEL			((unsigned char)0x10)
#define XOR_VAL 	((unsigned char)0x20)

#define REG_WRITE(Addr, Value) \
		(*(volatile unsigned int *)((Addr)) = (((unsigned int)(Value))))

#define REG_READ(Addr) \
		(*(volatile unsigned int *)(Addr))


extern volatile unsigned char enter_once;
extern volatile unsigned char send_once;
extern volatile unsigned char scheduling_priority;

typedef enum {normal, send_xon, send_xoff, do_not_send}state_fsm_tc_t;

extern volatile state_fsm_tc_t state_fsm_tc;
extern volatile state_fsm_tc_t prev_state_fsm_tc;

#endif /* UART_H_ */
