#include "main.h"
#include "Lcd.h"
#include "FlowMeter.h"
#include "ModBusDev.h"

u16 Rx1_Timer  = 0;
u16 Rx2_Timer  = 0;
u16 Rx3_Timer  = 0;
u16 Rx4_Timer  = 0;


u8  TX1_Cnt;    //���ͼ���
u8  RX1_Cnt;    //���ռ���
bit B_TX1_Busy; //����æ��־

u8  TX2_Cnt;    //���ͼ���
u8  RX2_Cnt;    //���ռ���
bit B_TX2_Busy; //����æ��־

u8  TX3_Cnt;
u8  RX3_Cnt;
bit B_TX3_Busy;

u8  TX4_Cnt;
u8  RX4_Cnt;
bit B_TX4_Busy;


u8  RX1_Buffer[MAX_LENGTH]; //���ջ���
u8  RX2_Buffer[MAX_LENGTH]; //���ջ���
u8  RX3_Buffer[MAX_LENGTH]; //���ջ���
u8  RX4_Buffer[MAX_LENGTH]; //���ջ���


void UART1_config()
{
    /*********** ������ʹ�ö�ʱ��1 *****************/
    {
        TR1 = 0;
        AUXR &= ~0x01;      //S1 BRT Use Timer1;
        AUXR |=  (1<<6);    //Timer1 set as 1T mode
        TMOD &= ~(1<<6);    //Timer1 set As Timer
        TMOD &= ~0x30;      //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate) % 256);
        ET1 = 0;    //��ֹ�ж�
        INTCLKO &= ~0x02;  //�����ʱ��
        TR1  = 1;
    }

    /*************************************************/
    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 
    //           0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨���� 

    SCON = (SCON & 0x3f) | 0x40; 

    PS  = 0;    //�жϸ����ȼ�
    PSH = 0;
    //PS  = 1;    //�����ȼ��ж�
    ES  = 1;    //�����ж�
    REN = 1;    //�������
    
    //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 
    //                 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;  
    
    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;
}


void UART2_config()
{
    
    T2R   = 0;
    T2x12 = 1;
    AUXR &= ~(1<<3);
    T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate)/ 256);
    T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate)% 256);
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;

    S2CON = (S2CON & 0x3f) | 0x40;
    
    PS2  = 1;    //�жϸ����ȼ�
    PS2H = 1;
    
    ES2   = 1;
    S2REN = 1;
    P_SW2 &= ~0x01; 

    B_TX2_Busy = 0;
    TX2_Cnt = 0;
    RX2_Cnt = 0;
        
}


void UART3_config()
{
    T3R = 0;
    S3CON = 0x40;
    T3H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T3L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
    //ET3 = 0;    //��ֹ�ж�
    T4T3M = 0x0a;
    
    // �ж����ȼ�
    PS3H = 1;
    PS3 = 0;

    ES3 = 1;
    S3REN = 1;
    P_SW2 &= ~0x02; 

    B_TX3_Busy = 0;
    TX3_Cnt = 0;
    RX3_Cnt = 0;

        
}

void UART4_config()
{
    T4R = 0;
    S4CON = 0x40;   // ��ʱ��4Ϊ�����ʷ�����
    T4H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T4L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
    //ET4 = 0;    //��ֹ�ж�
    T4T3M |= (0x0a<<4);

    // �ж����ȼ�
    PS4H = 0;
    PS4 = 0;    
    
    ES4 = 1;    // ����4�ж�����
    S4REN = 1;  // �����ڽ�������
    
    P_SW2 &= ~0x08;   // S4_S == 0; ���ţ�P0.2, P0.3 

    B_TX4_Busy = 0;
    TX4_Cnt = 0;
    RX4_Cnt = 0;
        
}

void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;
        Rx1_Timer = 0;
        RX1_Buffer[RX1_Cnt] = SBUF;
        
        if(++RX1_Cnt >= MAX_LENGTH)   
        {
            RX1_Cnt = 0;
        }
    }

    if(TI)
    {
        TI = 0;
        B_TX1_Busy = 0;
    }
}



