#include "main.h"
#include "mcp4725.h"
#include "Lcd.h"
#include "LcdApp.h"
#include "ModBus.h"
#include "Temper.h"
#include "FlowMeter.h"
#include "ModBusDev.h"
#include "ModBusHost.h"


BYTE code VERSION = 100;  // V1.0.0

BYTE xdata StrTmp[64] = {0};
BYTE xdata Valve[8] = {0};

BYTE ChannelError[FLOW_METER_CNT] ={0};

#define Log //((CSampDemoDlg *)theApp.m_pMainWnd)->AddLog

#define PARAM_SIGN  0x3132
SYS_PARAM xdata SysParam;
RUN_STATUS xdata RunStatus;

float SimFlow = 35.0;

u16 SendFlowTim = 0;
BYTE SendFlowFlag = 0;

u16 LcdBusyTim = 0;
BYTE LcdBusyFlag = 0;
BYTE ChNum = 1;

alt_u8 g_Output[OUT_IO_COUNT]      = {0,0,0,0,0};   // �ϵ������� // 
alt_u8 g_OutStatus[OUT_IO_COUNT]   = {0,0,0,0,0};


u16  Timer0Cnt = 0;

BYTE g_Key_Confrom  = 0; 
BYTE g_Key_Power  = 0; 
BYTE g_Key_Input  = 0; 
BYTE Input_Status = 0;

WORD gRunTime = 0;

void DebugMsg(char *msg)
{
    BYTE len = (BYTE)strlen(msg);
    //Uart1Send((BYTE *)msg,len);
}

void DebugInt(int msg)
{
    memset(StrTmp,0,64);
    sprintf(StrTmp,"%x\r\n",msg);
    DebugMsg(StrTmp);
}

void DumpCmd(BYTE *dat, BYTE len)
{
    BYTE i;
    memset(StrTmp,0,64);
    for (i=0;i<len;i++)
    {
        if (strlen(StrTmp) >= 60)
        {
            break;
        }
        sprintf(&StrTmp[i*3], "%02X ", dat[i]);
    }
    sprintf(&StrTmp[i*3], "\r\n");
    DebugMsg(StrTmp);
}


void Error()
{
    while(1)
    {
        RUN_LED(1);
        Delay(50);
        RUN_LED(0);
        Delay(50);
    }
    
}


void SysInit()
{
    HIRCCR = 0x80;           // �����ڲ�����IRC
    while(!(HIRCCR & 1));
    CLKSEL = 0;              
}

void IoInit()
{
    EAXFR = 1;
    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���

    P0M1 = 0x00;   P0M0 |= (1<<4) ;                     // P0.0 P0.1 P0.4 �������
    P1M1 = (1<<4)|(1<<3);   P1M0 = 0x00;                       //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 |= 0x00;                      // P2.2 �������
    P3M1 = 0x00;   P3M0 |= (1<<2)|(1<<3)|(1<<4);        //����Ϊ׼˫���
    P4M1 = (1<<0);   P4M0 = 0x00;                       //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 |= (1<<0) | (1<<2);             //����Ϊ׼˫���
    P6M1 = (1<<6); P6M0 |= (1<<4) | (1<<7);     //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;                         //����Ϊ׼˫���
}


void SensorInit()
{
    // P1.0 -- �½�Ե�ж�
    P1IM0 = 0;
    P1IM1 = 0;

    // ���ȼ�2
    //PIN_IP  |= (1<<1);
    PINIPH |= (1<<1);
    //P1_IP  = 1; // |= (1<<1);
    //P1_IPH = 1; //|= (1<<1);

    // �����ж�
    P1INTE |= (1<<0) | (1<<1) | (1<<4) | (1<<5);
}



void Timer0Init()
{
    AUXR = 0x00;    //Timer0 set as 12T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    
    // �ж����ȼ�3
    PT0  = 1;
    PT0H = 0;
}

