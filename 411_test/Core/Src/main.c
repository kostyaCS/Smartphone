/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "i2s.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd5110.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define ROW_1_Pin GPIO_PIN_4
#define ROW_2_Pin GPIO_PIN_5
#define ROW_3_Pin GPIO_PIN_7
#define ROW_4_Pin GPIO_PIN_8
#define COL_1_Pin GPIO_PIN_0
#define COL_2_Pin GPIO_PIN_1
#define COL_3_Pin GPIO_PIN_2
#define COL_4_Pin GPIO_PIN_3
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
GPIO_TypeDef* ROW_1_Port = GPIOB;
GPIO_TypeDef* ROW_2_Port = GPIOB;
GPIO_TypeDef* ROW_3_Port = GPIOB;
GPIO_TypeDef* ROW_4_Port = GPIOB;
GPIO_TypeDef* COL_1_Port = GPIOD;
GPIO_TypeDef* COL_2_Port = GPIOD;
GPIO_TypeDef* COL_3_Port = GPIOD;
GPIO_TypeDef* COL_4_Port = GPIOD;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
LCD5110_display lcd1;
void keypad_init(void)
{
  // Configure GPIO pins for keypad matrix
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = ROW_1_Pin | ROW_2_Pin | ROW_3_Pin | ROW_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ROW_1_Port, &GPIO_InitStruct);
  HAL_GPIO_Init(ROW_2_Port, &GPIO_InitStruct);
  HAL_GPIO_Init(ROW_3_Port, &GPIO_InitStruct);
  HAL_GPIO_Init(ROW_4_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = COL_1_Pin | COL_2_Pin | COL_3_Pin | COL_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(COL_1_Port, &GPIO_InitStruct);
  HAL_GPIO_Init(COL_2_Port, &GPIO_InitStruct);
  HAL_GPIO_Init(COL_3_Port, &GPIO_InitStruct);
  HAL_GPIO_Init(COL_4_Port, &GPIO_InitStruct);
}

