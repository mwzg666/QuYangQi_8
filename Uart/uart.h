#ifndef __UART_H__
#define __UART_H__

#define Baudrate   115200UL
#define Baudrate3   9600UL

#define MAX_LENGTH             200



extern u8  TX1_Cnt;    //���ͼ���
extern u8  RX1_Cnt;    //���ռ���
extern bit B_TX1_Busy; //����æ��־
extern u16 Rx1_Timer;

extern u8  TX2_Cnt;    //���ͼ���
extern u8  RX2_Cnt;    //���ռ���
extern bit B_TX2_Busy; //����æ��־
extern u16 Rx2_Timer;

extern u8  RX3_Cnt;    //���ռ���
extern u8  TX3_Cnt;    //���ͼ���
extern bit B_TX3_Busy; //����æ��־
extern u16 Rx3_Timer;

extern u8  RX4_Cnt;    //���ռ���
extern u8  TX4_Cnt;    //���ͼ���
extern bit B_TX4_Busy; //����æ��־
extern u16 Rx4_Timer;


extern u8  xdata RX2_Buffer[]; //���ջ���
extern u8  xdata RX3_Buffer[]; //���ջ���


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