#include <atomic>
#include <gtest/gtest.h>
#include <thread>
extern "C" {
#include "fifo.h"
}

namespace {
constexpr int kWritesPerWriter = 1000;
constexpr uint8_t kWriter0Tag = 0x00;
constexpr uint8_t kWriter1Tag = 0x80;
constexpr uint8_t kTagIndexMask = 0x7F;

uint8_t MakeTag(int writer_id, int index) {
  const auto base =
      static_cast<uint8_t>(writer_id != 0 ? kWriter1Tag : kWriter0Tag);
  return static_cast<uint8_t>(base | (index & kTagIndexMask));
}

bool IsFromWriter(uint8_t byte, int writer_id) {
  return writer_id == 0 ? (byte & kWriter1Tag) == 0 : (byte & kWriter1Tag) != 0;
}
} // namespace

class FifoTest : public ::testing::Test {
protected:
  void SetUp() override { fifo_init(); }
};

// Тесты, чтобы понять, что fifo в принципе работает
TEST_F(FifoTest, Basic) {
  EXPECT_TRUE(write_fifo(static_cast<uint8_t>('A')));
  EXPECT_TRUE(write_fifo(static_cast<uint8_t>('B')));
  EXPECT_TRUE(write_fifo(static_cast<uint8_t>('C')));

  uint8_t data;

  EXPECT_TRUE(read_fifo(&data));
  EXPECT_EQ(data, static_cast<uint8_t>('A'));
  EXPECT_TRUE(read_fifo(&data));
  EXPECT_EQ(data, static_cast<uint8_t>('B'));
  EXPECT_TRUE(read_fifo(&data));
  EXPECT_EQ(data, static_cast<uint8_t>('C'));

  EXPECT_FALSE(read_fifo(&data));
  EXPECT_TRUE(fifo_is_empty());
}

// Тесты, чтобы понять, что при переполнении всё норм
TEST_F(FifoTest, Overwrite) {
  constexpr int overwrite_bytes = 40;
  int write_bytes = 0;

  for (int i = 0; i < overwrite_bytes; ++i) {
    if (write_fifo(static_cast<uint8_t>('Z'))) {
      write_bytes++;
    }
  }

  EXPECT_LT(write_bytes, 40);
  EXPECT_EQ(write_bytes, 31);
  EXPECT_FALSE(write_fifo('Z'));
}

// Тест, чтобы проверить на чтение из пустого fifo
TEST_F(FifoTest, Empty) {
  auto data = static_cast<uint8_t>('Z');

  EXPECT_FALSE(read_fifo(&data));
  EXPECT_EQ(data, static_cast<uint8_t>('Z'));
  EXPECT_TRUE(fifo_is_empty());
}

// Тест на нескольких писателей
TEST_F(FifoTest, TwoWriters) {
  std::atomic<int> writer0_ok{0};
  std::atomic<int> writer1_ok{0};
  std::atomic<bool> start{false};
  auto writer = [&](int writer_id, std::atomic<int> *ok_count) {
    while (!start.load(std::memory_order_acquire)) {
    }
    for (int i = 0; i < kWritesPerWriter; ++i) {
      if (write_fifo(MakeTag(writer_id, i))) {
        ok_count->fetch_add(1, std::memory_order_relaxed);
      }
    }
  };
  std::thread t0(writer, 0, &writer0_ok);
  std::thread t1(writer, 1, &writer1_ok);
  start.store(true, std::memory_order_release);
  t0.join();
  t1.join();
  const int expected_total = writer0_ok.load() + writer1_ok.load();
  ASSERT_GT(expected_total, 0) << "ни один writer ничего не записал";
  int read_total = 0;
  int read_w0 = 0;
  int read_w1 = 0;
  uint8_t data = 0;
  while (read_fifo(&data)) {
    ++read_total;
    ASSERT_TRUE(IsFromWriter(data, 0) || IsFromWriter(data, 1));
    if (IsFromWriter(data, 0)) {
      ++read_w0;
    } else {
      ++read_w1;
    }
  }
  EXPECT_EQ(read_total, expected_total);
  EXPECT_EQ(read_w0, writer0_ok.load());
  EXPECT_EQ(read_w1, writer1_ok.load());
  EXPECT_TRUE(fifo_is_empty());
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}