#include "stm32f10x.h"
#define RCC_APB2ENR_CLK (*(volatile unsigned int *) 0x40021018)
#define GPIOA_CRL (*(volatile unsigned int *) 0x40010800)
#define GPIOB_CRL (*(volatile unsigned int *) 0x40010C00)
#define GPIOC_CRL (*(volatile unsigned int *) 0x40011000)
#define GPIOC_CRH (*(volatile unsigned int *) 0x44011004)
#define GPIOD_CRL (*(volatile unsigned int *) 0x40011400)

#define GPIOA_IDR (*(volatile unsigned int *) 0x40010808)
#define GPIOB_IDR (*(volatile unsigned int *) 0x40010C08)
#define GPIOC_L_IDR (*(volatile unsigned int *) 0x40011008)
#define GPIOC_H_IDR (*(volatile unsigned int *) 0x4401140C)
#define GPIOD_IDR (*(volatile unsigned int *) 0x40011408)

#define GPIOD_BSRR (*(volatile unsigned int *) 0x40011410)

int main(void)
{
  
  // Bit 2, 3, 4, 5 Flip -> IOPAN, IOPBN, IOPCN, IOPDN
  RCC_APB2ENR_CLK = 0x3C;
    
  // Use KEY1 = PC4 | KEY2 = PB10 | KEY3 = PC13 | KEY4 = PA0
  GPIOA_CRL = 0x00000008;
  GPIOB_CRL = 0x00000800;
  GPIOC_CRL = 0x00080000;
  GPIOC_CRH = 0x00800000;
    
  // Use PD 2, 3, 4, 7
  GPIOD_CRL = 0x10011100;
  
  int flag1 = 0;
  int flag2 = 0;
  int flag3 = 0;
  int flag4 = 0;
  
  while(1){
    if ((GPIOC_L_IDR & (1 << 4) == 0)) { // KEY 1
      GPIOD_BSRR = (1 << 2);
    }
    if ((GPIOB_IDR & (1 << 3) == 0)) { // KEY 2
      GPIOD_BSRR = (1 << 3);
    }
    if ((GPIOC_H_IDR & (1 << 6) == 0)) { // KEY 3
      GPIOD_BSRR = (1 << 4);
    }
    if ((GPIOA_IDR & (0x1) == 0)) { // KEY 4
      GPIOD_BSRR = (1 << 7);
    }
  }
  return 0;
}