#ifndef CMD_CMD_H_
#define CMD_CMD_H_


#define   UART_DATA_TIMEOUT      100
#define   SENSOR_CMD_TIMEOUT     200

#define   HEAD                      0x02       //֡��ͷ�ֽ�
#define   TAIL                      0x03       //֡��β�ֽ�

#define  LEN  1
#define  CMD  4
#define  DAT  5

//�����
#define CMD_DEV_CON                   0x43    //��C��//����
#define CMD_WRITE_DETER_PARA_W        0x57    // 'W'     //
#define CMD_WRITE_ALARM_PARA_B        0x42   //  'B'     //
#define CMD_SET_DEV_NUM               0x48    //��H��
#define CMD_READ_DOSE                 0x56    //��V��
#define CMD_READ_PUSLE                0x50    //��P��
#define CMD_READ_ALARM_PARA           0x46    //��F��
#define CMD_READ_DETER_PARA_R         0x52    //��R��
#define CMD_READ_DETER_PARA_J         0x4A    //��J��

#define CMD_READ_TEMP_T               0x54    //��T�� �¶Ȳ�ѯ
#define CMD_READ_RANGE_Q              0x51    //��Q�� ���̲�ѯ
#define CMD_READ_EDITION_M            0x4D    //��M���汾��ѯ
#define CMD_SOUND         'A'    // ��������
#define CMD_LED           'Y'    // ������
#define CMD_CERTAINKEY    'Z'    // ����ȷ�ϰ�ť
#define CMD_POWER         'N'    // ���ػ�
#define CMD_INPUT         'I'    // IO״̬
#define CMD_BATTERY       'G'    // ��ص���
#define CMD_VERSION       'M'    // �汾

#define CMD_OUT_4_20MA     0x03     // 4_20MA �������
#define CMD_GET_4_20MA     0x04     // ��4_20MA ����


#pragma pack(1)



typedef union
{
    alt_u8 ByteWhole;
    struct
    {   
        alt_u8 bit0:1;
        alt_u8 bit1:1;
        alt_u8 bit2:1;
        alt_u8 bit3:1;
        alt_u8 bit4:1;
        alt_u8 bit5:1;
        alt_u8 bit6:1;
        alt_u8 bit7:1;
    }BIT;
}BYTE_BIT;


// 0�ر�   1�� 2��˸
typedef struct
{
    char StateLed_Green;      // 0
    char StateLed_Yellow;     // 1
    char StateLed_Red;        // 2
    char AlarmLed_RED;        // 3
    char AlarmLed_Yellow;     // 4
}LED_PARAM;


typedef struct 
{
    float DOSE_RATE_ALARM_1;//�����ʱ���1
    float DOSE_RATE_ALARM_2;//�����ʱ���2
    float CUM_DOSE_RATE_ALARM_1;//�ۻ������ʱ���1
    float CUM_DOSE_RATE_ALARM_2;//�ۻ������ʱ���2
    float INVALID_ALRAM_1;//ʧЧһ������
    float INVALID_ALRAM_2;//ʧЧ��������
}HOST_ALRAM_PARA;// ������������

typedef struct 
{
    float LOW_REVISE_COE_A;//����������ϵ��A
    float LOW_REVISE_COE_B;//����������ϵ��B
    #if 1
    float LOW_REVISE_COE_C;//����������ϵ��C
    float HIG_REVISE_COE_A;//����������ϵ��A
    float HIG_REVISE_COE_B;//����������ϵ��B
    float HIG_REVISE_COE_C;//����������ϵ��C
    float SUPER_REVISE_COE_A;//������������ϵ��A
    float SUPER_REVISE_COE_B;//������������ϵ��B
    float SUPER_REVISE_COE_C;//������������ϵ��C
    #endif
    alt_u32 DET_THR_1;//̽������ֵ1
    alt_u32 DET_THR_2;//̽������ֵ2
    alt_u32 DET_THR_3;//̽������ֵ3
    alt_u32 DET_TIME;//̽����ʱ��
    alt_u32 HV_THR;//��ѹ��ֵ��ֵ
}HOST_SENSOR_PARAM;//д̽��������

