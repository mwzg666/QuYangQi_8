#ifndef __LCD_H__
#define __LCD_H__

//#define CRC_ENABLE  

#define BIG_EDTION

#define LCD_HEAD   0x5AA5


#define LCD_CMD_WRITE  0x82
#define LCD_CMD_READ   0x83


// �������Ĵ���
#define LCD_REG_RTC    0x0010
#define LCD_SET_RTC    0x009C
#define LCD_REG_PAGE   0x0084    // ҳ���л�
#define LCD_REG_LIGHT 0x0082     // ��������


extern BYTE LcdFrameloc;
extern BYTE RecLength1;
extern BYTE xdata  SendBuf1[];
extern BYTE xdata RecvBuf1[];
extern BYTE Adc_Flag;

#pragma pack(1)

// Э��ͷ�ṹ
typedef struct
{
    WORD Head;       // 1 0x02 ֡ͷ
    BYTE Lenght;     // 2 ֡�� ����֡�ĳ���
    BYTE Cmd;        // 3 ����
    WORD Addr;
    BYTE Data[251];  // �������249 + CRC
}LCD_FRAME;

#pragma pack()


WORD SwWord(WORD input);
DWORD SwDWord(DWORD input);

void ClearRevBuf();
BOOL LcdCmd(BYTE Cmd, WORD Addr, BYTE *Data, BYTE Length);
BOOL ValidLcdFrame();
void HndLcdFrame();
void HndLcdData();

bool CheckDataLen(BYTE Len);
WORD PopWord();

void EnterPage(BYTE Page);
void GetLcdTime();
void SetLcdTime();
void SetBkLight(bool s);
void SetTextColor(WORD mp, WORD color);
void HideModule(WORD mp);
void ShowModule(WORD mp, WORD module);

#endif