// 10ms �ж�һ��
void Timer0Int (void) interrupt 1
{
    Timer0Cnt ++;
}

#if 0
// �����жϷ������
void CommInt (void) interrupt 13
{
    u8 intf =  P1INTF;
    
    if (intf)
    {
        P1INTF = 0;

        if (intf & (1<<0))  // P1.0 �ж�
        {
            Counter[0] ++;
        }

        if (intf & (1<<1))  // P1.1 �ж�
        {
            Counter[1] ++;
        }

        if (intf & (1<<4))  // P1.4 �ж�
        {
            Counter[2] ++;
        }

        if (intf & (1<<5))  // P1.5 �ж�
        {
            Counter[3] ++;
        }
    }
    
}
#endif


void OutCtl(alt_u8 id, alt_u8 st)
{
    if (g_OutStatus[id] == st)
    {
        return;
    }

    g_OutStatus[id] = st;
    
    switch(id)
    {   
        case LIGHT_YELLOW: 
        {
            (st)? YEL_LIGHT(1):YEL_LIGHT(0);
            break;
        }

        case LIGHT_BLUE: 
        {
            (st)? BLU_LIGHT(1)   :BLU_LIGHT(0); 
            break;
        }
        
        case GAS_BUMP:      //��
        {
            (st)? BUMP_M(1) : BUMP_M(0);
            break;
        }

        case EX_FAN:        //����
        {
            (st)? FANS_M(1) : FANS_M(0);
            break;
        }

        case ALARM_SOUND:   //����
        {
            (st)? ALARM(1) : ALARM(0);       
            break;
        }
    }

    
}

void OutFlash(alt_u8 id)
{
    static alt_u16 OutTimer[OUT_IO_COUNT] = {0,0,0,0,0};
    if (OutTimer[id] ++ > LED_FLASH_TIME/10)
    {
        OutTimer[id] = 0;
        if (g_OutStatus[id] == 1)
        {
            OutCtl(id, 0);
        }
        else
        {
            OutCtl(id, 1);
        }
    }
}

void IoCtlTask()
{
    alt_u8 i;
    for (i=0;i<OUT_IO_COUNT;i++)
    {
        if (g_Output[i] == 2)
        {
            OutFlash(i);
        }
        else
        {
            OutCtl(i, g_Output[i]);
        }
    }
}

// ����ָʾ��
void RunLed(u16 dt)
{   
    static u16 tm = 0;
    u16 to = 3000;
    tm += dt;

    if (tm > to)
    {
        tm = 0;
        RUN_LED(0);
    }
    else if (tm > (to-100))
    {
        RUN_LED(1);
    }
}



void Task1s()
{
    static BYTE tm = 0;

    CLR_WDT = 1;  // ι��
    
    tm++;
    if(tm == 10)
    {
        ADC_Temp();
        SyncModBusDev();
        tm = 0;
    } 
    
    if((tm == 6)||(tm == 3))
    {
        GetRetCode();
    }
    
    if(tm == 9)
    {
        if (RunStatus.Running)
        {  
            DevRun();
        }
    }
}


void TimerTask()
{
    u16 delta = 0;
    static u16 Time1s = 0;
    
    if (Timer0Cnt)
    {
        delta = Timer0Cnt * 10;
        Timer0Cnt = 0;

        if (RX2_Cnt > 0)
        {
            Rx2_Timer += delta;
        }

        if(RX3_Cnt > 0)
        {
            Rx3_Timer += delta;
        }
        
        if(RX4_Cnt > 0)
        {
            Rx4_Timer += delta;
        }
        
        SendFlowTim += delta;
        if(SendFlowTim > 220)
        {
            SendFlowTim = 0;
            SendFlowFlag = 1;
        }

        if (gRunTime < 5000)
        {
            gRunTime += delta;
        }

        //if (g_CommIdleTime < 300)
        //{
            //g_CommIdleTime += delta;
        //}

        #ifdef IRDA_FUN
        if (IrDAStart == 1)
        {
            IrDATimer += delta;
        }
        #endif

        Time1s += delta;
        if (Time1s >= 100)
        {
            Time1s = 0;
            Task1s();
        }

        RunLed(delta);
        IoCtlTask();
    }
}



