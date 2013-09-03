#include"msp430f5438.h"
#include"stdint.h"
#include"TimerA1.h"
#include"IMU.h"
#define I2C2_RXBUF_SIZE 2
#define I2C2_TXBUF_SIZE 2
#define I2C2DelayTime 100

volatile unsigned char I2C2_RXBUF[I2C2_RXBUF_SIZE];
volatile unsigned char I2C2_TXBUF[I2C2_TXBUF_SIZE];
volatile unsigned char I2C2_RXBUF_LEN,I2C2_TXBUF_LEN;
volatile unsigned char I2C2_TXBUF_R,I2C2_TXBUF_W;
volatile unsigned char I2C2_TXFIN,I2C2_RXFIN,I2C2_RXLEN,I2C2_RXCNT;
volatile unsigned char* datadd2;
volatile unsigned char isReading2;

void I2C2_init()
{
    P3SEL &= ~BIT7;
    P5SEL &= ~BIT4;    
    P3DS |= BIT7;    
    P5DS |= BIT4;      
    P3DIR |= BIT7;    
    P5DIR |= BIT4;        
    P5OUT &= ~BIT4;
    P3OUT |= BIT7;
    // ���9��ʱ�� �Իָ�I2C2����״̬
    for( char i = 0 ; i < 9 ; i++ )
    {
      P5OUT |= BIT4;
      __delay_cycles(8000);
      P5OUT &= ~BIT4;
      __delay_cycles(8000);
    }  
  
    // Configure pins
    P3SEL |= BIT7;
    P5SEL |= BIT4;    
    
    UCB1CTL1 |= UCSWRST;	            //Software reset enabled
    UCB1CTL0 |= UCMODE_3  + UCMST + UCSYNC;//I2C2 mode, Master mode, sync
    UCB1CTL1 |= UCSSEL_2;                  //SMCLK
    UCB1BR0  =  40;                        //4M/100K=40 
    UCB1BR1  =  0;
    UCB1CTL0 &= ~UCSLA10;
    UCB1CTL1  &=~UCSWRST;
    
    UCB1IE    |= UCRXIE + UCTXIE;
}

void I2C2_reset(unsigned char slaveadd)
{
    UCB1CTL1 |= UCSWRST;	            //Software reset enabled
    UCB1CTL0 |= UCMODE_3  + UCMST + UCSYNC;//I2C2 mode, Master mode, sync
    UCB1CTL1 |= UCSSEL_2;                  //SMCLK
    UCB1BR0  =  10;                        //4M/400K=10 
    UCB1BR1  =  0;
    UCB1CTL0 &= ~UCSLA10;
    UCB1I2CSA = slaveadd;    
    UCB1CTL1  &=~UCSWRST;
    UCB1IE    |= UCRXIE + UCTXIE;
}

unsigned char I2C2_write(unsigned char slaveadd,unsigned char add,unsigned char data)
{
  static unsigned long I2C2PreTime;  
  I2C2PreTime = TimeBase;
  while( UCB1CTL1 & UCTXSTP )
  {
    if (I2C2PreTime + I2C2DelayTime < TimeBase)
    {
      I2C2_init();
      UCB1CTL1 &=~UCTXSTP;
      return 1;
    }
  }
  _DINT();
  if (UCB1I2CSA != slaveadd)
    I2C2_reset(slaveadd);
  UCB1CTL1 |= UCTR;                 // дģʽ
  UCB1CTL1 |= UCTXSTT;              // ��������λ  
  isReading2=0;
  I2C2_TXBUF_LEN=2;
  I2C2_TXBUF_R=0;
  I2C2_TXBUF[0]=add;
  I2C2_TXBUF[1]=data;
  UCB1IE  |= UCTXIE;
  I2C2_TXFIN=0;
  _EINT();
  while((!I2C2_TXFIN) || (UCB1STAT&UCBUSY));
  return 0;
}

unsigned char I2C2_read(unsigned char slaveadd,unsigned char regadd,unsigned char len,unsigned char *ramadd)
{
  static unsigned long I2C2PreTime;  
  I2C2PreTime = TimeBase;
  while( UCB1CTL1 & UCTXSTP )
  {
    if (I2C2PreTime + I2C2DelayTime < TimeBase)
    {
      I2C2_init();
      IMU_init();        
      return 1;
    }
  }
  _DINT();
  if (UCB1I2CSA != slaveadd)
    I2C2_reset(slaveadd);
  UCB1CTL1 |= UCTR;                 // дģʽ
  UCB1CTL1 |= UCTXSTT;              // ��������λ  
  isReading2=1;
  I2C2_TXBUF_LEN=1;
  I2C2_TXBUF_R=0;
  I2C2_TXBUF[0]=regadd;
  UCB1IE  |= UCTXIE;
  datadd2=ramadd;
  I2C2_RXFIN=0;
  I2C2_RXCNT=0;
  I2C2_RXLEN=len;
  _EINT();
  return 0;
}

