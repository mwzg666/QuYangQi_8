#ifndef __LCDAPP_H__
#define __LCDAPP_H__

#define BUTTON_OK   0x0D0A
#define LOGIN_PASSWORD  1689



// �豸�Ĵ���
// ��������
#define REG_STRART    0x1900    // ��ʼ��ť
#define REG_PASSWORD   0x1901
#define REG_MODEHINT   0x1800

// ������
#define REG_DEV_CTL    0x1080   // ��ͣ��ť
#define REG_RETRUN     0x1081   // ���ذ�ť
#define REG_TIME_BTN   0x1082   // ���ʱ�� 
#define REG_CH_FLOW    0x1000   // ���� 0x1000 -- 0x1007
#define REG_CH_STATUS  0x1200   // ͨ��״̬

#define REG_HINT_ENTER  0x1083  // ȡ��������ʾ���ϵ�ȷ�ϰ�ť 0x0D0A
#define REG_HINT_END    0x1084  // ȡ��������ʾ�������ַ
#define MP_HINT_END     0xA800  // ȡ��������ʾ������ָ��


// �������ý���
#define REG_SYS_PARAM  0x4000   // ��Ļ������ʼ��ַ
#define REG_SP_FLOW1  0x4000
#define REG_SP_FLOW2  0x4001
#define REG_SP_FLOW3  0x4002
#define REG_SP_FLOW4  0x4003
#define REG_SP_FLOW5  0x4004
#define REG_SP_FLOW6  0x4005
#define REG_SP_FLOW7  0x4006
#define REG_SP_FLOW8  0x4007
#define REG_SP_ADDR   0x4008
#define REG_SP_TIME   0x4009
#define REG_SP_VOL    0x400A
#define REG_SP_LIGHT  0x400B
#define REG_SP_THRES  0x400C
#define REG_SP_MODE   0x400D
#define REG_CH_ONOFF  0x400E

#define REG_MODE_TXT 0x4010
#define REG_SP_RET    0x4020

// �޸�ʱ��
#define REG_ADJ_TIME   0x2010  // ���ذ�ť
#define REG_DATE_TIME  0x2000  // 7����ַ

// IO����
#define REG_IO_BUMP     0x5000
#define REG_IO_BLUE    0x5001
#define REG_IO_YELLOW   0x5002
//#define REG_IO_RED      0x5003
#define REG_IO_SOUND    0x5004
#define REG_IO_FAN      0x5005
#define REG_IO_EMSTOP   0x5006
#define REG_IO_TEMP     0x5007


// ���ڽ���
#define REG_DEV_TYPE  0x7000    // �豸�ͺ�
#define REG_SOFT_VER  0x7010    // ����汾
#define REG_CH_COUNT  0x7020    // ͨ������

#define PAGE_START   0
#define PAGE_MAIN   1
#define PAGE_SET    4


#pragma pack(1)
typedef struct
{
    WORD Flow;    // ������0.1L/min
    WORD Vol;     // �����0.1m3,���6553m3
}CHANNEL_FLOW;

typedef struct
{
    DWORD Flow;  // ������0.1L/min
    DWORD Vol;   // �����0.1m3
}TOTLEFLOW;


// ��ʾ�����н�����Ϣ
typedef struct
{
    CHANNEL_FLOW ChFlow[CHANNLE_NUM];
    TOTLEFLOW    TotFlow;
    char         RunTime[16];
}RUN_INFO;


typedef struct
{
    WORD Flow[CHANNLE_NUM];
    WORD Address;
    WORD SampTime;
    WORD SampVol;
    WORD BkLight;
    WORD AlarmThres;
    WORD SampMode;
    WORD ChOnOff;    // ʹ�õ�8λ��1λָʾһ��ͨ��
    WORD res;
    char ModeTxt[16];  // ȡ��ģʽ
}SHOW_PARAM;


typedef struct
{
    WORD YearH;
    WORD YearL;
    WORD Month;
    WORD Day;
    WORD Hour;
    WORD Minute;
    WORD Secend;
}LCD_DATE_TIME;

typedef struct
{
    char DevType[16];
    char SoftVer[16];
}DEV_INFO;
#pragma pack()


extern RUN_INFO xdata RunInfo;
extern char xdata ChannelStatus[CHANNLE_NUM][16];
extern BYTE ChannelAlarm[CHANNLE_NUM];
extern BYTE HisAlarm[CHANNLE_NUM];
extern bool NeedGetRet;

void EnterPage(BYTE Page);
void ShowFlow();
void ShowStatus();
void SendParam();
void ShowDevInfo();
void SetStartBtn(BYTE s);
void StatusColor(bool force);

void ReadReg();
void WriteReg();
void ModeHint();
void GetRetCode();

void ShowTemp(u16 TEMPER);
void ShowEmStop(bool on);

#endif
