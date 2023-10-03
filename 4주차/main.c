#include "stm32f10x.h"
#define RCC_APB2ENR_CLK *((volatile unsigned int *)0x40021018)
#define GPIOB_CRH *((volatile unsigned int *)0x40010C04) // PB10
#define GPIOC_CRL *((volatile unsigned int *)0x40011000) // PC4
#define GPIOC_CRH *((volatile unsigned int *)0x40011004) // PC13
#define GPIOD_CRL *((volatile unsigned int *)0x40011400) // LED 주소

// GPIO 포트 설정 (PB10, PC4, PC14)
#define GPIOB_IDR *((volatile unsigned int *)0x40010C08)
#define GPIOC_IDR *((volatile unsigned int *)0x40011008)
#define GPIOC_H_IDR *((volatile unsigned int *)0x4401140C)
#define GPIOD_IDR *((volatile unsigned int *)0x40011408)

#define GPIOD_BSRR *((volatile unsigned int *)0x40011410) // 포트 D 기본 주소 + BSRR 오프셋 (PD2, 3, 4, 7 LED)
#define GPIOD_BRR *((volatile unsigned int *)0x40011414) // LED 리셋 (PD4)

void delay() {
    for (int i = 0; i < 10000000; i++) {}
}

int main(void) {

   RCC_APB2ENR_CLK |= 0x38; // IOPBEN, IOPCN, IOPDEN 비트 1 활성화
   // 레지스터 주소 리셋
   GPIOB_CRH &= ~0x00000F00;
   GPIOC_CRL &= ~0x000F0000;
   GPIOC_CRH &= ~0x00F00000;
   // KEY2 = PB10 | KEY3 = PC13 사용
   GPIOB_CRH |= 0x00000800;
   GPIOC_CRH |= 0x00800000;
   // PD 2, 3 사용
   // 레지스터 주소 리셋
   GPIOD_CRL &= ~0x0000FF00;
   // LED(2, 3) 설정
   GPIOD_CRL |= 0x00001100;
   // 초기 LED 상태 -> 꺼짐
   GPIOD_BSRR |= 0x0C;


   while (1) {
      if (~GPIOC_IDR & 0x10) {
         GPIOD_BRR |= (1 << 2);
         GPIOD_BRR |= (1 << 3);
         delay();
      }
      if (~GPIOB_IDR & 0x400) {
         GPIOD_BRR |= (1 << 2);
         delay();
      }
      if (~GPIOC_IDR & 0x2000) {
         GPIOD_BRR |= (1 << 3);
         delay();
      }
      
      GPIOD_BSRR |= (1 << 2); // LED 켜기 (설정)
      GPIOD_BSRR |= (1 << 3);
      delay();
   }
}
