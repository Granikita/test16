/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : am_uart.c
  * @brief          : Amplitude-modulated UART on timer interrupt
  ******************************************************************************
  */
/* USER CODE END Header */

#include "am_uart.h"
#include "main.h"

/* ====================== Глобальные переменные ====================== */
volatile uint8_t  currentByte    = 0;     // Текущий байт для передачи
volatile uint8_t  bitIndex       = 0;     // Номер текущего бита
volatile bool     isTransmitting = false; // Флаг активной передачи
volatile uint32_t secondCounter  = 0;     // Счётчик для паузы 1 секунда

/* ====================== Точная задержка ====================== */
void delayUs(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000UL);
    while ((DWT->CYCCNT - start) < ticks);
}

/* ====================== Запуск передачи байта ====================== */
void transmitAmUart(uint8_t byte)
{
    __disable_irq();                // Критическая секция — защита от гонок
    currentByte = byte;
    bitIndex = 0;
    isTransmitting = true;
    __enable_irq();
}

/* ====================== Прерывание TIM2 (каждые ~417 мкс) ====================== */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM2 || !isTransmitting)
        return;

    __disable_irq();                // Критическая секция

    // Определяем, какой бит передаём сейчас
    uint8_t bit = (bitIndex == 0) ? 0 :                    // Старт-бит
                  (bitIndex == 9) ? 1 :                    // Стоп-бит
                  (currentByte >> (bitIndex - 1)) & 1;     // Биты данных

    if (bit == 1)
    {
        htim1.Instance->CCR1 = 100;   // PA8: высокий уровень
        htim1.Instance->CCR2 = 100;   // PA9: высокий уровень
    }
    else
    {
        htim1.Instance->CCR1 = 50;    // PA8: меандр 1 МГц
        htim1.Instance->CCR2 = 0;     // PA9: низкий уровень
    }

    bitIndex++;

    // Байт полностью передан?
    if (bitIndex > 9)
    {
        isTransmitting = false;
    }

    __enable_irq();
}