void Delay(WORD ms)
{
    WORD t = 1000;
    while(ms--)
    {
        for (t=0;t<1000;t++) ;
    }
}


WORD ParamCheck(BYTE *buf, WORD len)
{
    WORD dwSum = 0;
    WORD i;

    for (i = 0; i < len; i++)
    {
        dwSum += buf[i];
    }

    return dwSum;
}

/*
void DefSenParam()
{
    BYTE i;
    for (i=0; i<SENSOR_COUNT; i++)
    {
        SysParam.SenParam[i].LOW_REVISE_COE_A = 1;
        SysParam.SenParam[i].LOW_REVISE_COE_B = 1;
        SysParam.SenParam[i].LOW_REVISE_COE_C = 1;

        SysParam.SenParam[i].HIG_REVISE_COE_A = 1;
        SysParam.SenParam[i].HIG_REVISE_COE_B = 1;
        SysParam.SenParam[i].HIG_REVISE_COE_C = 1;

        SysParam.SenParam[i].SUPER_REVISE_COE_A = 1;
        SysParam.SenParam[i].SUPER_REVISE_COE_B = 1;
        SysParam.SenParam[i].SUPER_REVISE_COE_C = 1;

        SysParam.SenParam[i].DET_THR_1 = 500;
        SysParam.SenParam[i].DET_THR_2 = 150;
        SysParam.SenParam[i].DET_THR_3 = 150;

        SysParam.SenParam[i].DET_TIME = 1000;
        SysParam.SenParam[i].HV_THR = 1000;
    }
}
*/

/*
void DefSenAlarm()
{
    BYTE i;
    for (i=0; i<SENSOR_COUNT; i++)
    {
        SysParam.AlmParam[i].DOSE_RATE_ALARM_1 = 300;
        SysParam.AlmParam[i].DOSE_RATE_ALARM_2 = 400;
        SysParam.AlmParam[i].CUM_DOSE_RATE_ALARM_1 = 300;
        SysParam.AlmParam[i].CUM_DOSE_RATE_ALARM_2 = 400;
        SysParam.AlmParam[i].INVALID_ALRAM_1 = 8000;
        SysParam.AlmParam[i].INVALID_ALRAM_2 = 10000;
    }
}
*/

void ReadParam()
{
    EEPROM_read(0, (BYTE *)&SysParam, sizeof(SYS_PARAM));

    #if 0
    memset(StrTmp,0,32);
    sprintf((char *)StrTmp,"%d \r\n",sizeof(SYS_PARAM));
    DebugMsg((char *)StrTmp);

    memset(StrTmp,0,32);
    sprintf((char *)StrTmp,"%d \r\n",sizeof(HOST_SENSOR_PARAM));
    DebugMsg((char *)StrTmp);

    memset(StrTmp,0,32);
    sprintf((char *)StrTmp,"%d \r\n",sizeof(HOST_ALRAM_PARA));
    DebugMsg((char *)StrTmp);

    memset(StrTmp,0,32);
    sprintf((char *)StrTmp,"%d \r\n",sizeof(float));
    DebugMsg((char *)StrTmp);
    
    //Rs485Send((BYTE *)&SysParam, sizeof(SYS_PARAM));
    
    
    if (SysParam.Sign != PARAM_SIGN)
    {
        DebugMsg("Sign error. \r\n");
    }

    if (SysParam.Sum != ParamCheck((BYTE *)&SysParam, sizeof(SYS_PARAM)-2))
    {
        DebugMsg("Param Check error. \r\n");
    }
    #endif

   
    if ( (SysParam.Sign != PARAM_SIGN) ||
         (SysParam.Sum != ParamCheck((BYTE *)&SysParam, sizeof(SYS_PARAM)-2)) )
    {
        //SysParam.Sign = PARAM_SIGN;
        //SysParam.Address = 1;
        ParamDef();
        //DefSenParam();
        //DefSenAlarm();
        WriteParam();

        //DebugMsg("Def Param. \r\n");
    }
}


