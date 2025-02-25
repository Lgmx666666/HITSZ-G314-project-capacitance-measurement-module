#ifndef ___USART2_H___
#define ___USART2_H___

#ifdef UART_USE_STDIO
#if __ICCARM__
#include <yfuns.h>
#endif

#endif
	


#ifdef UART_USE_STDIO
#define UART_printf(fmt,args...)    printf (fmt ,##args)
#endif

#ifdef UART_USE_STDIO
int printf(const char *fmt, ...);
#endif

void UART1_Init(unsigned int baudrate);
__interrupt void UART2_RX_RXNE(void);

#endif

