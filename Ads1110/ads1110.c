#include "system.h"
#include "i2c.h"
#include "ads1110.h"




#if 0
//������:
//GetAds1110
//��������:
//����ADS�����ʵ�ʵ�ѹ
//����:
//[����]:��
//[���]:��
//[����ֵ]:
// ADS1110�����ʵ�ʵ�ѹmv
int GetAds1110(BYTE id)
{
   int iVoltage = 0;
   unsigned char OutBuf[3]={0};
  
   I2C_Start();
   I2C_Write_Byte((ADS110_ADDR<<1)| 1);
   I2C_ReadAck();
   
   OutBuf[0] = I2C_Read_Byte(I2C_ACK);//gao
   OutBuf[1] = I2C_Read_Byte(I2C_ACK);//di
   OutBuf[2] = I2C_Read_Byte(I2C_NACK);//state
   I2C_Stop();
   
   iVoltage = (OutBuf[0] << 8) | OutBuf[1];
   iVoltage = (int)(1.0*iVoltage/16);
   
   return iVoltage;  //��λMV 
}
#endif