void WriteParam()
{
    EA = 0;    
    
    EEPROM_SectorErase(0);
    EEPROM_SectorErase(512);
    SysParam.Sum = ParamCheck((BYTE *)&SysParam, sizeof(SYS_PARAM)-2);
    if (!EEPROM_write(0, (BYTE *)&SysParam, sizeof(SYS_PARAM)))
    {
        Error();
    }

    EA = 1;     //�����ж�
}

BYTE GetInput()
{
    // ��ǰֻ��һ�����ػ�״̬ P2.1
    static BYTE his = LOCK_BIT();
    BYTE st = POWER_LOCK();

    if (st != his)
    {
        Delay(50);
        if ( st == POWER_LOCK() )
        {
            his = st;
            return st;
        }
    }

    return 0xFF;
}


void PowerOff()
{
    PW_MAIN(0);

    while(1)
    {
        ;
    }
}

void HndInput()
{
    static bool em = false;
    if(STOP_M() == 0)
    {
        Delay(10);
        if (STOP_M() == 0)
        {
            if (RunStatus.Running)
            {
                StopSamp(false);
            }
        }

        if (em == false)
        {
            em = true;
            ShowEmStop(em);
        }
    }
    else
    {
        if (em)
        {
            em = false;
            ShowEmStop(em);
        }
    }
}

/*
void ReportInput()
{
    BYTE PwOff = POWER_OFF;
    
    if (g_CommIdleTime > 200)
    {
        if (g_Key_Confrom)
        {
            g_Key_Confrom = 0;
            SendPcCmd(0, CMD_CERTAINKEY, NULL, 0);
            return;
        }

        if (g_Key_Power)
        {
            g_Key_Power = 0;
            SendPcCmd(0, CMD_POWER, &PwOff, 1);
            return;
        }

        #if 0
        if (g_Key_Input)
        {
            g_Key_Input = 0;
            SendPcCmd(0, CMD_INPUT, &Input_Status, 1);
        }
        #endif
    }
}
*/

void LedInit()
{
    // ��ʼ״̬��Ϊ0
    
    // ��ɫLED
    //GRE_LED(0);    // ��
    //YEL_LED(0);    // ��
    //RED_LED(0);    // ��

    // ָʾ��
    //RED_LIGHT(0); // ���
    YEL_LIGHT(0);   // �Ƶ�
    BLU_LIGHT(0);   // ����
    CloseValve();   // ��ŷ�
    BUMP_M(0);      // ��
    FANS_M(0);      // ����
    ALARM(0);       // ������
}


void ParamDef()
{
    BYTE i;
    
    SysParam.Sign     = PARAM_SIGN;
    SysParam.Address = 1;
    SysParam.BkLight = 50;

    SysParam.SampMode = MODE_TIME;
    SysParam.SampTime = 5;  
    SysParam.SampVol   = 2;
    SysParam.AlarmThres   = 10;
    for (i=0;i<CHANNLE_NUM;i++)
    {
        SysParam.SampFlow[i] = 35;
    }

    SysParam.Enable = 0x1F;
}

void SaveParam()
{
    //CString t;
    //t.Format(_T("SaveParam: %02X\r\n"), SysParam.Enable);
    //Log(t);
    //DebugMsg("123");
    WriteParam();
}


void UpdataUI()
{
    BYTE i;
    for (i=0;i<CHANNLE_NUM;i++)
    {
        ChannelAlarm[i] = ((SysParam.Enable & (1<<i)) == 0)?0:1;
    }
    ShowStatus();
    Delay(200);
    StatusColor(true);
}

