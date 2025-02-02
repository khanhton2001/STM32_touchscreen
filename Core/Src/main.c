/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "touch.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define INIT 0
#define DRAW 1
#define CLEAR 2
#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define MAX_FOOD 2
#define SNAKE_SIZE 100
#define CELL_SIZE 10

int draw_Status = INIT;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void touchProcess();
uint8_t isButtonClear();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
extern int x  = 20 ;
extern int y = 200 ;
extern int snake_direction = 0  ;
int food_count = 0 ;

typedef struct {
	int xf ;
	int yf ;
} Point ;

typedef struct {
    Point position;
    Point velocity;
    Point snake[SNAKE_SIZE];
    int length;
} Snake;

typedef struct {
    Point position;
    int active;
} Food;

Snake snake;
Food food[MAX_FOOD];


int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 touch_Adjust();
 lcd_Clear(BLACK);
 while (1)
  {
	 if((x == 0)||(y == 0)||(x == 240)||(y == 320)){

			lcd_Fill(60, 10, 180, 60, GBLUE);
			lcd_ShowStr(60, 20, "GAME OVER", RED, BLACK, 24, 1);
	 }

	 if((x ==  food[0].position.xf)&&(y ==  food[0].position.yf)){
		 lcd_ShowStr( food[0].position.xf,  food[0].position.yf, "+", BLACK, BLACK, 24, 1);
     	 food[0].position.xf = rand() % (LCD_WIDTH / CELL_SIZE) * CELL_SIZE;
		 food[0].position.yf = rand() % (LCD_HEIGHT / CELL_SIZE) * CELL_SIZE;
		 lcd_ShowStr( food[0].position.xf,  food[0].position.yf, "+", YELLOW, BLACK, 24, 1);
	 }

	 if((x ==  food[1].position.xf)&&(y ==  food[1].position.yf)){

	 		 lcd_ShowStr( food[1].position.xf,  food[1].position.yf, "+", BLACK, BLACK, 24, 1);
	 		 food[1].position.xf = rand() % (LCD_WIDTH / CELL_SIZE) * CELL_SIZE;
	 		 food[1].position.yf = rand() % (LCD_HEIGHT / CELL_SIZE) * CELL_SIZE;
	 		 lcd_ShowStr( food[1].position.xf,  food[1].position.yf, "+", YELLOW, BLACK, 24, 1);
	 	 }

	 if(food_count == 0 ){

		 food_count++ ;
         for (int i = 0; i < MAX_FOOD; i++) {
	  			        food[i].position.xf = rand() % (LCD_WIDTH / CELL_SIZE) * CELL_SIZE;
	  			        food[i].position.yf = rand() % (LCD_HEIGHT / CELL_SIZE) * CELL_SIZE;
	  			        food[i].active = 1;
	  			      lcd_ShowStr(food[i].position.xf, food[i].position.yf, "+", YELLOW, BLACK, 24, 1);
	  		 }
	 }

	  //scan touch screen
	  touch_Scan();
	  updateSnake() ;
	  //check if touch screen is touched
	  if(touch_IsTouched() && draw_Status == DRAW){
            //draw a point at the touch position
		  lcd_DrawPoint(touch_GetX(), touch_GetY(), RED);
	  }
	  // 50ms task
	  if(flag_timer2 == 1){
		  flag_timer2 = 0;
		  touchProcess();
		  test_LedDebug();
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  timer_init();
	  button_init();
	  lcd_init();
	  touch_init();
	  setTimer2(50);






}


void displayVerticalText(uint16_t x, uint16_t y, char *text, uint16_t fc, uint16_t bc, uint8_t sizey) {
    int charHeight = 24 * sizey; // Assuming character height

    // Display each character vertically, line by line
    for (int i = 0; i < strlen(text); i++) {
        lcd_ShowStr(x, y + i * charHeight, text[i], fc, bc, 24, 1);
    }
}

uint8_t count_led_debug = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

uint8_t isButtonClear(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 60 && touch_GetX() < 180 && touch_GetY() > 10 && touch_GetY() < 60;
}

uint8_t isButtonUp(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 90 && touch_GetX() < 120 && touch_GetY() > 90 && touch_GetY() < 120;
}

uint8_t isButtonDown(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 90 && touch_GetX() < 120 && touch_GetY() > 150 && touch_GetY() < 180;
}

uint8_t isButtonLeft(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 40 && touch_GetX() < 70 && touch_GetY() > 120 && touch_GetY() < 150;
}

uint8_t isButtonRight(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 140 && touch_GetX() < 170 && touch_GetY() > 120 && touch_GetY() < 150;
}

//void lcd_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color) //add a hcn = 1 mau car been trogn
//void lcd_ShowStr(uint16_t x, uint16_t y,char *str,uint16_t fc, uint16_t bc,uint8_t sizey,uint8_t mode)
void touchProcess(){
	switch (draw_Status) {
		case INIT:
                // display blue button
			lcd_Fill(90, 90, 120, 120, GBLUE); //up
			lcd_Fill(40, 120, 70, 150, GBLUE); //left
			lcd_Fill(140, 120, 170, 150, GBLUE); //right
			lcd_Fill(90, 150, 120, 180, GBLUE); //down



			lcd_Fill(60, 10, 180, 60, GBLUE);
			lcd_ShowStr(90, 20, "START", RED, BLACK, 24, 1);
			lcd_ShowStr(x, y, "000", RED, GREEN, 24, 1);


			draw_Status = DRAW;

			break;
		case DRAW:
			if(isButtonClear()){
				draw_Status = CLEAR;
				 setTimersnake(500);
                    // clear board
			//	lcd_Fill(0, 60, 240, 320, BLACK);
                    // display green button
				lcd_Fill(60, 10, 180, 60, GREEN);
				lcd_ShowStr(90, 20, "CLEAR", RED, BLACK, 24, 1);
			}

			if(isButtonUp()){

			                    // display green button
				draw_Status = CLEAR;
				snake_direction = 3 ;
				lcd_Fill(90, 90, 120, 120, GREEN);

						}

			if(isButtonDown()){
						draw_Status = CLEAR;
			            // clear board
                        snake_direction = 4 ;
					    lcd_Fill(90, 150, 120, 180, GREEN);}


			if(isButtonLeft()){
		    			draw_Status = CLEAR;
						snake_direction = 1  ;
						 // clear
					    lcd_Fill(40, 120, 70, 150, GREEN);}

			if(isButtonRight()){
						draw_Status = CLEAR;
						snake_direction = 2 ;
						// clear board
                        lcd_Fill(140, 120, 170, 150, GREEN);}
          	break;


		case CLEAR:


			if(!touch_IsTouched()) draw_Status = INIT;
			break;


		default:
			break;
	}
}
/* USER CODE END 4 */
void renderSnake(){;}
void updateSnake(){
	if((flag_timersnake == 1)&&(snake_direction == 0 )){
		    lcd_ShowStr(x, y, "000", BLACK, GREEN, 24, 1);
		    x = x + 5 ;
		    flag_timersnake = 0 ;
		    lcd_ShowStr(x, y, "000", RED, GREEN, 24, 1);
	}

	if((flag_timersnake == 1)&&(snake_direction == 1 )){
		    lcd_ShowStr(x, y+5, "0", BLACK, GREEN, 24, 1);
		    lcd_ShowStr(x, y+10, "0", BLACK, GREEN, 24, 1);
	        lcd_ShowStr(x, y-5, "0", BLACK, GREEN, 24, 1);
		    lcd_ShowStr(x, y-10, "0", BLACK, GREEN, 24, 1);
		    lcd_ShowStr(x, y, "000", BLACK, GREEN, 24, 1);
			x = x - 5 ;
			flag_timersnake = 0 ;
			lcd_ShowStr(x, y, "000", RED, GREEN, 24, 1);
		}

	if((flag_timersnake == 1)&&(snake_direction == 2 )){
	   	    lcd_ShowStr(x, y-5, "0", BLACK, GREEN, 24, 1);
	     	lcd_ShowStr(x, y-10, "0", BLACK, GREEN, 24, 1);
	    	lcd_ShowStr(x, y+5, "0", BLACK, GREEN, 24, 1);
		    lcd_ShowStr(x, y+10, "0", BLACK, GREEN, 24, 1);
			lcd_ShowStr(x, y, "000", BLACK, GREEN, 24, 1);
			x = x + 5 ;
			flag_timersnake = 0 ;
			lcd_ShowStr(x, y, "000", RED, GREEN, 24, 1);
			}

	if((flag_timersnake == 1)&&(snake_direction == 3 )){
				lcd_ShowStr(x, y, "000", BLACK, GREEN, 24, 1);
				lcd_ShowStr(x, y+5, "0", BLACK, GREEN, 24, 1);
				lcd_ShowStr(x, y+10, "0", BLACK, GREEN, 24, 1);

			    lcd_ShowStr(x, y, "0", RED, GREEN, 24, 1);
				y = y - 5 ;
				lcd_ShowStr(x, y, "0", RED, GREEN, 24, 1);
				y = y - 5 ;
				lcd_ShowStr(x, y, "0", RED, GREEN, 24, 1);
			//	displayVerticalText(x, y, "000", RED, GREEN, 1);
				flag_timersnake = 0 ;

			}

	if((flag_timersnake == 1)&&(snake_direction == 4 )){
					lcd_ShowStr(x, y, "000", BLACK, GREEN, 24, 1);
					lcd_ShowStr(x, y-5, "0", BLACK, GREEN, 24, 1);
					lcd_ShowStr(x, y-10, "0", BLACK, GREEN, 24, 1);

				    lcd_ShowStr(x, y, "0", RED, GREEN, 24, 1);
					y = y + 5 ;
					lcd_ShowStr(x, y, "0", RED, GREEN, 24, 1);
					y = y + 5 ;
					lcd_ShowStr(x, y, "0", RED, GREEN, 24, 1);
				//	displayVerticalText(x, y, "000", RED, GREEN, 1);
					flag_timersnake = 0 ;

				}
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
