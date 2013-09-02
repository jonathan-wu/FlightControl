#include "msp430x54x.h"
#include "PWM.h"
//��ʼ��PWM
//P1.2|--> PWM_1
//P1.3|--> PWM_2
//P1.4|--> PWM_3
//P1.5|--> PWM_4
//Ĭ��ռ�ձ�5%
//SMCLK=XT2/4=4MHz,4��Ƶ��1MHz,Ƶ��200Hz
void PWM_init(void)
{
	P1DIR |= BIT2+BIT3+BIT4+BIT5;           // P2.2 and P2.3 output
	P1SEL |= BIT2+BIT3+BIT4+BIT5;			// P1.2 and P2.3 options select
	TA0CCR0 = 5000-1;						// PWM Period
	TA0CCTL1 = OUTMOD_7;                    // CCR1 reset/set
	TA0CCTL2 = OUTMOD_7;                    // CCR2 reset/set
	TA0CCTL3 = OUTMOD_7;                    // CCR3 reset/set
	TA0CCTL4 = OUTMOD_7;                    // CCR4 reset/set
	TA0CTL = TASSEL_2 + ID_2 + MC_1 + TACLR;	// SMCLK, up mode, clear TAR
	PWM_1(0);
	PWM_2(0);
	PWM_3(0);
	PWM_4(0);
}
//����ռ�ձ�
//����  n Ϊ0~1000��������ռ�ձ�Ϊ  n/10 %
void PWM_1(unsigned int n)
{
  if(n>1000) n=1000;
//  else if(n<0) n=0;
  TA0CCR1 = n*2/5+1100;                            // CCR1 PWM duty cycle
}
void PWM_2(unsigned int n)
{
  if(n>1000) n=1000;
//  else if(n<0) n=0;
  TA0CCR2 = n*2/5+1100;                            // CCR1 PWM duty cycle
}
void PWM_3(unsigned int n)
{
  if(n>1000) n=1000;
//  else if(n<0) n=0;
  TA0CCR3 = n*2/5+1100;                            // CCR1 PWM duty cycle
}
void PWM_4(unsigned int n)
{
  if(n>1000) n=1000;
//  else if(n<0) n=0;
  TA0CCR4 = n*2/5+1100;                            // CCR1 PWM duty cycle
}