typedef struct 
{
    float DOSE_RATE;//������
    float ACC_DOSE_RATE;//�ۻ�����
    float WAITINGFORUSE;//����
    alt_u32 PULSE1;//����1
    alt_u32 PULSE2;//����2

    /*
    bit7   ������һ��������
    bit6λ �����ʶ���������
    bit5λ �ۼƼ�����һ��������
    bit4λ �ۼƼ����ʶ���������
    bit3λ ���ر�����       
    bit2λ ʧЧһ��������
    bit1λ ʧЧ����������
    bit0λ �ͱ��׹���
    */
    BYTE_BIT ALARM_STATUS;//����״̬
    //INF_SOUND_CERTAIN CERTAIN_KEY;//��ť��
    alt_u32 PULSE3;//����3 -- �ٽ�̽ͷר��
}HOST_COUNT_PULSE;//��������


//ϵͳ���ò���
typedef struct
{
	float Canshu1;	 //������ͨ��У׼����C
	float Canshu2;	// ������ͨ��У׼����C 	
	char yuzhi1[4];	//
	char yuzhi2[4];	//
	char PingHuaShiJian[4];//ƽ��ʱ��,���͵�ʱ����Ҫת��Ϊ4�ֽ�ASC�ַ�����
}SENSOR_PARAM;

//ϵͳ���ò���
typedef struct
{
	float DI_A;	//������ͨ��У׼����A		
	float DI_B;	// ������ͨ��У׼����B 	
	float DI_C;	//������ͨ��У׼����C	=	SYS_PRAM.Canshu1
	float GAO_A;	// ������ͨ��У׼����A		
	float GAO_B;	// ������ͨ��У׼����B 
	float GAO_C;	// ������ͨ��У׼����C 	=	SYS_PRAM.Canshu2
}SENSOR_JIAOZHUNYINZI;

//������Ϣ
typedef struct
{
    float  DoseRatePreAlarm;  	//������һ������(uSv/h)
    float  DoseRateAlarm;  		//�����ʶ�������(uSv/h)
    float  DosePreAlarm;  		//�ۼƼ���Ԥ����ֵ
    float  DoseAlarm;  			//�ۼƼ���������ֵ
}SENSOR_ALARM;

//�����ṹ��    
typedef struct
{
    float  DoseRate;  
    float  Dose;
	alt_u8 State;	
    //״̬��: ������Ϊ��Ч��������Ϊ��Ч��
    //  bit7: ������һ��������
    //  bit6: �����ʶ���������
    //  bit5: �ۼƼ�����һ��������
    //  bit4: �ۼƼ����ʶ���������
    //  bit3: ���ر���
    //  bit2: �ͱ��׹��ϣ�
    //  bit������λδ����
}SENSOR_DOSERATE;


typedef struct
{
    BYTE Charging;
    WORD Vol;       // ��ѹ/10:����һλС�� 
}BAT_INFO;



#pragma pack()


bool ConnectSensor(alt_u8 Addr);
bool ReadDoseRate(alt_u8 Addr);

bool ReadAlarmParam(alt_u8 Addr);
bool WriteAlarmParam(alt_u8 Addr);
bool ReadSensorParam(alt_u8 Addr);
bool WriteSensorParam(alt_u8 Addr);


void ClearRecvData(UART_DATA *pUartData);
bool ValidFrame(UART_DATA *pUartData);
void MakeFrame(UART_DATA *pUartData, alt_u8 Addr, alt_u8 Cmd, alt_u8 *dat, alt_u8 length);
bool WaitSensorAck(alt_u8 Addr, alt_u8 Cmd);

bool HndPcFrame();
bool HndPcCmd();
void SendPcCmd(alt_u8 Addr, alt_u8 Cmd, alt_u8 *dat, alt_u8 length);


bool SoundCtl(alt_u8 Ctl);
bool LedCtl(alt_u8 *led);
bool DevVer(alt_u8 Addr);
bool Out4_20ma(BYTE index, BYTE val);
bool Read4_20ma();

bool ReadBatVol();

#endif /* CMD_CMD_H_ */
