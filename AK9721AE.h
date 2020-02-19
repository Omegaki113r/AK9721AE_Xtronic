#ifndef AK9721AE_H
#define AK9721AE_H

#include<Arduino.h>
#include<Wire.h>


class AK9721AE{
    public:
        AK9721AE(int INTN,int PDN,int RESET);
        int get_IR1();
        int get_IR2();
        int get_temp();
        int get_forward_volt();
        void write_to_i2c(int address, int data);
        byte get_status();
        bool check_errors(byte _status);
        bool check_DRDY(byte _status);
        int convert_to_CO2_milli_volts(int _value);
        int convert_to_temperature(int _value);
        int convert_to_LED_milli_volts(int _value);
        void get_sonsor_data();
    private:
        int _INTN;
        int _PDN;
        int _RESET;
        int _IR1 = 0;
        int _IR2 = 0;
        int _temp = 0;
        int _forward_volt = 0;
};
#endif