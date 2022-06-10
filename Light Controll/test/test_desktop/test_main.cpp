#include <unity.h>

void testInput(void){
    TEST_ASSERT_EQUAL(1, 1);
}

int main(int argc, char const *argv[])
{
    UNITY_BEGIN();
    RUN_TEST(testInput);
    UNITY_END();
    /* code */
    return 0;
}
