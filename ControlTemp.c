#include "main.h"

// Define port
#define A_SEG_PORT	GPIOB
#define B_SEG_PORT	GPIOB
#define C_SEG_PORT	GPIOA
#define D_SEG_PORT	GPIOA
#define E_SEG_PORT	GPIOA
#define F_SEG_PORT	GPIOA
#define G_SEG_PORT	GPIOA
#define DIG2_PORT		GPIOA
#define DIG1_PORT		GPIOA

#define COND1_PORT	GPIOB
#define COND2_PORT	GPIOB
#define COND3_PORT	GPIOB

// Define pin
#define A_SEG_PIN	GPIO_PIN_1
#define B_SEG_PIN	GPIO_PIN_0
#define C_SEG_PIN	GPIO_PIN_4
#define D_SEG_PIN	GPIO_PIN_2
#define E_SEG_PIN	GPIO_PIN_3
#define F_SEG_PIN	GPIO_PIN_5
#define G_SEG_PIN	GPIO_PIN_1
#define DIG2_PIN	GPIO_PIN_7
#define DIG1_PIN	GPIO_PIN_6

#define COND1_PIN	GPIO_PIN_12
#define COND2_PIN	GPIO_PIN_10
#define COND3_PIN	GPIO_PIN_11

uint8_t Segment_Cathode_Code[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
uint16_t value_adc_sensor = 0;
float value_sensor = 0;

void displayOneNum(uint8_t num, uint8_t dig)
{
	HAL_GPIO_WritePin(A_SEG_PORT, A_SEG_PIN, (Segment_Cathode_Code[num] & 0x01) >> 0);
	HAL_GPIO_WritePin(B_SEG_PORT, B_SEG_PIN, (Segment_Cathode_Code[num] & 0x02) >> 1);
	HAL_GPIO_WritePin(C_SEG_PORT, C_SEG_PIN, (Segment_Cathode_Code[num] & 0x04) >> 2);
	HAL_GPIO_WritePin(D_SEG_PORT, D_SEG_PIN, (Segment_Cathode_Code[num] & 0x08) >> 3);
	HAL_GPIO_WritePin(E_SEG_PORT, E_SEG_PIN, (Segment_Cathode_Code[num] & 0x10) >> 4);
	HAL_GPIO_WritePin(F_SEG_PORT, F_SEG_PIN, (Segment_Cathode_Code[num] & 0x20) >> 5);
	HAL_GPIO_WritePin(G_SEG_PORT, G_SEG_PIN, (Segment_Cathode_Code[num] & 0x40) >> 6);
	
	switch (dig)
	{
		case 0:
			HAL_GPIO_WritePin(DIG1_PORT, DIG1_PIN, 1);
			HAL_GPIO_WritePin(DIG2_PORT, DIG2_PIN, 0);
			break;
		case 1:
			HAL_GPIO_WritePin(DIG1_PORT, DIG1_PIN, 0);
			HAL_GPIO_WritePin(DIG2_PORT, DIG2_PIN, 1);
			break;
	}
}

void Display(uint8_t num)
{	
	displayOneNum(num / 10, 1);
	HAL_Delay(1);
	displayOneNum(10, 1);
	displayOneNum(num % 10, 0);
	HAL_Delay(1);
	displayOneNum(10, 0);
}


// Config the TIMER interrupt first
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM1)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		value_adc_sensor = HAL_ADC_GetValue(&hadc1);
		value_sensor = (float)value_adc_sensor * 220.0 / 4096.0;
		HAL_ADC_Stop(&hadc1);
		
		
		if(value_sensor <36.5)
		{
			HAL_GPIO_WritePin(COND1_PORT, COND1_PIN, 0);
		}
		else if((value_sensor  >= 36.5) && (value_sensor < 37))
		{
			HAL_GPIO_WritePin(COND1_PORT, COND1_PIN, 0);
			HAL_GPIO_WritePin(COND2_PORT, COND2_PIN, 0);
		}
		else if(value_sensor >= 37)
		{
			HAL_GPIO_WritePin(COND1_PORT, COND1_PIN, 0);
			HAL_GPIO_WritePin(COND2_PORT, COND2_PIN, 0);
			HAL_GPIO_WritePin(COND3_PORT, COND3_PIN, 0);
		}
	}
}

int main(void)
{
    HAL_TIM_Base_Start_IT(&htim1);
	HAL_GPIO_WritePin(COND1_PORT, COND1_PIN, 1);
	HAL_GPIO_WritePin(COND2_PORT, COND2_PIN, 1);
	HAL_GPIO_WritePin(COND2_PORT, COND3_PIN, 1);

    while(1)
    {
        Display((uint8_t) value_sensor);
    }
}