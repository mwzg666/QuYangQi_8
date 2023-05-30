#ifndef __UART_H__
#define __UART_H__

#define Baudrate   115200UL
#define Baudrate3   9600UL

#define MAX_LENGTH             200


void UART1_config();
void UART2_config();
void UART3_config();
void UART4_config();

void Uart1Send(u8 *buf, u8 len);
void Uart2Send(u8 *buf, u8 len);
void Uart3Send(u8 *buf, u8 len);
void Uart4Send(u8 *buf, u8 len);

void ClearUart1Buf();
void ClearUart2Buf();
void ClearUart3Buf();
void ClearUart4Buf();

void Uart1Hnd();
void Uart2Hnd();
void Uart3Hnd();
void Uart4Hnd();
#endif