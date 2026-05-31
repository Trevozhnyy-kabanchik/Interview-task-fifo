/**
 * @file crit_sec.c
 * @brief Реализация критической секции для Linux
 *
 * Делает одновременные вызовы write_fifo() из нескольких потоков
 * при запуске тестов
 */

#include "crit_sec.h"

#include <pthread.h>

static pthread_mutex_t fifo_write_lock = PTHREAD_MUTEX_INITIALIZER;

void crit_sec_enter(uint32_t *saved_state) {
  if (saved_state != NULL) {
    *saved_state = 0;
  }
  pthread_mutex_lock(&fifo_write_lock);
}

void crit_sec_exit(uint32_t saved_state) {
  (void)saved_state;
  pthread_mutex_unlock(&fifo_write_lock);
}