void InitLcd()
{   
    memset(&RunStatus, 0, sizeof(RUN_STATUS));
    memset(&RunInfo, 0, sizeof(RUN_INFO));
    memset(&RealFlow, 0, sizeof(RealFlow));
    
    //ParamDef();
    ModeHint();
    Delay(200);
    HideModule(MP_HINT_END);
    Delay(200);
    UpdataUI();    
    Delay(200);
    SendParam();
    Delay(200);
    SetBkLight(false);
    Delay(200);
    ShowDevInfo();
    Delay(200);
}


void GetFlow()
{
    BYTE i;
    WORD  w;
    DWORD d;

    
    for (i=0;i<CHANNLE_NUM;i++)
    {
        if (SysParam.Enable & (1<<i))
        {
            RunStatus.Flow[i] = RealFlow[i].val;  // ģ�� -- ʵ��Ҫ���������ж�ȡ
            w = (WORD)(RunStatus.Flow[i]*10);
            RunInfo.ChFlow[i].Flow = SwWord(w);
            
            RunStatus.Volume[i] =  RealFlow[i].Totol; 
            w = (WORD)(RunStatus.Volume[i]*10);
            RunInfo.ChFlow[i].Vol = SwWord(w);
        }
    }

    // ������
    RunStatus.TotleFlow = RealFlow[8].val;
    d = (DWORD)(RealFlow[8].val*10);
    RunInfo.TotFlow.Flow = SwDWord(d);

    // �����
    RunStatus.TotleVol = RealFlow[8].Totol;
    d = (DWORD)(RealFlow[8].Totol*10);
    RunInfo.TotFlow.Vol   = SwDWord(d);
}

void StartSamp()
{
    memset(&RunStatus, 0, sizeof(RUN_STATUS));
    memset(&RunInfo, 0, sizeof(RUN_INFO));
    memset(&RealFlow, 0, sizeof(RealFlow));

    RunStatus.Running = true;
    g_Output[LIGHT_BLUE] = 1;
    GetValve();
    OpenPump();
    SetStartBtn(0);

}

void StopSamp(bool Auto)
{
    ClosePump();
    memset(RealFlow,0, sizeof(RealFlow));

    RunStatus.Running = false;
    g_Output[LIGHT_BLUE] = 0;
    
    SetStartBtn(1);  // ��ť�Զ���Ϊ����ʼ��
    
    if (Auto)  // �Զ�����
    {
        // ��ʾȡ��������ʾ��
        ShowModule(MP_HINT_END, REG_HINT_END);
    }
}

// ��ʱģʽ
void TimingMode()
{
    if (RunStatus.RunTime >= ((DWORD)SysParam.SampTime) * 60)
    {
        StopSamp(true);
    }
}


// ����ģʽ
void VolumeMode()
{
    if (RunStatus.TotleVol >= SysParam.SampVol)
    {
        StopSamp(true);
    }
}


void RunCheck()
{
    switch (SysParam.SampMode)
    {
        case MODE_TIME:  TimingMode();  break;
        case MODE_VOL:   VolumeMode();  break;
    }
}

