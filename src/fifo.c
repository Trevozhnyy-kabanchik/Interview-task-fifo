#include "fifo.h"

#define BUF_SIZE 32

// буфер для кольцевого fifo
static volatile uint8_t buf[BUF_SIZE];
// указатель на ячейку, куда запишем следующий байт
static volatile uint32_t head = (uint32_t) 0;
// указатель, откуда мы прочитаем самый старый непрочитанный байт
static volatile uint32_t tail = (uint32_t) 0;

void fifo_init(){
    head = (uint32_t) 0;
    tail = (uint32_t) 0;
}

bool fifo_is_empty(){
    return head == tail;
}

bool read_fifo(uint8_t *data){
    if (fifo_is_empty()) {
        return false;
    }

    *data = buf[tail];
    tail = (tail + (uint32_t) 1) % BUF_SIZE;

    return true;
}

bool write_fifo(uint8_t data){
    uint32_t next_head = (head + (uint32_t) 1) % BUF_SIZE;

    // Это проверка на косяк, если кольцо заполнилось, мы не можем перезаписать следующий байт, т.к. он ещё нужен, ведь он не прочитан
    if (next_head == tail){
        return false;
    }

    buf[head] = data;
    head = next_head;

    return true;
}