// USCI_B1 Data ISR
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
{
  switch(UCB1IV)
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4:                                 // Vector  4: NACKIFG
    UCB1IFG &=  ~UCNACKIFG;
    _NOP();
    break;  
  case  6:                                 // Vector  6: STTIFG SLAVE MODE
    break;
  case  8: break;                           // Vector  8: STPIFG SLAVE MODE
  case 10:                                  // Vector 10: RXIFG    
/*    if (I2C2_RXBUF_LEN == I2C2_RXBUF_MAXLEN-1)
    {
      UCB1CTL1 |= UCTXSTP;              // �ڽ������һ���ֽ�֮ǰ����ֹͣλ
    }*/
    datadd2[I2C2_RXCNT++] = UCB1RXBUF;	//Get data from UCB1RXBUF
    if (I2C2_RXLEN-I2C2_RXCNT==1)
      UCB1CTL1 |= UCTXSTP;    
    if (I2C2_RXLEN-I2C2_RXCNT==0)
      I2C2_RXFIN=1;
    break;
  case 12:                                  // Vector 12: TXIFG
    if (I2C2_TXBUF_LEN > 0)
    {
        I2C2_TXBUF_LEN--;
        UCB1TXBUF = I2C2_TXBUF[I2C2_TXBUF_R++];	//Send data
        if (I2C2_TXBUF_R == I2C2_TXBUF_SIZE)
            I2C2_TXBUF_R = 0;
    }
    else 
    {
      if (!isReading2)
      {
        UCB1CTL1 |= UCTXSTP;
        I2C2_TXFIN=1;
      }
      else
      {
        UCB1CTL1 &= ~UCTR;                // ��ģʽ        
        UCB1CTL1 |= UCTXSTT;        
        while(UCB1CTL1 & UCTXSTT);
        if (I2C2_RXLEN==1)
          UCB1CTL1 |= UCTXSTP;
      }
      UCB1IE &= ~UCTXIE;				//Turn off transmit
    }
    break;
  default: break; 
  }
}

/*
void I2C2_init()
{
  P3SEL &= ~BIT2;                         // P3.2@UCB1SCL
  P3DIR |= BIT2;
  P3OUT |= BIT2;
  // ���9��ʱ�� �Իָ�I2C2����״̬
  for( uint8_t i = 0 ; i < 9 ; i++ )
  {
    P3OUT |= BIT2;
    __delay_cycles(8000);
    P3OUT &= ~BIT2;
    __delay_cycles(8000);
  }
  
  P3SEL |= (BIT1 + BIT2);                 // P3.1@UCB1SDA P3.2@UCB1SCL
  // P3.1@ISP.1 P3.2@ISP.5
  
  UCB1CTL1 |= UCSWRST;
  UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC ;  // I2C2����ģʽ
  UCB1CTL1 |= UCSSEL_2;                   // ѡ��SMCLK
  UCB1BR0 = 40;
  UCB1BR1 = 0;
  UCB1CTL0 &= ~UCSLA10;                   // 7λ��ַģʽ
  UCB1I2C2SA = 0x1e;            // ADXL345
  UCB1CTL1 &= ~UCSWRST;
}

uint8_t eeprom_readbyte( uint8_t word_addr , uint8_t *pword_value )
{
  UCB1CTL1 |= UCTR;                 // дģʽ
  UCB1CTL1 |= UCTXSTT;              // ��������λ��д�����ֽ�

  UCB1TXBUF = word_addr;            // �����ֽڵ�ַ������Ҫ�����TXBUF
  // �ȴ�UCTXIFG=1 ��UCTXSTT=0 ͬʱ�仯 �ȴ�һ����־λ����
  while(!(UCB1IFG & UCTXIFG))
  {
    if( UCB1IFG & UCNACKIFG )       // ����Ӧ�� UCNACKIFG=1
    {
      return 1;
    }
  }                        

  UCB1CTL1 &= ~UCTR;                // ��ģʽ
  UCB1CTL1 |= UCTXSTT;              // ��������λ�Ͷ������ֽ�

  while(UCB1CTL1 & UCTXSTT);        // �ȴ�UCTXSTT=0
  // ����Ӧ�� UCNACKIFG = 1
  UCB1CTL1 |= UCTXSTP;              // �ȷ���ֹͣλ

  while(!(UCB1IFG & UCRXIFG));      // ��ȡ�ֽ�����
  *pword_value = UCB1RXBUF;         // ��ȡBUF�Ĵ����ڷ���ֹͣλ֮��

  while( UCB1CTL1 & UCTXSTP );
  
  return 0; 
}

uint8_t eeprom_writebyte( uint8_t word_addr , uint8_t word_value )
{
  while( UCB1CTL1 & UCTXSTP );
  UCB1CTL1 |= UCTR;                 // дģʽ
  UCB1CTL1 |= UCTXSTT;              // ��������λ

  UCB1TXBUF = word_addr;            // �����ֽڵ�ַ
  // �ȴ�UCTXIFG=1 ��UCTXSTT=0 ͬʱ�仯 �ȴ�һ����־λ����
  while(!(UCB1IFG & UCTXIFG))
  {
    if( UCB1IFG & UCNACKIFG )       // ����Ӧ�� UCNACKIFG=1
    {
      return 1;
    }
  }   

  UCB1TXBUF = word_value;           // �����ֽ�����
  while(!(UCB1IFG & UCTXIFG));      // �ȴ�UCTXIFG=1

  UCB1CTL1 |= UCTXSTP;
  while(UCB1CTL1 & UCTXSTP);        // �ȴ��������
  
  return 0;
}

void I2C2_write(unsigned char add,unsigned char data)
{
  eeprom_writebyte(add,data);
}


uint8_t I2C2_read(unsigned char add)
{
  uint8_t temp;
  eeprom_readbyte(add,&temp);
  return temp;
}
*/