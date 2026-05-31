# Interview-task-fifo

Тестовое задание: программный модуль кольцевого FIFO для приёма данных через UART.

Принятый байт записывается в FIFO в обработчике прерывания UART.
Извлечение байтов выполняется в основном потоке программы.

## Структура проекта

| Файл | Назначение |
|------|------------|
| `include/fifo.h` | Публичный API модуля FIFO |
| `include/crit_sec.h` | API критической секции для `write_fifo` |
| `src/fifo.c` | Реализация FIFO |
| `src/crit_sec.c` | Реализация критической секции на Linux |
| `tests/test.cpp` | Модульные тесты через Google Test |

## Предположения

- 32-битная архитектура
- Несколько писателей и один читатель в main.
- При переполнении новые байты **не записываются**, уже сохранённые **не перезаписываются**.
- Размер буфера — 32 ячейки, фактическая ёмкость — **31 байт**
  (одна ячейка резервируется для различения состояний «пусто» и «полон»).

## Сборка и тесты

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
./build/fifo_test
```


## Форматирование и статический анализ

```bash
clang-format -i include/*.h src/*.c tests/test.cpp

clang-tidy src/fifo.c src/crit_sec.c tests/test.cpp -p build --header-filter='(include|src|tests)/.*'
```

## Doxygen

```bash
doxygen Doxyfile
# результат: docs/html/index.html
```

## Инструменты

- **Google Test** — проверка логики FIFO
- **clang-format** — форматирование (`.clang-format`, стиль LLVM)
- **clang-tidy** — статический анализ (`.clang-tidy`)
- **Doxygen** — документация API из заголовков

## Пример использования (STM32)

```c
void UART_handler(void) {
  if (USART1->SR & USART_SR_RXNE) {
    uint8_t byte = (uint8_t)(USART1->DR & 0xFF);
    write_fifo(byte);
  }
}

int main(void) {
  fifo_init();
  // ... 

  uint8_t data;
  while (1) {
    if (read_fifo(&data)) {
      // обработка data
    }
  }
}
```
