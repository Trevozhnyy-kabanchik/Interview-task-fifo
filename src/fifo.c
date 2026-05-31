#include "fifo.h"
#include "crit_sec.h"

/**
 * @file fifo.c
 * @brief Реализация кольцевого FIFO
 */

/**
 * @brief Размер кольцевого буфера в байтах
 *
 * Хранится не более (BUF_SIZE - 1) байт, при head == tail
 * буфер считается пустым
 */
#define BUF_SIZE 32

static volatile uint8_t buf[BUF_SIZE]; /**< кольцевой буфер данных */
static volatile uint32_t head = (uint32_t)0; /**< следующая позиция записи */
static volatile uint32_t tail = (uint32_t)0; /**< следующая позиция чтения */

void fifo_init() {
  head = (uint32_t)0;
  tail = (uint32_t)0;
}

bool fifo_is_empty() { return head == tail; }

bool read_fifo(uint8_t *data) {
  if (fifo_is_empty()) {
    return false;
  }

  *data = buf[tail];
  tail = (tail + (uint32_t)1) % BUF_SIZE;

  return true;
}

bool write_fifo(uint8_t data) {
  uint32_t saved_state = 0;
  crit_sec_enter(&saved_state);

  uint32_t next_head = (head + (uint32_t)1) % BUF_SIZE;

  // Буфер полон => запись отклоняется
  if (next_head == tail) {
    crit_sec_exit(saved_state);
    return false;
  }

  buf[head] = data;
  head = next_head;

  crit_sec_exit(saved_state);
  return true;
}