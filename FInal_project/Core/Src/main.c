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
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd5110.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define ROW_1_Pin GPIO_PIN_4
#define ROW_2_Pin GPIO_PIN_5
#define ROW_3_Pin GPIO_PIN_6
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
GPIO_TypeDef* ROW_3_Port = GPIOD;
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

#define MAP_WIDTH 84
#define MAP_HEIGHT 48
int SNAKE_SIZE = 8;
#define MAX_SNAKE_SIZE 25

#define BLACK 1
#define WHITE 0


#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3

int map[MAP_HEIGHT][MAP_WIDTH];
bool isDead = false;
int DIRECTION = UP;

typedef struct {
    int x;
    int y;
} SnakeSegment;

SnakeSegment snake[MAX_SNAKE_SIZE];

SnakeSegment apple;

void initializeSnake() {
    for (int i = 0; i < SNAKE_SIZE; ++i) {
        snake[i].x = i + 10;
        snake[i].y = 24;
    }
}

void initializeApple() {
    generateRandomApplePosition();
}

void generateRandomApplePosition() {
    srand(HAL_GetTick());

    while (1) {
        apple.x = rand() % (MAP_WIDTH - 8) + 2;
        apple.y = rand() % (MAP_HEIGHT - 8) + 2;

        if (isPositionEmpty(apple.x, apple.y)) {
            break;
        }
    }
}

int isPositionEmpty(int x, int y) {
    for (int i = 0; i < SNAKE_SIZE; ++i) {
        if (snake[i].x == x && snake[i].y == y) {
            return 0;
        }
    }

    if (map[y][x] == BLACK) {
        return 0;
    }

    return 1;
}

void drawApple(LCD5110_display* lcd_conf) {
    LCD5110_putpix(apple.x, apple.y, BLACK, &lcd_conf->hw_conf);
    LCD5110_putpix(apple.x, apple.y + 1, BLACK, &lcd_conf->hw_conf);
    LCD5110_putpix(apple.x + 1, apple.y, BLACK, &lcd_conf->hw_conf);
    LCD5110_putpix(apple.x + 1, apple.y + 1, BLACK, &lcd_conf->hw_conf);
}

