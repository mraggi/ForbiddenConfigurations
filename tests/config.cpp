#include <gtest/gtest.h>
#include "config.h"

TEST(InitialTest, SuperDumb)
{
    auto K2 = Config::K(2);
    auto K3 = Config::K(3);
    
    ASSERT_TRUE(K2 <= K3);
    ASSERT_TRUE(K3 <= K3);
    ASSERT_FALSE(K3 <= K2);
}
