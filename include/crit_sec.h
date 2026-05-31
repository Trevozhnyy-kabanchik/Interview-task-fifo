#ifndef CRIT_SEC_H
#define CRIT_SEC_H

#include <stdint.h>

/**
 * @file crit_sec.h
 * @brief Критическая секция для защиты write_fifo() между несколькими
 * писателями.
 *
 * Используется внутри fifo.c. На Linux реализована через mutex,
 * чтобы тесты с несколькими потоками работали корректно
 */

/**
 * @brief Вход в критическую секцию.
 *
 * @param[out] saved_state Состояние для восстановления при exit
 */
void crit_sec_enter(uint32_t *saved_state);

/**
 * @brief Выход из критической секции.
 *
 * @param saved_state Значение, полученное из crit_sec_enter()
 */
void crit_sec_exit(uint32_t saved_state);

#endif
