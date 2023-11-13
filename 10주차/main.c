#include "misc.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "core_cm3.h"
#include "stm32f10x_exti.h"
#include "lcd.h"
#include "touch.h"

int color[12] ={WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};

uint16_t value = 0; // 조도 센서 값을 저장하기 위한 전역변수
uint16_t x;
uint16_t y;
uint16_t convert_x;
uint16_t convert_y;

GPIO_InitTypeDef GPIO_init;                    // GPIO 초기화 변수 선언
USART_InitTypeDef USART_InitStructure;         // USART 초기화 변수 선언
NVIC_InitTypeDef NVIC_init;                    // 중첩 인터럽트 처리위한 NVIC 초기화 변수 선언    
 
void RCC_Configuration(void){    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}           
 
void GPIO_Configuration(void){
    // ADC_IN1 (PB0) Configure
    GPIO_init.GPIO_Pin = GPIO_Pin_0;  
    GPIO_init.GPIO_Mode = GPIO_Mode_AIN;    
    GPIO_Init(GPIOB, &GPIO_init);        
}
 
 
void NVIC_Configuration(void){      

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
    NVIC_Init(&NVIC_InitStructure);
 
    NVIC_EnableIRQ(ADC1_2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void ADC_Configuration(){
    ADC_DeInit(ADC1);
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
   
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);    
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    
    while(ADC_GetResetCalibrationStatus(ADC1));
    
    ADC_StartCalibration(ADC1);
    
    while(ADC_GetCalibrationStatus(ADC1));
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
 

void ADC1_2_IRQHandler(void) {
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
      value = ADC_GetConversionValue(ADC1);
      ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);   
   }    
}

static void LCD_Delay(uint32_t nCount)
{
   __IO uint16_t i;
   for (i = 0; i < (nCount * 5); i++)
   {
      ;
   }
}

int main(void){
    SystemInit();        
 
    RCC_Configuration();        
    GPIO_Configuration();        
    ADC_Configuration();
    NVIC_Configuration();        
    
    LCD_Init();
    Touch_Configuration();
    Touch_Adjust();
    LCD_Clear(WHITE);
    
    
    while(1)
    {

      LCD_ShowString(60, 60, "THU_Team09", BLACK, WHITE);
      
      Touch_GetXY(&x, &y, 1);
      Convert_Pos(x, y, &convert_x, &convert_y);

      LCD_ShowNum(60, 100, value, 4, BLACK, WHITE);
      LCD_ShowNum(60, 120, convert_x, 3, BLACK, WHITE);
      LCD_ShowNum(60, 140, convert_y, 3, BLACK, WHITE);
      
      LCD_DrawCircle(convert_x, convert_y, 4);

    }
    
    return 0;
    
}