#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "lcd.h"
#include "touch.h"


/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void NVIC_Configure(void);
void TIM_Configure(void);
void Delay(void);


uint16_t ADC1_CONVERTED_VALUE;

void RCC_Configure(void)
{
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
    
    /* Alternate Function IO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    /* ADC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    /* ADC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    /* LED */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
    /* PWM */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // ADC
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // PWM
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // LED 1, LED 2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
}


void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    /* Enable TIM2 Global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Configure(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 10000;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}
    
void TIM3_Configure(void) {
  
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    uint16_t prescale = (uint16_t) (SystemCoreClock / 1000000); // = 72
    
    TIM_TimeBaseStructure.TIM_Period = 20000;
    TIM_TimeBaseStructure.TIM_Prescaler = prescale;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 10000;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

int LED_ON = 0;
int LED_1_STATUS = 0;
int LED_2_STATUS = 0;
int LED_2_CNT = 0;
int motorflag = 0;

void TIM2_IRQHandler(void) {
    if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET) {
        if (!LED_ON) {
            GPIO_ResetBits(GPIOD, GPIO_Pin_2);
            GPIO_ResetBits(GPIOD, GPIO_Pin_3);
        }
        else {
            if (LED_1_STATUS) GPIO_ResetBits(GPIOD, GPIO_Pin_2);
            else GPIO_SetBits(GPIOD, GPIO_Pin_2);
            LED_1_STATUS = !LED_1_STATUS;
            if (LED_2_STATUS) {
                if (LED_2_CNT > 4) {
                    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
                    LED_2_CNT = 0;
                    LED_2_STATUS = !LED_2_STATUS;
                }
            }
            else {
                if (LED_2_CNT > 4) {
                    GPIO_SetBits(GPIOD, GPIO_Pin_3);
                    LED_2_CNT = 0;
                    LED_2_STATUS = !LED_2_STATUS;
                }
            }
            LED_2_CNT++;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void moveMotor(uint16_t var){
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = var;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
}

void Delay(void) {
    int i;
    for (i = 0; i < 2000000; i++) {}
}

void printTeamName(void) {
    char * str = "TEAM09\0";
    for (int i = 0; i < 6; i++) {
        LCD_ShowChar(0x50 + i * 10, 0x35, str[i], 16, BLACK, WHITE);
    }
}

void drawLEDStatus(int * LED_ON) {
    if (*LED_ON) LCD_ShowString(0x50, 0x55, "ON   ", RED, WHITE);
    else LCD_ShowString(0x50, 0x55, "OFF", RED, WHITE);
}

void drawLEDButton() {
    LCD_ShowString(0x60, 0x80, "BUT", RED, WHITE);
    LCD_DrawRectangle(0x50, 0x65, 0x80, 0x95);
}

int checkLCDButtonTouch(int x, int y) {
    if (x >= 0x50 && x <= 0x80) {
        if (y >= 0x65 && y <= 0x95) {
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    NVIC_Configure();
    TIM2_Configure();
    TIM3_Configure();
    
    LCD_Init();
    Touch_Configuration();
    Touch_Adjust();
    LCD_Clear(WHITE);
    
    uint16_t x;
    uint16_t y;
    uint16_t convert_x;
    uint16_t convert_y;
    uint16_t val[17] = {700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300};
    int idx = 0;
    
    while(1) {
      moveMotor(val[idx]);
      if (motorflag){  
        idx++;
        idx %= 17;}
      else{
        idx--;
        if (idx < 0) idx = 16;}
      
        printTeamName();
        drawLEDButton();
        drawLEDStatus(&LED_ON);
        Touch_GetXY(&x, &y, 0);
        Convert_Pos(x, y, &convert_x, &convert_y);
        if (T_INT != 1) {
            if (checkLCDButtonTouch(convert_x, convert_y)) {
                LED_ON = !LED_ON;
                motorflag = !motorflag;
            }
        }
        Delay();
    }
    return 0;
}