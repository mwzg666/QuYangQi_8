#ifndef __MAIN_H__
#define __MAIN_H__

#include "STC32G.h"

#include "stdio.h"
#include "intrins.h"
#include "string.h"



//#define  FOR_DEBUG

typedef 	unsigned char	BOOL;
#define     TRUE    1
#define     FALSE   0

typedef 	unsigned char	BYTE;
typedef 	unsigned short	WORD;
typedef 	unsigned long	DWORD;

typedef 	unsigned char	u8;
typedef 	unsigned short	u16;
typedef 	unsigned long	u32;

typedef 	unsigned int	uint;


#define alt_u8   BYTE
#define alt_u16 WORD
#define alt_u32 DWORD  


#define bool BYTE
#define true 1
#define false 0

#define BOOL BYTE
#define TRUE 1
#define FALSE 0

#define SENSOR_COUNT 4
#define CHANNLE_NUM  8
#define FLOW_METER_CNT   9



#include "IrDA.h"
#include "uart.h"
#include "EepRom.h"
#include "MwPro.h"
#include "LcdApp.h"
#include "ModBus.h"



typedef struct
{
    WORD  Sign;        // 0xA55A
    BYTE  Address;
    BYTE  res1;
    //HOST_SENSOR_PARAM SenParam[SENSOR_COUNT];   // 14*4 = 56 * 4 = 224
    //HOST_ALRAM_PARA   AlmParam[SENSOR_COUNT];   //  6*4 = 24 * 4 = 96
    WORD  res2;
       
    BYTE  BkLight;      // �������� 1-100
    
    BYTE  SampMode;     // ȡ��ģʽ
    WORD  SampTime;     // ȡ��ʱ��
    float SampVol;      // ȡ�����
    float SampFlow[CHANNLE_NUM];  // ȡ������
    
    DWORD TotleTime; 
    float TotleFlow;

    BYTE  AlarmThres;   // ������ֵ��%

    BYTE  Enable;       // ͨ��ʹ�ܣ�1λָʾһ��ͨ��

    WORD  Sum;
}SYS_PARAM;

typedef struct
{
    bool  Running;
    DWORD RunTime;

    float Flow[CHANNLE_NUM];
    float Volume[CHANNLE_NUM];   // ȡ����� 
    float TotleFlow;            // ������
    float TotleVol;              // ��ȡ�����
}RUN_STATUS;



#define MAIN_Fosc        11059200UL    // 11.0592M
//#define MAIN_Fosc        35000000UL    // 35M

#define SysTick     9216   // 10ms    // ��/��, ϵͳ�δ�Ƶ��, ��4000~16000֮��

#define Timer0_Reload   (65536UL - SysTick)     //Timer 0 �ж�Ƶ��

#define OFFLINE_TIME  600

#define OUT_IO_COUNT      5
//#define LED_GREEN         0
//#define LED_YELLOW        1
//#define LED_RED           2
#define LIGHT_BLUE        0
#define LIGHT_YELLOW      1
#define GAS_BUMP          2
#define EX_FAN            3
#define ALARM_SOUND       4



#define MODE_TIME  1   // ��ʱȡ�� 
#define MODE_VOL   2   // ����ȡ��
#define MODE_MAN   3   // �ֶ�ȡ��


// output port 

#define ALM_CH_DISABLE    0
#define ALM_FLOW_NOR      1
#define ALM_FLOW_LOW      2
#define ALM_FLOW_HIGH     3
#define ALM_FLOW_ABNOR    4


#define LED_FLASH_TIME    500   // ms


#define RUN_LED(x) (x)?(P4 |= (1<<7)):(P4 &= ~(1<<7))    // ����LED

// ��ɫLED
//#define GRE_LED(x) (x)?(P0 |= (1<<2)):(P0 &= ~(1<<2))    // ��
//#define YEL_LED(x) (x)?(P0 |= (1<<3)):(P0 &= ~(1<<3))    // ��
//#define RED_LED(x) (x)?(P0 |= (1<<4)):(P0 &= ~(1<<4))    // ��

// ָʾ��
//#define RED_LIGHT(x) (x)?(P4 |= (1<<1)):(P4 &= ~(1<<1))      // ���
#define YEL_LIGHT(x) (x)?(P3 |= (1<<3)):(P3 &= ~(1<<3))      // �Ƶ�
#define BLU_LIGHT(x) (x)?(P3 |= (1<<2)):(P3 &= ~(1<<2))      // ����


#define STOP_M(x)     (P6 & (1<<6))      //��ͣ����

#define PW_MAIN(x)   (x)?(P6 |= (1<<4)):(P6 &= ~(1<<4))    // ����Դ����

#define RS485_EN1(x)  (x)?(P0 |= (1<<4)):(P0 &= ~(1<<4)) 
#define RS485_EN2(x)  (x)?(P5 |= (1<<2)):(P5 &= ~(1<<2))

#define BUMP_M(x)    (x)?(P6 |= (1<<7)):(P6 &= ~(1<<7))       //��ձÿ���
#define FANS_M(x)    (x)?(P5 |= (1<<0)):(P5 &= ~(1<<0))       //���ȿ���
#define ALARM(x)     (x)?(P3 |= (1<<4)):(P3 &= ~(1<<4))      // ������


#define POWER_LOCK()      (P4 & (1<<0))       // ���ػ���
#define LOCK_BIT()      (1<<0)

#define CMD_OUT_CTL      'O'  // 0x4F
#define CMD_IN_STATUE    'I'  // 0x49
#define CMD_GET_VER      'M'  // 0x56
#define CMD_DATA_TT      'T'  // 0x54 ����͸��
#define CMD_SET_ADD      'H'  // 0x48


#define KEY_ALM_CFM      3
#define POWER_ON               0xAA
#define POWER_OFF              0x55


#define RED       0
#define YELLOW    1
#define BLUE      2

extern SYS_PARAM xdata SysParam;
extern RUN_STATUS xdata RunStatus;
extern float SimFlow;
extern alt_u8 g_Output[OUT_IO_COUNT];
extern BYTE ChannelError[FLOW_METER_CNT];





void WriteParam();
void Delay(WORD ms);

void Error();
void OutCtl(alt_u8 id, alt_u8 st);

void ParamDef();

void Init();
void SaveParam();
void OpenPump();
void ClosePump();
void CheckAlarm();
void UpdataUI();

//void MainTask();
void DevRun();
void StartSamp();
void StopSamp(bool Auto);
void SendReadFlowCmd(BYTE ch);
void AbnorAlarm();
void SyncModBusDev();

void DebugMsg(char *msg);
void DebugInt(int msg);
void DumpCmd(BYTE *dat, BYTE len);

#endif

