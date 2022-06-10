#include <unity.h>
#include <Arduino.h>

void test(void){
    TEST_ASSERT_EQUAL(1, 1);
}

void setup(){
    UNITY_BEGIN();
    RUN_TEST(test);
    UNITY_END();
}

void loop(){
    
}