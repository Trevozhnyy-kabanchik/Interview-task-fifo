#ifndef FIFO_H
#define FIFO_H

#include <stdbool.h>
#include <stdint.h>

// инициализация fifo
void fifo_init();
// Проверка на пустоту fifo
bool fifo_is_empty();
// читаем из fifo байт
bool read_fifo(uint8_t *data);
// записываем в fifo байт
bool write_fifo(uint8_t data);

#endif