void moveSnake(LCD5110_display* lcd_conf) {

	LCD5110_putpix(snake[SNAKE_SIZE-1].x, snake[SNAKE_SIZE-1].y, WHITE, &lcd_conf->hw_conf);
	LCD5110_putpix(snake[SNAKE_SIZE-1].x, snake[SNAKE_SIZE-1].y + 1, WHITE, &lcd_conf->hw_conf);
	LCD5110_putpix(snake[SNAKE_SIZE-1].x + 1, snake[SNAKE_SIZE-1].y, WHITE, &lcd_conf->hw_conf);
	LCD5110_putpix(snake[SNAKE_SIZE-1].x + 1, snake[SNAKE_SIZE-1].y + 1, WHITE, &lcd_conf->hw_conf);

    for (int i = SNAKE_SIZE - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }


    if (DIRECTION == RIGHT){
    	snake[0].x += 2;
    }
    else if (DIRECTION == UP){
    	snake[0].y -= 2;
    }
    else if (DIRECTION == LEFT){
    	snake[0].x -= 2;
    }
    else if (DIRECTION == DOWN){
    	snake[0].y += 2;
    }

    if (DIRECTION == RIGHT || DIRECTION == DOWN){
    	if (snake[0].x <= 1 || snake[0].x >= MAP_WIDTH - 2 || snake[0].y <= 1 || snake[0].y >= MAP_HEIGHT - 2) {
			isDead = true;
		}
    }else{
    	if (snake[0].x + 1 <= 1 || snake[0].x + 1 >= MAP_WIDTH - 2 || snake[0].y + 1 <= 1 || snake[0].y + 1 >= MAP_HEIGHT - 2) {
			isDead = true;
		}
    }


    for (int i = 1; i < SNAKE_SIZE; ++i) {
    	if (DIRECTION == RIGHT){
    		if ( (snake[0].x == snake[i].x + 1 && snake[0].y == snake[i].y) ||
    			 (snake[0].x == snake[i].x + 1 && snake[0].y == snake[i].y + 1)) {
				isDead = true;
				break;
			}
    	}
    	else if (DIRECTION == LEFT){
    		if ( (snake[0].x == snake[i].x && snake[0].y == snake[i].y) ||
				 (snake[0].x == snake[i].x && snake[0].y == snake[i].y + 1)) {
				isDead = true;
				break;
			}
    	}
    	else if (DIRECTION == UP){
    		if ( (snake[0].x == snake[i].x && snake[0].y == snake[i].y) ||
				 (snake[0].x == snake[i].x + 1 && snake[0].y == snake[i].y)) {
				isDead = true;
				break;
			}
    	}
    	else{
    		if ( (snake[0].x == snake[i].x && snake[0].y == snake[i].y + 1) ||
				 (snake[0].x == snake[i].x + 1 && snake[0].y == snake[i].y + 1)) {
				isDead = true;
				break;
			}
    	}
    }

	if (isDead) {
		for (int i = 1; i < SNAKE_SIZE; ++i) {
			LCD5110_putpix(snake[i].x, snake[i].y, WHITE, &lcd_conf->hw_conf);
			LCD5110_putpix(snake[i].x, snake[i].y + 1, WHITE, &lcd_conf->hw_conf);
			LCD5110_putpix(snake[i].x + 1, snake[i].y, WHITE, &lcd_conf->hw_conf);
			LCD5110_putpix(snake[i].x + 1, snake[i].y + 1, WHITE, &lcd_conf->hw_conf);
		}

		LCD5110_putpix(apple.x, apple.y, WHITE, &lcd_conf->hw_conf);
		LCD5110_putpix(apple.x, apple.y + 1, WHITE, &lcd_conf->hw_conf);
		LCD5110_putpix(apple.x + 1, apple.y, WHITE, &lcd_conf->hw_conf);
		LCD5110_putpix(apple.x + 1, apple.y + 1, WHITE, &lcd_conf->hw_conf);

		initializeSnake();
		initializeApple();

		drawApple(&lcd1);
		DIRECTION = UP;
		isDead = false;
	}

    if ( (snake[0].x == apple.x && snake[0].y == apple.y ) ||
    	 (snake[0].x == apple.x + 1 && snake[0].y == apple.y) ||
		 (snake[0].x == apple.x && snake[0].y == apple.y + 1) ||
		 (snake[0].x == apple.x + 1 && snake[0].y == apple.y + 1) ||
		 (snake[0].x + 1 == apple.x && snake[0].y + 1 == apple.y) ||
		 (snake[0].x + 1 == apple.x + 1 && snake[0].y + 1 == apple.y) ||
		 (snake[0].x + 1 == apple.x && snake[0].y + 1 == apple.y + 1) ||
		 (snake[0].x + 1 == apple.x + 1 && snake[0].y + 1 == apple.y + 1)) {
		if (SNAKE_SIZE < MAX_SNAKE_SIZE) {
			snake[SNAKE_SIZE].x = snake[SNAKE_SIZE - 1].x;
			snake[SNAKE_SIZE].y = snake[SNAKE_SIZE - 1].y;
			SNAKE_SIZE++;

		}

		LCD5110_putpix(apple.x, apple.y, WHITE, &lcd_conf->hw_conf);
		LCD5110_putpix(apple.x, apple.y + 1, WHITE, &lcd_conf->hw_conf);
		LCD5110_putpix(apple.x + 1, apple.y, WHITE, &lcd_conf->hw_conf);
		LCD5110_putpix(apple.x + 1, apple.y + 1, WHITE, &lcd_conf->hw_conf);

		generateRandomApplePosition();
		drawApple(lcd_conf);
	}

}

void drawSnake(LCD5110_display* lcd_conf) {
    for (int i = 0; i < SNAKE_SIZE; ++i) {
        LCD5110_putpix(snake[i].x, snake[i].y, BLACK, &lcd_conf->hw_conf);
        LCD5110_putpix(snake[i].x, snake[i].y + 1, BLACK, &lcd_conf->hw_conf);
        LCD5110_putpix(snake[i].x + 1, snake[i].y, BLACK, &lcd_conf->hw_conf);
        LCD5110_putpix(snake[i].x + 1, snake[i].y + 1, BLACK, &lcd_conf->hw_conf);
    }
}

void clearSnake(LCD5110_display* lcd_conf){
	for (int i = 0; i < SNAKE_SIZE; ++i) {
		LCD5110_putpix(snake[i].x, snake[i].y, WHITE, &lcd_conf->hw_conf);
	}
}

