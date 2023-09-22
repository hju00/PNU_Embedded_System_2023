#include "stm32f10x.h"

#define RCC_APB2ENR_CLK *((volatile unsigned int *) 0x40021018)
#define GPIOA_CRL *((volatile unsigned int *) 0x40010800) // PA0
#define GPIOB_CRH *((volatile unsigned int *) 0x40010C04) // PB10
#define GPIOC_CRL *((volatile unsigned int *) 0x40011000) // PC4
#define GPIOC_CRH *((volatile unsigned int *) 0x40011004) // PC14
#define GPIOD_CRL *((volatile unsigned int *) 0x40011400) // LED 모드 설정 address

// 버튼 눌렸는지 확인용 주소 (PA0, PB10, PC4, PC14)
#define GPIOA_IDR *((volatile unsigned int *) 0x40010808)
#define GPIOB_IDR *((volatile unsigned int *) 0x40010C08)
#define GPIOC_IDR *((volatile unsigned int *) 0x40011008)
// #define GPIOC_H_IDR *((volatile unsigned int *) 0x4401140C)
#define GPIOD_IDR *((volatile unsigned int *) 0x40011408)

#define GPIOB_BRR *(volatile unsigned int *)0x40010C14 // Port B의 base 주소 + BRR offset (사용안함)

#define GPIOD_BSRR *(volatile unsigned int *)0x40011410 // Port D의 base 주소 + BSRR offset (PD2,3,4,7LED 불켜는용 주소)
#define GPIOD_BRR *((volatile unsigned int *) 0x40011414) // LED reset용 주소

int main(void)
{
 
  RCC_APB2ENR_CLK |= 0x3C; // IOPAEN, IOPBEN, IOPDEN 각 bit에 1을 할당

  // register address reset
  GPIOA_CRL &= ~0x0000000F;
  GPIOB_CRH &= ~0x00000F00;
  GPIOC_CRL &= ~0x000F0000;
  GPIOC_CRH &= ~0x00F00000;

  // Bit 2, 3, 4, 5 Flip -> IOPAN, IOPBN, IOPCN, IOPDN
   
  // Use KEY1 = PC4 | KEY2 = PB10 | KEY3 = PC13 | KEY4 = PA0
  GPIOA_CRL |= 0x00000008;
  GPIOB_CRH |= 0x00000800;
  GPIOC_CRL |= 0x00080000;
  GPIOC_CRH |= 0x00800000;
   
  // Use PD 2, 3, 4, 7
 
  GPIOD_CRL &= ~0xF00FFF00;
  GPIOD_CRL |= 0x10011100; // LED 연결 

  GPIOD_BSRR |= 0x9C; // initial LED state -> off
 
  // LED의 상태 확인용 변수
  int flag1 = 0;
  int flag2 = 0;
  int flag3 = 0;
  int flag4 = 0;

 while (1) // 계속 반복
  {
    if (~GPIOC_IDR & 0x00000010) { // Key1의 버튼이 눌렸는지 확인. (IDR은 눌렸으면 0,  뗐으면 1)
      if (flag1 == 0) {
        GPIOD_BRR |= (1<<2); // LED 켜기
        flag1 = 1;
      }
      else if (flag1 == 1) {
        GPIOD_BSRR |= (1<<2); // LED 끄기
        flag1 = 0;
      }
    }
    if (~GPIOB_IDR & 0x400) {
      if (flag2 == 0) {
        GPIOD_BRR |= (1 << 3);
        flag2 = 1;
      }
      else if (flag2 == 1) {
        GPIOD_BSRR |= (1 << 3);
        flag2 = 0;
      }
    }
    if (~GPIOC_IDR & 0x2000) {
      if (flag2 == 0) {
        GPIOD_BRR |= (1 << 4);
        flag2 = 1;
      }
      else if (flag2 == 1) {
        GPIOD_BSRR |= (1 << 4);
        flag2 = 0;
      }
    }
    if (~GPIOA_IDR & 0x1) {
      if (flag2 == 0) {
        GPIOD_BRR |= (1 << 7);
        flag2 = 1;
      }
      else if (flag2 == 1) {
        GPIOD_BSRR |= (1 << 7);
        flag2 = 0;
      }
      
    }
  }
}
