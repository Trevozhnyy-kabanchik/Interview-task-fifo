#include <gtest/gtest.h>
extern "C"{
#include "fifo.h"
}

class FifoTest : public ::testing::Test{
protected:
    void SetUp() override{
        fifo_init();
    }
};

// Тесты, чтобы понять, что fifo в принципе работает
TEST_F(FifoTest, Basic){
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
TEST_F(FifoTest, Overwrite){
    int write_bytes = 0;

    for (int i = 0; i < 40; ++i){
        if (write_fifo(static_cast<uint8_t>('Z'))){
            write_bytes++;
        }
    }

    EXPECT_LT(write_bytes, 40);
    EXPECT_EQ(write_bytes, 31);
    EXPECT_FALSE(write_fifo('Z'));
}

// Тест, чтобы проверить на чтение из пустого fifo
TEST_F(FifoTest, Empty){
    uint8_t data = static_cast<uint8_t>('Z');

    EXPECT_FALSE(read_fifo(&data));
    EXPECT_EQ(data, static_cast<uint8_t>('Z'));
    EXPECT_TRUE(fifo_is_empty());
}

int main(int argc, char *argv[]){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}