void AbnorAlaerm()
{   
    BYTE i;
    bool HaveAlarm = false;

    
    
    for(i = 0;i < CHANNLE_NUM;i++)
    {
        if( (ChannelAlarm[i] == ALM_FLOW_ABNOR) ||
            (ChannelAlarm[i] ==  ALM_FLOW_LOW) || 
            (ChannelAlarm[i] == ALM_FLOW_HIGH)  )
        {
            HaveAlarm = true;
            break;
        }
    }

    if (HaveAlarm)
    {
        g_Output[LIGHT_YELLOW] = 1;
        g_Output[ALARM_SOUND] = 2;
    }
    else
    {
        g_Output[LIGHT_YELLOW] = 0;
        g_Output[ALARM_SOUND] = 0;
    }
}
void CheckAlarm()
{
    BYTE i;
    float flow;
    
    for (i=0;i<CHANNLE_NUM;i++)
    {
        if (SysParam.Enable & (1<<i))
        {
            flow = RunStatus.Flow[i];
            if (flow > SysParam.SampFlow[i]*(100+SysParam.AlarmThres)/100)
            {
                ChannelAlarm[i] = ALM_FLOW_HIGH;
            }
            else if (flow < SysParam.SampFlow[i]*(100-SysParam.AlarmThres)/100)
            {
                ChannelAlarm[i] = ALM_FLOW_LOW;
            }
            else
            {
                ChannelAlarm[i] = ALM_FLOW_NOR;
            }
        }
        else
        {
            ChannelAlarm[i] = ALM_CH_DISABLE;
        }
        
        if( (ChannelError[i] > 3) && (ChannelAlarm[i] != ALM_CH_DISABLE) )
        {
            ChannelAlarm[i] = ALM_FLOW_ABNOR;
        }
    }

    if (memcmp(HisAlarm,ChannelAlarm,CHANNLE_NUM) != 0)
    {
        // �����б仯�Ÿ��½���
        Delay(200);
        ShowStatus();
        Delay(200);
        AbnorAlaerm();
        StatusColor(false);

        memcpy(HisAlarm,ChannelAlarm,8);
    }
}

// 1������һ��
void DevRun()
{
    RunStatus.RunTime ++;
    
    // 1. ��ȡ����
    GetFlow();

    // 2. ��ʾ������״̬
    ShowFlow();
    //Delay(200);
    
    // 3. ��鱨��״̬  
    if (RunStatus.RunTime > 10)
    {
        // ����ʱ�����10��ż��
        CheckAlarm();
    }
    
    // 4. ����ģʽ�ж��Ƿ����ȡ��
    RunCheck();
}

/*
void MainTask()
{
    
}
*/

void GetValve()
{
    BYTE i;
    for(i = 0;i<CHANNLE_NUM;i++)
    {
        if(SysParam.Enable & (1<<i))
        {
           Valve[i] = 1;
        }
        else
        {
            Valve[i] = 0;
        }
    }
    CheckValve();
}



//������ر�ͨ����ŷ�
void CheckValve()
{
    BYTE i;
    for(i = 0;i<CHANNLE_NUM;i++)
    {
        if(Valve[i])
        {
            switch(i)
            {
                case 0 : VALVE0(1); break;
                case 1 : VALVE1(1); break;
                case 2 : VALVE2(1); break;
                case 3 : VALVE3(1); break;
                case 4 : VALVE4(1); break;
                case 5 : VALVE5(1); break;
                case 6 : VALVE6(1); break;
                case 7 : VALVE7(1); break;  
            }
        }
        else
        {
            switch(i)
            {
                case 0 : VALVE0(0); break;
                case 1 : VALVE1(0); break;
                case 2 : VALVE2(0); break;
                case 3 : VALVE3(0); break;
                case 4 : VALVE4(0); break;
                case 5 : VALVE5(0); break;
                case 6 : VALVE6(0); break;
                case 7 : VALVE7(0); break;  
            }
        }
    }
    Delay(20);
}

void CloseValve()
{
    VALVE0(0);
    VALVE1(0);
    VALVE2(0);
    VALVE3(0);
    VALVE4(0);
    VALVE5(0);
    VALVE6(0);
    VALVE7(0);
    Delay(20);

}
// ��������
void OpenPump()
{
    //BUMP_M(1);
    g_Output[GAS_BUMP] = 1;
}


// ֹͣ����
void ClosePump()
{
    //BUMP_M(0);
    CloseValve();
    g_Output[GAS_BUMP] = 0;
}