void initializeMap() {
    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            map[i][j] = WHITE;
        }
    }

    for (int i = 0; i < MAP_WIDTH; ++i) {
        map[0][i] = BLACK;
        map[1][i] = BLACK;
        map[MAP_HEIGHT - 1][i] = BLACK;
        map[MAP_HEIGHT - 2][i] = BLACK;
    }

    for (int i = 0; i < MAP_HEIGHT; ++i) {
        map[i][0] = BLACK;
        map[i][1] = BLACK;
        map[i][MAP_WIDTH - 1] = BLACK;
        map[i][MAP_WIDTH - 2] = BLACK;
    }
}

void drawMap(LCD5110_display* lcd_conf) {
    LCD5110_clear_scr(lcd_conf);

    for (int i = 0; i < MAP_HEIGHT; ++i) {
        for (int j = 0; j < MAP_WIDTH; ++j) {
            if (map[i][j] == BLACK) {
                LCD5110_putpix(j, i, BLACK, &lcd_conf->hw_conf);
            }
        }
    }

    LCD5110_refresh(lcd_conf);
}

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

char buf[10] = {0};

char usart1_rx_buf[10000] = {0,};
char messages[100][300];
char numbers[100][11];
int cnt_msg = 0;
bool next_msg = false;

volatile uint8_t usart1_byte;
volatile uint8_t usart1_byte_count = 0;
int check = 0;
int end_r = 0;
bool get_call = false;
bool cursor_on_phone = true;
bool screen_main = false;
bool cursor_on_message = false;
bool screen_message = false;
bool type_message_screen = false;
bool screen_begin = true;
bool want_to_send_digits = true;
bool end_call_flag = false;
bool cursor_on_read = false;
bool message_read = false;
bool snake_screen = false;
bool snake_start = false;
int num_of_msg = 0;

const char *MESSAGE_MODE = "AT+CMGF=1\r\n";

const char *MSG_NUMBER_INIT = "AT+CMGS=\"+38";

const char *CALL_NUMBER = "ATD+38";

const char *END_CALL = "AT+CHUP\r\n";

const char *CALL_ANSWER = "ATA\r\n";

const char *CHECK_NUMBER = "AT+CLCC\r\n";

const char *MAX_VOLUME = "AT+CRSL=15\r\n";

const char *READ_LAST_MSG = "AT+CMGR=";

const char *SEE_MEM = "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n";

const char *SAVE_SMS = "AT+CMGW\r\n";

const char *REC_MODE = "AT+CNMI=2,2,0,0,0\r\n";

typedef struct module{
	char *last_response;
	char *last_command;
} module_t;

module_t gsm_module;

