/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hagl.h"
#include "font6x9.h"
#include "rgb565.h"
#include "sprites.c"
#include <stdio.h>
#include <wchar.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TOTAL_ENEMY_COUNT 21
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char str[20];  // enough to hold int32 + sign + null terminator
wchar_t wstr[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t JOY[2];
uint16_t joy_x = 0, joy_y = 0;
uint8_t button_state;

void char_to_wchar(const char *src, wchar_t *dst, size_t max_len) {
    for (size_t i = 0; i < max_len - 1 && src[i] != '\0'; i++) {
        dst[i] = (wchar_t)src[i];
        dst[i+1] = 0; // zawsze zakończ na null
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi2)
	{
		lcd_transfer_done();
	}
}

void Read_Joystick_ADC(void)
{

	joy_x = JOY[0];

	joy_y = JOY[1];

	//joystick button state
    button_state = !HAL_GPIO_ReadPin(UserButton_GPIO_Port, UserButton_Pin);

}

void draw_image(uint8_t offset_x, uint8_t offset_y, uint8_t width, uint8_t height, uint16_t *image){
    for(uint8_t y = 0; y < height; y++){
        for(uint8_t x = 0; x < width; x++){
            uint16_t pixel = image[y * width + x];
            hagl_put_pixel(offset_x + x, offset_y + y, pixel);
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, JOY, 2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  lcd_init();
  int8_t movement_vertical = 0;
  int8_t movement_horizontal = 0;
  uint8_t player_position = 80;
  uint8_t bullet_position_x = 0;
  uint8_t bullet_position_y = 0;
  uint8_t bullet_speed = 6;
  uint8_t enemy_bullet_position_x = 0;
  uint8_t enemy_bullet_position_y = 0;
  uint8_t enemy_bullet_speed = 2;
  uint8_t alive_enemy_count = 21;
  const uint8_t original_enemy_types[] =       { 3,  3,  3,  3,  3,  3,   3  , 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2};
  const uint8_t original_enemy_positions_x[] = { 10, 30, 50, 70, 90, 110, 130, 10, 30, 50, 70, 90, 110, 130, 10, 30, 50, 70, 90, 110, 130 };
  const uint8_t original_enemy_positions_y[] = { 20, 20, 20, 20, 20, 20,  20, 30, 30, 30, 30, 30, 30, 30, 40, 40, 40, 40, 40, 40, 40};
  uint8_t enemy_types[] =       { 3,  3,  3,  3,  3,  3,   3  , 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2};
  uint8_t enemy_positions_x[] = { 10, 30, 50, 70, 90, 110, 130, 10, 30, 50, 70, 90, 110, 130, 10, 30, 50, 70, 90, 110, 130 };
  uint8_t enemy_positions_y[] = { 20, 20, 20, 20, 20, 20,  20, 30, 30, 30, 30, 30, 30, 30, 40, 40, 40, 40, 40, 40, 40};


  uint16_t score = 0;
  uint16_t high_score = 0;
  uint8_t anim_counter = 0;
  uint8_t enemy_direction = -1;
  uint8_t enemy_change_direction = 0;
  uint8_t enemy_move_down = 0;
  uint8_t hp = 3;
  uint8_t game_over = 0;
  uint8_t lowest_enemy_position = 0;

  void restart_wave(){
	  lowest_enemy_position = 0;
	  enemy_move_down = 0;
	  enemy_direction = -1;
	  enemy_change_direction = 0;
	  anim_counter = 0;
	  alive_enemy_count = 21;
	  enemy_bullet_position_y = 0;
	  for(uint8_t i = 0; i < TOTAL_ENEMY_COUNT; i++){
		  enemy_types[i] = original_enemy_types[i];
		  enemy_positions_x[i] = original_enemy_positions_x[i];
		  enemy_positions_y[i] = original_enemy_positions_y[i];
	  }
  }
  while(HAL_GPIO_ReadPin(JOY_BTN_GPIO_Port, JOY_BTN_Pin) == GPIO_PIN_SET && HAL_GPIO_ReadPin(JOY_BTN_GPIO_Port, JOY_BTN_Pin) == GPIO_PIN_SET){
		hagl_clear_screen();
		hagl_put_text(L"Press UserButton to start", 2, 100, WHITE, font6x9);
		hagl_put_text(L"ALIEN", 62, 50, WHITE, font6x9);
		hagl_put_text(L"INVADERS", 55, 60, WHITE, font6x9);
		lcd_copy();
  }

  while (1)
  {
	  	if(HAL_GPIO_ReadPin(JOY_BTN_GPIO_Port, JOY_BTN_Pin) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(JOY_BTN_GPIO_Port, JOY_BTN_Pin) == GPIO_PIN_RESET) {
	  	  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	  	} else {
	  	  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	  	}
	    Read_Joystick_ADC();
	  	if (!lcd_is_busy()){
	  		if(game_over){
	  			hagl_clear_screen();
	  			hagl_put_text(L"GAME OVER", 50, 50, RED, font6x9);
				 sprintf(str,  "SCORE: %d", score);
				 char_to_wchar(str, wstr, 20);
				 hagl_put_text(wstr, 50, 60, RED, font6x9);
				 if(score > high_score){
					 hagl_put_text(L"NEW HIGHSCORE!", 30, 70, WHITE, font6x9);
				 }
				 hagl_put_text(L"Press UserButton to restart", 0, 100, WHITE, font6x9);
				 if(HAL_GPIO_ReadPin(UserButton_GPIO_Port, UserButton_Pin) == GPIO_PIN_RESET || HAL_GPIO_ReadPin(JOY_BTN_GPIO_Port, JOY_BTN_Pin) == GPIO_PIN_RESET) {
				  	  game_over = 0;
				  	  hp = 3;
				  	  if(score > high_score){
						high_score = score;
				  	  }
				  	  score = 0;
				  	}
				 restart_wave();
	  		}
	  		else{
	  		  if(alive_enemy_count == 0){
	  			  restart_wave();
	  		  }
	  		  if(hp==0 || lowest_enemy_position > 100){
	  			  game_over = 1;
	  		  }

			  if(enemy_change_direction){
				  enemy_direction = -enemy_direction;
				  enemy_change_direction = 0;
			  }
			  if(enemy_move_down){
				  enemy_move_down = 0;
				  for(uint8_t i = 0; i < TOTAL_ENEMY_COUNT; i++){
					  if(enemy_positions_y[i] != 0)
						  enemy_positions_y[i] += 5;
				  }
			  }
	  		  anim_counter++;
	  		  if(anim_counter > 30)
	  			  anim_counter = 0;
			  hagl_clear_screen();
//			  for (int i = 0; i < 8; i++) {
//				  hagl_draw_rounded_rectangle(2+i, 2+i, 158-i, 126-i, 8-i, rgb565(0, 0, i*16));
//			  }

			  // read user input
			  if(joy_x < 1000){
//				  sprintf(str, "up");
				  movement_vertical = 2;
			  }
			  else if(joy_x > 4000){
//				  sprintf(str, "down");
				  movement_vertical = -2;
			  }
			  else{
//				  sprintf(str, "center");
				  movement_vertical = 0;
			  }
//			  char_to_wchar(str, wstr, 20);
//			  hagl_put_text(wstr, 10, 10, RED, font6x9);
			  if(joy_y < 1000){
//				  sprintf(str, "right");
				  movement_horizontal = 2;
			  }
			  else if(joy_y > 4000){
//				  sprintf(str, "left");
				  movement_horizontal = -2;
			  }
			  else{
//				  sprintf(str, "center");
				  movement_horizontal = 0;
			  }
//			  char_to_wchar(str, wstr, 20);
//			  hagl_put_text(wstr, 10, 20, RED, font6x9);
			  sprintf(str, "SCORE: %d", score);
			  char_to_wchar(str, wstr, 20);
			  hagl_put_text(wstr, 10, 10, WHITE, font6x9);

			  sprintf(str, "HIGH SCORE: %d", high_score);
			  char_to_wchar(str, wstr, 20);
			  hagl_put_text(wstr, 70, 10, WHITE, font6x9);


			  // update player position
			  player_position += movement_horizontal;
			  if(player_position < 2)
				  player_position = 2;
			  if(player_position > 147)
				  player_position = 147;
			  draw_image(player_position, 100, 12, 12, sprite_player);

			  //update bullet position
			  if(bullet_position_y == 0){
				  bullet_position_x = player_position + 5;
				  bullet_position_y = 99;
			  }
			  bullet_position_y -= bullet_speed;

			  if(bullet_position_y < 20){
				  bullet_position_y = 0;
			  }

			  draw_image(bullet_position_x, bullet_position_y, 3, 4, sprite_bullet);

			  //update enemy bullet position
			  if(enemy_bullet_position_y == 0){
				  enemy_bullet_position_x = player_position + 5;
				  enemy_bullet_position_y = lowest_enemy_position;
		  		  lowest_enemy_position = 0;
			  }
			  enemy_bullet_position_y += enemy_bullet_speed;

			  if(enemy_bullet_position_y > 120){
				  enemy_bullet_position_y = 0;
			  }

			  draw_image(enemy_bullet_position_x, enemy_bullet_position_y, 3, 4, sprite_enemy_bullet);

			  if(((enemy_bullet_position_x - player_position) < 12 && (enemy_bullet_position_x - player_position) > 0 && (enemy_bullet_position_y - 100) < 8 && (enemy_bullet_position_y - 100) > 0)){
				  enemy_bullet_position_y = 0;
				  hp--;
			  }

			  //drawing enemies
			  for(uint8_t i = 0; i < TOTAL_ENEMY_COUNT; i++){
				  //skip if dead
				  if(enemy_positions_y[i] == 0)
					  continue;
				  //update lowest enemy position
				  if(enemy_positions_y[i] > lowest_enemy_position)
					  lowest_enemy_position = enemy_positions_y[i];
				  //detect collisions
				  if(((bullet_position_x - enemy_positions_x[i]) < 12 && (bullet_position_x - enemy_positions_x[i]) > 0 && (bullet_position_y - enemy_positions_y[i]) < 8 && (bullet_position_y - enemy_positions_y[i]) > 0)){
					  bullet_position_y = 0;
					  draw_image(enemy_positions_x[i], enemy_positions_y[i], 12, 8, sprite_explosion);
					  enemy_positions_y[i] = 0;
					  if(score < 999)
						  score++;
					  alive_enemy_count--;
					  if(alive_enemy_count == 2){
						  enemy_direction *= 2;
					  }
				  }
				  else {
					  enemy_positions_x[i] += enemy_direction;
					  if(enemy_positions_x[i] < 2){
						  enemy_positions_x[i] = 2;
						  enemy_change_direction = 1;
					  }
					  else if(enemy_positions_x[i] >= 147){
						  enemy_positions_x[i] = 147;
						  enemy_change_direction = 1;
						  enemy_move_down = 1;
					  }
					  if(enemy_types[i] == 1){
						  if(anim_counter > 5 && anim_counter < 15)
							  draw_image(enemy_positions_x[i], enemy_positions_y[i], 12, 8, sprite_crab_2);
						  else
							  draw_image(enemy_positions_x[i], enemy_positions_y[i], 12, 8, sprite_crab);
					  }
					  else if(enemy_types[i] == 2){
						  if(anim_counter > 10 && anim_counter < 20)
							  draw_image(enemy_positions_x[i], enemy_positions_y[i], 12, 8, sprite_octopus_2);
						  else
							  draw_image(enemy_positions_x[i], enemy_positions_y[i], 12, 8, sprite_octopus);
					  }
					  else{
						  if(anim_counter > 20 && anim_counter < 30)
							  draw_image(enemy_positions_x[i], enemy_positions_y[i], 12, 8, sprite_squid_2);
						  else
							  draw_image(enemy_positions_x[i], enemy_positions_y[i], 12, 8, sprite_squid);
					  }
				  }
			  }


			  //draw hp
			  if(hp > 1){
				  draw_image(10, 114, 12, 12, sprite_player);
			  }
			  if(hp > 2){
				  draw_image(30, 114, 12, 12, sprite_player);
			  }

	  		}
			  lcd_copy();
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

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
#ifdef USE_FULL_ASSERT
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