void SendReadFlowCmd(BYTE ch)
{
    ChannelError[ch-1] ++;
    SendReadFlow(ch);    
}

WORD GetAlarm()
{
    BYTE i;

    for (i=0;i<8;i++)
    {
        if (ChannelAlarm[i] ==  ALM_FLOW_ABNOR)
        {
            return ALM_FLOW_ABNOR;
        }

        if (ChannelAlarm[i] ==  ALM_FLOW_HIGH)
        {
            return ALM_FLOW_HIGH;
        }

        if (ChannelAlarm[i] ==  ALM_FLOW_LOW)
        {
            return ALM_FLOW_LOW;
        }
    }

    return ALM_FLOW_NOR;
}

void SyncModBusDev()
{
    BYTE i;
    memset(&ModBusParam, 0, sizeof(MODBUS_PARAM));
    ModBusParam.AlamrThres = SysParam.AlarmThres;
    ModBusParam.ChEnable =   SysParam.Enable;

    for (i=0;i<8;i++)
    {
        ModBusParam.ChFlow[i] = (WORD)(SysParam.SampFlow[i]);
    }
    ModBusParam.SampFlow = SysParam.SampVol;
    ModBusParam.SampMode = SysParam.SampMode;
    ModBusParam.SampTime = SysParam.SampTime;
    ModBusParam.SampVol = SysParam.SampVol;

    memset(&ModBusStatus, 0, sizeof(MODBUS_STATUS));
    ModBusStatus.TotleTime = SysParam.TotleTime;
    ModBusStatus.TotleVol = SysParam.TotleFlow;

    ModBusStatus.SampleFlow = RunStatus.TotleFlow;
    ModBusStatus.SampleVol = RunStatus.TotleVol;

    ModBusStatus.Alarm = GetAlarm();
    ModBusStatus.RemTime = ((DWORD)SysParam.SampTime) * 60 - RunStatus.RunTime;
    ModBusStatus.RunTime = RunStatus.RunTime;
    ModBusStatus.RunStatus = RunStatus.Running;

    for (i=0;i<8;i++)
    {
        ModBusStatus.ChFlow[i] = (WORD)(RunStatus.Flow[i]);
        ModBusStatus.ChVol[i] = (WORD)(RunStatus.Volume[i]);
    }

    memset(&ModBusInfo, 0, sizeof(MODBUS_INFO));
    ModBusInfo.Address = SysParam.Address;
    ModBusInfo.Version = VERSION;
}



void FlowTask()
{
    if (RunStatus.Running)
    {
        if(SendFlowFlag == 1)
        {
            SendFlowFlag = 0;
            SendReadFlowCmd(ChNum++);
        }
        
        if (ChNum>9)
        {
            ChNum = 1;
        }
    }
}

void main(void)
{
    SysInit();
    IoInit();
    PW_MAIN(0);  // ����Դ
    LedInit();
    
    RUN_LED(1);
   
    Delay(200);
    
    Timer0Init();
    Delay(200);
    Adc_Init();
    Delay(200);
    
    UART1_config();
    UART2_config();
    UART3_config();
    UART4_config();
    ClearUart1Buf();
    ClearUart2Buf();
    ClearUart3Buf();
    ClearUart4Buf();
    
    // ��CPU�ȶ����ٶ�����
    Delay(500);
    ReadParam();
    Delay(200);

    RUN_LED(0);

    #if 0
    while(1)
    {
        RUN_LED(0);
        Delay(800);
        RUN_LED(1);
        Delay(200);
    }
    #endif
    
    
    EA = 1;     //�����ж�

    WDT_CONTR |= (1<<5) |  7;  // �������Ź���Լ8��

    Delay(200);
    InitLcd();
    
    
    while(1)
    {
        TimerTask();
        HndInput();
        
        Uart1Hnd();
        Uart2Hnd();
        Uart3Hnd();
        FlowTask(); 

        Uart4Hnd();
    }
}


