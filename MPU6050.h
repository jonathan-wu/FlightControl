#include "Global.h"
#ifdef MPU6050_Used_
#ifndef MPU6050_H_
#define MPU6050_H_
#define MPU6050_Address         0x68
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	        0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_XOUT_H	        0x3B
#define	WHO_AM_I		0x75

typedef struct{
  signed int AX,AY,AZ,TEMP,GX,GY,GZ;
}MPU6050_DataStruct;
extern MPU6050_DataStruct MPU6050_raw, MPU6050_filtered;

extern void MPU6050_init();
extern void MPU6050_getData();
extern void MPU6050_changeFormat();
extern void MPU6050_filter();
extern void MPU6050_calibrate();

#endif
#endif