module_t init_module() {
    module_t newModule;
    newModule.last_response = strdup("");
    newModule.last_command = strdup("");

    if (newModule.last_response == NULL || newModule.last_command == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return newModule;
}

void free_module(struct module *myModule) {
    free(myModule->last_response);
    free(myModule->last_command);
}

void send_message(char* number, char* msg){
	char *number_command = MSG_NUMBER_INIT;
	strcat(number_command, number);
	strcat(number_command, "\"\r\n");
	char *message = msg;
	strcat(message, "\r\n\x1A");

	HAL_UART_Transmit(&huart1,(uint8_t *)MESSAGE_MODE,strlen(MESSAGE_MODE),100);
	HAL_UART_Transmit(&huart1,(uint8_t *)number_command,strlen(number_command),100);
	HAL_UART_Transmit(&huart1,(uint8_t *)message,strlen(message),100);
}

void call_number(char* number){
	char call_command[16];
	strcat(call_command, CALL_NUMBER);
	strcat(call_command, number);
	strcat(call_command, "\r\n");
	HAL_UART_Transmit(&huart1,(uint8_t *)call_command,strlen(call_command),100);
}

char *answer_call(module_t gsm_module){
	HAL_UART_Transmit(&huart1, (uint8_t *)CALL_ANSWER, strlen(CALL_ANSWER), 100);
	HAL_Delay(200);
}

void end_call(){
	HAL_UART_Transmit(&huart1,(uint8_t *)END_CALL, 9,100);
}

void read_message(module_t gsm_module, int number){
	LCD5110_set_cursor(0, 0, &lcd1);
	LCD5110_print(numbers[99 - number], 0, &lcd1);
	LCD5110_set_cursor(0, 10, &lcd1);
	LCD5110_print(messages[99 - number], 0, &lcd1);
}

void handle_interrupts(module_t gsm_module, char *rx_buff, int count){
	if (count >= 5){
		char first[2];
		char second[2];
		char third[2];
		char fourth[2];
		first[0] = rx_buff[2];
		first[1] = '\0';
		second[0] = rx_buff[3];
		second[1] = '\0';
		third[0] = rx_buff[4];
		third[1] = '\0';
		fourth[0] = rx_buff[5];
		fourth[1] = '\0';

		if (message_read){
			LCD5110_set_cursor(0, 0, &lcd1);
			LCD5110_print(rx_buff, 0, &lcd1);

		}
		if (strcmp(rx_buff, "OK\r\n") == 0){
			return;
		}
		if (strcmp(rx_buff, "ERROR\r\n") == 0){
			return;
		}
		if ((second[0] == 'C' && third[0] == 'A' && fourth[0] == 'R') || (first[0] == 'R' && second[0] == 'O' && third[0] == 'R')){
			end_call_flag = true;
		}
		if (first[0] == 'R' && second[0] == 'I' && third[0] == 'N' && fourth[0] == 'G'){
			cursor_on_phone = false;
			screen_main = false;
			cursor_on_message = false;
			screen_message = false;
			type_message_screen = false;
			screen_begin = false;
			want_to_send_digits = false;
			get_call = true;
			memcpy(buf, &rx_buff[11], 10);
			buf[10] = '\0';
			return;
		}
	}

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  	lcd1.hw_conf.spi_handle = &hspi1;
    lcd1.hw_conf.spi_cs_pin =  GPIO_PIN_2;
    lcd1.hw_conf.spi_cs_port = GPIOA;
    lcd1.hw_conf.rst_pin =  GPIO_PIN_0;
    lcd1.hw_conf.rst_port = GPIOC;
    lcd1.hw_conf.dc_pin =  GPIO_PIN_3;
    lcd1.hw_conf.dc_port = GPIOA;
    lcd1.def_scr = lcd5110_def_scr;
    LCD5110_init(&lcd1.hw_conf, LCD5110_NORMAL_MODE, 0x40, 2, 3);

    //gsm_module = init_module();
	char ATcommand[100];
	char res[10];
		sprintf(ATcommand,"AT\r\n");
		HAL_UART_Transmit(&huart1,(uint8_t *)ATcommand,strlen(ATcommand),100);
		HAL_UART_Receive (&huart1, (uint8_t *)res, 10, 10);

		if(strstr((char *)res,"OK"))
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);   // green
		}
		else{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);    // red

		}



    int pos = 0;

    char key_pressed = 0;

    rect_t rect={0,0, 83, 10};

    const unsigned char main_screen_image[] = {
    		0x0f, 0x03, 0xf1, 0xf8, 0xf9, 0x33, 0x07, 0xcf, 0xff, 0xff, 0xff, 0xe6, 0xe4, 0xcc, 0x19, 0x39,
    		0xe3, 0x87, 0x1f, 0xfe, 0xf8, 0xf1, 0xc7, 0x8f, 0x3c, 0x38, 0x73, 0xe7, 0xcf, 0xcf, 0xcf, 0xcf,
    		0xe7, 0xe7, 0xcf, 0x1f, 0x3f, 0x7f, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x04, 0x04,
    		0x01, 0x01, 0x01, 0x01, 0x01, 0x04, 0x04, 0x06, 0x07
    };

    const unsigned char main_screen_snake[] = {
		0x01, 0xf8, 0xfc, 0xfe, 0xfe, 0x8e, 0x06, 0x66, 0x76, 0x76, 0x76, 0x7e, 0x76, 0xfe, 0xfe, 0xfe,
		0xfc, 0xf8, 0x01, 0x00, 0xff, 0xff, 0x8f, 0x07, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x72,
		0x78, 0x3f, 0x9f, 0xff, 0xff, 0x60, 0x04, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
		0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x04
    };

    const unsigned char main_message[] = {
    		0x01, 0xfc, 0xfe, 0xfe, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xfe,
    		0xfe, 0xfc, 0x01, 0xe0, 0xcf, 0xdf, 0x1b, 0xfb, 0xfb, 0x7b, 0x3b, 0x9b, 0xdb, 0xdb, 0xdb, 0xdb,
    		0xdb, 0xdb, 0xdb, 0xdf, 0xcf, 0xe0, 0x07, 0x07, 0x07, 0x00, 0x04, 0x06, 0x07, 0x07, 0x07, 0x07,
    		0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07
    };

    const unsigned char write_message[] = {
    		0xff, 0xff, 0x7f, 0x3f, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0xbf, 0x3f, 0x3f, 0x9f, 0xcf, 0x67, 0x83,
    		0xc7, 0xef, 0xff, 0xff, 0xff, 0xe0, 0xc0, 0xdf, 0x9f, 0xbf, 0x3f, 0x13, 0x10, 0xd0, 0xd9, 0xdd,
    		0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x07,
    		0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07
    };


    char* letters_for_keys[] = {
            "ABC",
            "DEF",
            "GHI",
            "JKL",
            "MNO",
            "PQR",
            "STU",
            "VWX",
            "YZ"
        };

    int letters[] = {0,0,0,0,0,0,0,0,0};

    bool is_call;

	HAL_UART_Transmit(&huart1,(uint8_t *)REC_MODE,strlen(REC_MODE),100);
    char message[32] = {0};
    int message_pos = 0;

    gsm_module = init_module();

    keypad_init();

    void init_screen() {
		LCD5110_clear_scr(&lcd1);
		LCD5110_refresh(&lcd1);
		LCD5110_rect(&rect, 1, &lcd1);
		LCD5110_rect_fill(&rect, 1, &lcd1);
		LCD5110_set_cursor(10, 15, &lcd1);

		LCD5110_print("A-call", 0, &lcd1);

		LCD5110_set_cursor(10, 35, &lcd1);

		LCD5110_print("B-back", 0, &lcd1);
		LCD5110_set_cursor(0, 0, &lcd1);
		is_call = false;
    }

    void call_screen() {
    	LCD5110_clear_scr(&lcd1);
    	LCD5110_refresh(&lcd1);
    	char phone_info[26];
    	LCD5110_set_cursor(18, 1, &lcd1);
    	strcpy(phone_info, "Calling\n");
    	LCD5110_print(phone_info, BLACK, &lcd1);
    	LCD5110_set_cursor(10, 20, &lcd1);
    	LCD5110_print(buf, WHITE, &lcd1);
    	LCD5110_set_cursor(10, 40, &lcd1);
    	LCD5110_print("A-end call", 0, &lcd1);
    	is_call = true;
    }

    void screen_on_read(int number) {
    	LCD5110_clear_scr(&lcd1);
		LCD5110_refresh(&lcd1);
		LCD5110_set_cursor(65, 40, &lcd1);

		LCD5110_print("A->", 0, &lcd1);

		read_message(gsm_module, number);
		if (number != 0){
			LCD5110_set_cursor(5, 40, &lcd1);
			LCD5110_print("<-C", 0, &lcd1);
		}
		LCD5110_set_cursor(20, 40, &lcd1);
		LCD5110_print("B-exit", 0, &lcd1);
    }

    typedef enum {
      PHONE,
      MESSAGEE,
      SNAKE
    } Screen;

    void get_call_screen(){
    	LCD5110_clear_scr(&lcd1);
		LCD5110_refresh(&lcd1);
		LCD5110_print(buf, 0, &lcd1);
		LCD5110_set_cursor(5, 15, &lcd1);

		LCD5110_print("A-accept call", 0, &lcd1);

		LCD5110_set_cursor(5, 25, &lcd1);

		LCD5110_print("B-reject call", 0, &lcd1);
		LCD5110_set_cursor(0, 0, &lcd1);
    }

    void main_screen(Screen type_screen) {
    	LCD5110_clear_scr(&lcd1);
		LCD5110_refresh(&lcd1);
		LCD5110_drawBitmap(8, 3, main_screen_image, 19, 19, 0, &lcd1.hw_conf);

		rect_t rectbox_phone={6, 1, 23, 23};
		rect_t rectbox_snake={49,1, 23, 23};
		rect_t rectbox_message={6,23, 23, 23};
		rect_t rectbox_message_history = {48, 23, 23, 23};
		if (type_screen==0) {
			LCD5110_rect(&rectbox_phone, 1, &lcd1);
		}
		else if (type_screen==1) {
			LCD5110_rect(&rectbox_snake, 1, &lcd1);
		}
		else if (type_screen == 2) {
			LCD5110_rect(&rectbox_message, 1, &lcd1);
		} else {
			LCD5110_rect(&rectbox_message_history, 1, &lcd1);
		}
		LCD5110_refresh(&lcd1);
		LCD5110_set_cursor(50, 10, &lcd1);
		LCD5110_drawBitmap(50, 3, main_screen_snake, 19, 19, 0, &lcd1.hw_conf);
		LCD5110_refresh(&lcd1);
		LCD5110_set_cursor(8, 28, &lcd1);
		LCD5110_drawBitmap(8, 25, write_message, 19, 19, 0, &lcd1.hw_conf);
		LCD5110_set_cursor(50, 28, &lcd1);
		LCD5110_drawBitmap(50, 25, main_message, 19, 19, 0, &lcd1.hw_conf);
    }

    bool is_message;

    void screen_for_message_sending() {
    	LCD5110_clear_scr(&lcd1);
		LCD5110_refresh(&lcd1);
		LCD5110_rect(&rect, 1, &lcd1);
		LCD5110_rect_fill(&rect, 1, &lcd1);
		LCD5110_set_cursor(10, 15, &lcd1);

		LCD5110_print("A-go to message", 0, &lcd1);

		LCD5110_set_cursor(10, 35, &lcd1);

		LCD5110_print("B-back", 0, &lcd1);
		LCD5110_set_cursor(0, 0, &lcd1);
		is_message = false;
    }

    void snake_screen_build() {
    	LCD5110_clear_scr(&lcd1);
		LCD5110_refresh(&lcd1);
		initializeMap();
		initializeSnake();
		initializeApple();


		drawMap(&lcd1);
		drawApple(&lcd1);
    }

    void type_message_scren() {
        	LCD5110_clear_scr(&lcd1);
        	LCD5110_refresh(&lcd1);
        	rect_t rectbox_phone={1, 1, 60, 30};
        	LCD5110_set_cursor(5, 40, &lcd1);
        	LCD5110_print("A-send", 0, &lcd1);
        	is_message = true;
        }





    main_screen(cursor_on_phone - 1);

    HAL_UART_Receive_IT(&huart1, usart1_rx_buf, 1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
	    key_pressed = keypad_scan();
	    if (snake_start){
	    	drawSnake(&lcd1);
	    	moveSnake(&lcd1);

		   if(key_pressed != 0)
		   {
			if (key_pressed == '2' & DIRECTION != UP & DIRECTION != DOWN){
			 DIRECTION = UP;
			}
			else if (key_pressed == '6' & DIRECTION != RIGHT & DIRECTION != LEFT){
			 DIRECTION = RIGHT;
			}
			else if (key_pressed == '8' & DIRECTION != DOWN & DIRECTION != UP){
			 DIRECTION = DOWN;
			}
			else if (key_pressed == '4' & DIRECTION != LEFT & DIRECTION != RIGHT){
			 DIRECTION = LEFT;
			}
    		else if (key_pressed == 'B'){
    			snake_screen = false;
    			snake_start = false;
    			screen_main = true;
    			main_screen(cursor_on_phone - 1);
    		}

		   }

		   LCD5110_refresh(&lcd1);
		   HAL_Delay(10);
	    }
	    else if (get_call){
    		get_call_screen(buf);
    		if (key_pressed == 'A'){
    			answer_call(gsm_module);
    			get_call = false;
    			screen_main = true;
    			call_screen();
    		}
    		else if (key_pressed == 'B'){
    			end_call();
    			get_call = false;
    			screen_main = true;
				memset(buf, ' ', sizeof(buf) - 1);
				buf[0] = '\0';
				pos = 0;
    			main_screen(cursor_on_phone - 1);
    		}
    	}
    	else if(key_pressed != 0 || end_call_flag)
	    {
	    	if (type_message_screen) {
	    		if (!want_to_send_digits) {
					if (key_pressed >= '0' && key_pressed <= '9') {
						int num = key_pressed - '0';
						letters[num - 1]++;
					}
					if (key_pressed == 'C') {
						int maxindex = 0;
						for (int i = 0; i < 9; i++) {
								if (letters[i] > letters[maxindex]) {
									maxindex = i;
								}
							}
							char *message_t = letters_for_keys[maxindex];
							message[message_pos] = message_t[letters[maxindex] - 1];
							message_pos++;
							message[message_pos] = '\0';
							LCD5110_set_cursor(0, 0, &lcd1);
							LCD5110_print(message, 0, &lcd1);
							LCD5110_refresh(&lcd1);
							memset(letters, 0, sizeof(letters));
						}
					if (key_pressed == 'A') {
						strcat(message, "\r\n\x1A");
						HAL_UART_Transmit(&huart1,(uint8_t *)message,strlen(message),100);
						HAL_Delay(500);
						HAL_UART_Receive (&huart1, (uint8_t *)res, 10, 10);
						LCD5110_refresh(&lcd1);
						HAL_Delay(500);
						message[0] = '\0';
						message_pos = 0;
						LCD5110_refresh(&lcd1);
						type_message_screen = false;
						screen_begin = true;
						want_to_send_digits = true;
						buf[0] = '\0';
						message[0] = '\0';
						message_pos = 0;
						pos = 0;
						main_screen(cursor_on_phone - 1);
					}
	    		}
	    		else {
	    			if (key_pressed == 'C') { // changing mode to letters
						want_to_send_digits = false;
					}

					else if (key_pressed >= '0' && key_pressed <= '9' && message_pos < sizeof(message))
						{
							message[message_pos] = key_pressed;
							message_pos++;
							message[message_pos] = '\0';

							LCD5110_set_cursor(0, 0, &lcd1);
							LCD5110_print(message, 0, &lcd1);
						}
						if (key_pressed == 'A') {


							strcat(message, "\r\n\x1A");

							HAL_UART_Transmit(&huart1,(uint8_t *)message,strlen(message),100);
							HAL_Delay(500);
							HAL_UART_Receive (&huart1, (uint8_t *)res, 10, 10);
							LCD5110_refresh(&lcd1);
							HAL_Delay(500);
							message[0] = '\0';
							message_pos = 0;
							LCD5110_refresh(&lcd1);
							type_message_screen = false;
							screen_begin = true;
							buf[0] = '\0';
							pos = 0;
							main_screen(cursor_on_phone - 1);
						}
	    			}
	    		}
	    	else if (message_read){

	    		if (key_pressed == 'B') {
	    			message_read = false;
	    			screen_begin = true;
	    			main_screen(cursor_on_phone - 1);
	    		}

	    	}
	    	else if (screen_message) {
	    		if (key_pressed == 'B') {
	    			screen_message = false;
	    			type_message_screen = false;
	    			screen_begin = true;
	    			memset(buf, ' ', sizeof(buf) - 1);
					buf[0] = '\0';
					pos = 0;
					main_screen(cursor_on_phone - 1);
	    		}
	    		if (key_pressed >= '0' && key_pressed <= '9' && pos < sizeof(buf))
					{
						buf[pos] = key_pressed;
						pos++;
						buf[pos] = '\0';

						LCD5110_set_cursor(0, 0, &lcd1);
						LCD5110_print(buf, 0, &lcd1);
					}
	    		if (key_pressed == 'A') {
					  HAL_UART_Transmit(&huart1,(uint8_t *)MESSAGE_MODE,strlen(MESSAGE_MODE),100);
					  HAL_UART_Receive (&huart1, (uint8_t *)res, 10, 10);
					  if(strstr((char *)res,"OK")){
						  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);   // green
					  }
					  else{
						HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
					  }

					  char current_number[25];
					  strcpy(current_number, MSG_NUMBER_INIT);
					  strcat(current_number, buf);
					  strcat(current_number, "\"\r\n");
					  HAL_UART_Transmit(&huart1,(uint8_t *)current_number,strlen(current_number),100);
					  HAL_UART_Receive (&huart1, (uint8_t *)res, 10, 10);

	    			screen_message = false;
	    			type_message_screen = true;
	    			type_message_scren();
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
	    	}
	    	else if (screen_begin) {
	    		end_call_flag = false;
	    		if (key_pressed == '1' || key_pressed == '2' || key_pressed == '3' || key_pressed == '4') {
	    			int num = key_pressed - '0';
	    			if (num == 3) {
	    				cursor_on_message = true;
	    				cursor_on_read = false;
	    				cursor_on_phone = false;
	    			} else if (num == 1) {
	    				cursor_on_message = false;
	    				cursor_on_read = false;
	    				cursor_on_phone = true;
	    			} else if (num == 4){
	    				cursor_on_read = true;
	    				cursor_on_phone = false;
	    				cursor_on_message = false;
	    			} else if (num == 2){
	    				snake_screen = true;
						cursor_on_read = false;
						cursor_on_phone = false;
						cursor_on_message = false;

	    			}
	    			main_screen(num - 1);
	    		}
				if (key_pressed == '*' && cursor_on_phone) {
						screen_main = true;
						screen_begin = false;
						init_screen();
					}
				if (key_pressed == '*' && cursor_on_message) {
					screen_message = true;
					screen_begin = false;
					screen_for_message_sending();
				}
				if (key_pressed == '*' && cursor_on_read){
					screen_begin = false;
					message_read = true;
					screen_on_read(0);
				}
				if (key_pressed == '*' && snake_screen){
					snake_start = true;
					screen_main = false;
					screen_begin = false;
					snake_screen_build();
				}
	    	}
	    	else if (screen_main){
	    		if (key_pressed == 'B') {
						screen_main = false;
						screen_begin = true;
						memset(buf, ' ', sizeof(buf) - 1);
						buf[0] = '\0';
						pos = 0;
						main_screen(cursor_on_phone - 1);
				}
				if ((end_call_flag || key_pressed == 'A') && screen_main) {
					if (is_call) {
						buf[0] = '\0';
						pos = 0;
						end_call();
						get_call = false;
						LCD5110_refresh(&lcd1);
						LCD5110_rect_fill(&rect, 1, &lcd1);
						LCD5110_set_cursor(0, 0, &lcd1);
						screen_begin = true;
						main_screen(0);
					}
					else {
						char call_number[16];
						strcpy(call_number, CALL_NUMBER);
						strcat(call_number, buf);
						strcat(call_number, "\r\n");
						HAL_UART_Transmit(&huart1,(uint8_t *)call_number,strlen(call_number),100);
						call_screen();

					}
				}
				else if (key_pressed == 'D' && screen_main)
				{
					memset(buf, ' ', sizeof(buf) - 1);
					buf[0] = '\0';
					pos = 0;
					LCD5110_refresh(&lcd1);
					LCD5110_rect_fill(&rect, 1, &lcd1);
					LCD5110_set_cursor(0, 0, &lcd1);
				}
				else if (key_pressed == 'C' && pos > 0 && screen_main)
					{
						pos--;
						buf[pos] = '\0';

						LCD5110_refresh(&lcd1);
						LCD5110_rect_fill(&rect, 1, &lcd1);
						LCD5110_set_cursor(0, 0, &lcd1);
						LCD5110_print(buf, 0, &lcd1);
					}
				else if (key_pressed >= '0' && key_pressed <= '9' && pos < sizeof(buf) && screen_main)
				{
					buf[pos] = key_pressed;
					pos++;
					buf[pos] = '\0';

					LCD5110_set_cursor(0, 0, &lcd1);
					LCD5110_print(buf, 0, &lcd1);
				}
	    	}

	    }
	    LCD5110_refresh(&lcd1);
	    HAL_Delay(300);
	    LCD5110_refresh(&lcd1);
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


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if(huart == &huart1)
   {

    HAL_UART_Receive_IT(&huart1, (uint8_t*)&usart1_byte, 1);


    if(usart1_byte_count< 10000)
    {
      usart1_rx_buf[usart1_byte_count++] = usart1_byte;
    }

    if (check > 0){
      if(usart1_rx_buf[usart1_byte_count- 1 ] == '\n' && usart1_rx_buf[usart1_byte_count - 2] == '\r' && end_r < 2){
        usart1_byte_count = 0;
        usart1_rx_buf[0] = '\0';
        end_r++;
      } else if(usart1_rx_buf[usart1_byte_count - 1] == '\r' && end_r > 1){
        if (usart1_byte_count > 1){
          handle_interrupts(gsm_module, usart1_rx_buf, check);
          check = 0;
          end_r = 0;
          usart1_byte_count = 0;
        }
      }
    }

    check++;
  }


}

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
