#ifndef AK9721AE_DATA_H
#define AK9721AE_DATA_H

#include<Arduino.h>

typedef struct{
    int IR1 = 0;
    int IR2 = 0;
    int temp = 0;
    int forward_volt = 0;
}AK9721AE_data;

#endif