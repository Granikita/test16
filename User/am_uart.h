/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : am_uart.h
  * @brief          : Header for amplitude-modulated UART on interrupts
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef APPLICATION_AM_UART_H_
#define APPLICATION_AM_UART_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

extern TIM_HandleTypeDef htim1;     // Таймер PWM для модуляции

/* ====================== Глобальные переменные ====================== */
/* Все переменные volatile, чтобы прерывание и main не конфликтовали */
extern volatile uint8_t  currentByte;      // Текущий байт (0x00..0xFF)
extern volatile uint8_t  bitIndex;         // Номер бита (0=старт, 1-8=данные, 9=стоп)
extern volatile bool     isTransmitting;   // true — сейчас идёт передача байта
extern volatile uint32_t secondCounter;    // Счётчик миллисекунд для паузы 1 секунда

/* ====================== Прототипы функций ====================== */

/*!
 * \brief Точная задержка в микросекундах через DWT
 * \param us Количество микросекунд
 */
void delayUs(uint32_t us);

/*!
 * \brief Запуск передачи одного байта (вызывается из main)
 * \param byte Байт, который нужно передать
 */
void transmitAmUart(uint8_t byte);

/*!
 * \brief Обработчик прерывания TIM2 — передача каждого бита
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* APPLICATION_AM_UART_H_ */