void UART2_int (void) interrupt 8
{
    if(S2RI)
    {
        S2RI = 0;
        RX2_Buffer[RX2_Cnt] = S2BUF;
        Rx2_Timer = 0;
        
        if(++RX2_Cnt >= MAX_LENGTH)   
        {
            RX2_Cnt = 0;
        }
    }

    if(S2TI)
    {
        S2TI = 0;
        B_TX2_Busy = 0;
    }
}


void UART3_int (void) interrupt 17
{
    if(S3RI)
    {
        S3RI = 0;
        Rx3_Timer = 0;
        RX3_Buffer[RX3_Cnt] = S3BUF;
        
        if(++RX3_Cnt >= MAX_LENGTH)   
        {
            RX3_Cnt = 0;
        }
    }

    if(S3TI)
    {
        S3TI = 0;
        B_TX3_Busy = 0;
    }
}

void UART4_int (void) interrupt 18
{
    if(S4RI)
    {
        S4RI = 0;
        Rx4_Timer = 0;
        RX4_Buffer[RX4_Cnt] = S4BUF;
        
        if(++RX4_Cnt >= MAX_LENGTH)   
        {
            RX4_Cnt = 0;
        }
    }

    if(S4TI)
    {
        S4TI = 0;
        B_TX4_Busy = 0;
    }
}


void Uart1Send(u8 *buf, u8 len)
{
    u8 i;
    for (i=0;i<len;i++)     
    {
        SBUF = buf[i];
        B_TX1_Busy = 1;
        while(B_TX1_Busy);
    }
}

void Uart2Send(u8 *buf, u8 len)
{
    u8 i;
    for (i=0;i<len;i++)     
    {
        S2BUF = buf[i];
        B_TX2_Busy = 1;
        while(B_TX2_Busy);
    }  
}

void Uart3Send(u8 *buf, u8 len)
{
    u8 i;
    RS485_EN1(0);
    for (i=0;i<len;i++)     
    {
        S3BUF = buf[i];
        B_TX3_Busy = 1;
        while(B_TX3_Busy);
    }
    RS485_EN1(1);
}

void Uart4Send(u8 *buf, u8 len)
{
    u8 i;
    RS485_EN2(0);
    for (i=0;i<len;i++)     
    {
        S4BUF = buf[i];
        B_TX4_Busy = 1;
        while(B_TX4_Busy);
    }
    RS485_EN2(1);
}


void ClearUart1Buf()
{
    memset(RX1_Buffer,0,MAX_LENGTH);
    RX1_Cnt = 0;
}


void ClearUart2Buf()
{
    memset(RX2_Buffer,0,MAX_LENGTH);
    RX2_Cnt = 0;
}

void ClearUart3Buf()
{
    memset(RX3_Buffer,0,MAX_LENGTH);
    RX3_Cnt = 0;
}

void ClearUart4Buf()
{
    memset(RX4_Buffer,0,MAX_LENGTH);
    RX4_Cnt = 0;
}

void Uart1Hnd()
{
    if (Rx1_Timer > 20)
    {
        Rx1_Timer = 0;
        //Uart1Send(RX1_Buffer,RX1_Cnt);
        //HndModBusRecv(RS485,RX1_Buffer,RX1_Cnt);
        ClearUart1Buf();
    }
}


void Uart2Hnd()
{
    if (Rx2_Timer > 20)
    {
        Rx2_Timer = 0;

        DumpCmd(RX2_Buffer, RX2_Cnt);
        
        memcpy(RecvBuf1,RX2_Buffer, RX2_Cnt);
        RecLength1 = RX2_Cnt;
        ClearUart2Buf();
        
        HndLcdData();
    }
}



// ������
void Uart3Hnd()
{
    if (Rx3_Timer > 20)
    {
        Rx3_Timer = 0;
        //Error();
        FlowMeterAck((BYTE *)RX3_Buffer,(BYTE)RX3_Cnt);
        
        ClearUart3Buf();
    }
}


void Uart4Hnd()
{
    if (Rx4_Timer > 20)
    {
        Rx4_Timer = 0;

        HndModBusRecv(RS485, RX4_Buffer,RX4_Cnt);
        
        ClearUart4Buf();
    }
}