char keypad_scan(void)
{
  char keys[4][4] = {{'1', '2', '3', 'A'},
                     {'4', '5', '6', 'B'},
                     {'7', '8', '9', 'C'},
                     {'*', '0', '#', 'D'}};

  for(int i = 0; i < 4; i++)
  {
    // Set current column as output and low
    switch(i)
    {
      case 0:
        HAL_GPIO_WritePin(COL_1_Port, COL_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(COL_2_Port, COL_2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(COL_3_Port, COL_3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(COL_4_Port, COL_4_Pin, GPIO_PIN_SET);
        break;

      case 1:
        HAL_GPIO_WritePin(COL_1_Port, COL_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(COL_2_Port, COL_2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(COL_3_Port, COL_3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COL_4_Port, COL_4_Pin, GPIO_PIN_SET);
    break;

  case 2:
    HAL_GPIO_WritePin(COL_1_Port, COL_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COL_2_Port, COL_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COL_3_Port, COL_3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(COL_4_Port, COL_4_Pin, GPIO_PIN_SET);
    break;

  case 3:
    HAL_GPIO_WritePin(COL_1_Port, COL_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COL_2_Port, COL_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COL_3_Port, COL_3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(COL_4_Port, COL_4_Pin, GPIO_PIN_RESET);
    break;
}
// Read current rows
if(HAL_GPIO_ReadPin(ROW_1_Port, ROW_1_Pin) == GPIO_PIN_RESET)
  return keys[0][i];
if(HAL_GPIO_ReadPin(ROW_2_Port, ROW_2_Pin) == GPIO_PIN_RESET)
  return keys[1][i];
if(HAL_GPIO_ReadPin(ROW_3_Port, ROW_3_Pin) == GPIO_PIN_RESET)
  return keys[2][i];
if(HAL_GPIO_ReadPin(ROW_4_Port, ROW_4_Pin) == GPIO_PIN_RESET)
  return keys[3][i];
}
return 0; // No key pressed
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
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

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2S2_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  	lcd1.hw_conf.spi_handle = &hspi1;
    lcd1.hw_conf.spi_cs_pin =  GPIO_PIN_2;
    lcd1.hw_conf.spi_cs_port = GPIOA;
    lcd1.hw_conf.rst_pin =  GPIO_PIN_0;
    lcd1.hw_conf.rst_port = GPIOB;
    lcd1.hw_conf.dc_pin =  GPIO_PIN_3;
    lcd1.hw_conf.dc_port = GPIOA;
    lcd1.def_scr = lcd5110_def_scr;
    LCD5110_init(&lcd1.hw_conf, LCD5110_NORMAL_MODE, 0x40, 2, 3);

    char buf[10] = {0};
    int pos = 0;

    char key_pressed = 0;

    rect_t rect={0,0, 83, 10};

    const unsigned char call_image[] = {
    	0xbf, 0x0f, 0x07, 0x03, 0x03, 0x01, 0x00, 0x01, 0x03, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbd,
    	0x18, 0x18, 0x39, 0x39, 0x31, 0x73, 0xe3, 0xc7, 0x87, 0x0f, 0x1f, 0x7f, 0xff, 0xff, 0xff, 0xfc,
    	0xe0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x7e, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xf3, 0xe3,
    	0x87, 0x8e, 0xfe, 0xf8, 0xc0, 0x81, 0xcf, 0xff, 0xfc, 0xc0, 0x81, 0xcf, 0xff, 0xff, 0xff, 0xff,
    	0xff, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x01, 0x03, 0x07, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x0f,
    	0x0f, 0x07, 0x0f, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x3f, 0x7f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3e, 0x3e, 0x3c, 0x38, 0x38, 0x30, 0x30, 0x30, 0x20, 0x20,
    	0x20, 0x00, 0x20, 0x20, 0x30, 0x38, 0x3e, 0x3f
    };

    bool is_call;

    void init_screen() {
    	keypad_init();
		LCD5110_clear_scr(&lcd1);
		LCD5110_refresh(&lcd1);
		LCD5110_rect(&rect, 1, &lcd1);
		LCD5110_rect_fill(&rect, 1, &lcd1);
		LCD5110_set_cursor(10, 15, &lcd1);

		LCD5110_print("A-call", 0, &lcd1);

		LCD5110_set_cursor(10, 25, &lcd1);

		LCD5110_print("D-delete", 0, &lcd1);

		LCD5110_set_cursor(10, 35, &lcd1);

		LCD5110_print("C-clear last", 0, &lcd1);
		LCD5110_set_cursor(0, 0, &lcd1);
		is_call = false;
    }

    void call_screen() {
    	LCD5110_clear_scr(&lcd1);
    	LCD5110_refresh(&lcd1);
    	LCD5110_drawBitmap(25, 1, call_image, 30, 30, 0, &lcd1.hw_conf);
    	LCD5110_set_cursor(10, 40, &lcd1);
    	LCD5110_print("A-end call", 0, &lcd1);
    	is_call = true;
    }
    init_screen();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
	    key_pressed = keypad_scan();

	    if(key_pressed != 0)
	    {

	    	if (key_pressed == 'A') {
	    		if (is_call) {
	    			buf[0] = '\0';
					pos = 0;
					LCD5110_refresh(&lcd1);
					LCD5110_rect_fill(&rect, 1, &lcd1);
					LCD5110_set_cursor(0, 0, &lcd1);
	    			init_screen();
	    		}
	    		else {
	    			call_screen();
	    		}
	    	}
	    	else if (key_pressed == 'D')
	        {
	            memset(buf, ' ', sizeof(buf) - 1);
	            buf[0] = '\0';
	            pos = 0;
	            LCD5110_refresh(&lcd1);
	            LCD5110_rect_fill(&rect, 1, &lcd1);
	            LCD5110_set_cursor(0, 0, &lcd1);
	        }
	        else if (key_pressed == 'C' && pos > 0)
	                {
	        			pos--;
						buf[pos] = '\0';

						LCD5110_refresh(&lcd1);
						LCD5110_rect_fill(&rect, 1, &lcd1);
						LCD5110_set_cursor(0, 0, &lcd1);
						LCD5110_print(buf, 0, &lcd1);
	                }
	        else if (key_pressed >= '0' && key_pressed <= '9' && pos < sizeof(buf))
	        {
	            buf[pos] = key_pressed;
	            pos++;
	            buf[pos] = '\0';

	            LCD5110_set_cursor(0, 0, &lcd1);
	            LCD5110_print(buf, 0, &lcd1);
	        }
	    }
	    LCD5110_refresh(&lcd1);
	    HAL_Delay(1000);
	    LCD5110_refresh(&lcd1);
	    /* USER CODE END WHILE */
	}

    /* USER CODE BEGIN 3 */
  }


  /* USER CODE END 3 */

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 200;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 5;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
