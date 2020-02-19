#include "AK9721AE.h"

int AK9721AE::get_IR1(){
  return _IR1;
}

int AK9721AE::get_IR2(){
  return _IR2;
}

int AK9721AE::get_temp(){
  return _temp;
}
        
int AK9721AE::get_forward_volt(){
  return _forward_volt;
}

/*

custom implementation of the i2c write steps

*/
void AK9721AE::write_to_i2c(int address, int data){
  Wire.beginTransmission(address);
  Wire.write(byte(data));
  Wire.endTransmission();
}

/*

initial setup of the AK9721AE before reading the results

*/
AK9721AE::AK9721AE(int intn,int pdn,int reset){

    _INTN = intn;
    _PDN = pdn;
    _RESET = reset;

    //for detecting the power status of the AK9721AE
    pinMode(_INTN,INPUT);
    //for detecting the DRDY status of AK9721AE
    pinMode(_PDN,INPUT);
    //for resetting the AK9721AE if there is anything wrong
    pinMode(_RESET,OUTPUT);

    //establish communication with AK9721AE
    Wire.begin(0x65);

    //soft reset
    write_to_i2c(0x18,0xff);

    //measurement number of intermidiate data settings
    write_to_i2c(0x0f,0x00);

    //measurement time of intermidiate data settings
    write_to_i2c(0x10,0x48);

    //integration time settings
    write_to_i2c(0x11,0x29);

    //LED current adjustment settings
    write_to_i2c(0x16,0x18);
}

byte AK9721AE::get_status(){
    byte _status;
    //read status
    Wire.requestFrom(0x04,1);
    if(Wire.available()>=1){
        _status = Wire.read();
    }
    return _status;
}

/*

if there is an error within the Chip or with the power 
source there will be a FLAG in the appropirate registry
checking that and return true if there is an error => can take necessary steps ( Reset the AK9721AE)
false if there is no errors => can start reading the values

*/
bool AK9721AE::check_errors(byte _status){


  if(bitRead(_status,1) == 1){
    Serial.println("There was an error. Restarting the AK9721AE now .. ");
    return true;
  }else if(bitRead(_status,1) == 0){
    Serial.println("No Errors...");
    return false;
  }
}

/*

check whether the AK9721AE have read new data and stored them in the register
if DataReady then the appropirate bit value will be 1 => can read the values
if false then take necessary steps

*/
bool AK9721AE::check_DRDY(byte _status){
  if(bitRead(_status,0) == 0){
    Serial.println("Data not ready");
    return false;
  }else if(bitRead(_status,0) == 1){
    Serial.println("Data ready...");
    return true;
  }
}

/*

the IR sensor data are converted to the given mV values using the datasheet
Additional conversions may required to convert the mV to ppm

*/
//TODO conversion of mV to ppm
int AK9721AE::convert_to_CO2_milli_volts(int _value){
  if(_value == 0){
    return 0;
  }else if(_value == 1){
    return 8.94E-5;
  }else if(_value > 1 && _value <= 1118481){
    return map(_value,1,1118481,8.94E-5,100);
  }else if(_value > 1118481 && _value <= 5592405){
    return map(_value,1118481,5592405,100,500);
  }else if(_value > 5592405 && _value <= 8388607){
    return map(_value,5592405,8388607,500,750);
  }if(_value == -1){
    return -8.94E-5;
  }else if(_value < -1 && _value >= -1118481){
    return map(_value,-1,-1118481,-8.94E-5,-100);
  }else if(_value < -1118481 && _value >= -5592405){
    return map(_value,-1118481,-5592405,-100,-500);
  }else if(_value < -5592405 && _value >= -8388607){
    return map(_value,-5592405,-8388607,-500,-750);
  }
}

/*

Datasheet doesn't mention the conversion factors to convert the i2c values to 
temperature data. so currently return same input as the output

*/
//TODO convertion of the i2c values to real temperature values
int AK9721AE::convert_to_temperature(int _value){
  return _value;
}

/*

conversion of the LED forward voltages from the data given by i2c lines
as mentioned in the datasheet

*/
int AK9721AE::convert_to_LED_milli_volts(int _value){
  if(_value == 0){
    return 0;
  }else if(_value == 1){
    return 1400.05;
  }else if(_value > 1 && _value <= 8738){
    return map(_value,1,8738,1400.05,1800);
  }else if(_value > 8738 && _value <= 17476){
    return map(_value,8738,17476,1800,2200);
  }else if(_value > 17476 && _value <= 32767){
    return map(_value,17476,32767,2200,2900);
  }if(_value == -1){
    return 1399.95;
  }else if(_value < -1 && _value <= -8738){
    return map(_value,-1,-8738,1399.95,1000);
  }else if(_value < -8738 && _value <= -17476){
    return map(_value,-8738,-17476,1000,600);
  }else if(_value < -17476 && _value >= -32767){
    return map(_value,-17476,-32767,600,-100);
  }
}


void AK9721AE::get_sonsor_data(){

    //measure mode settings
    write_to_i2c(0x14,0x2);

    while(_PDN != LOW){
        delay(100);
    }

    byte _IR1_low,_IR1_med,_IR1_high,
            _IR2_low,_IR2_med,_IR2_high,
            _temp_low,_temp_high,
            _LED_forward_volt_low,_LED_forward_volt_high;
        
    int _IR1,_IR2,_temp,_LED_for_voltage;


    /*
    registers from 0x00 to 0x0E and from 0x0f to 0x17 are auto incremented
    */
    Wire.requestFrom(0x05,10);
    if(Wire.available()>=10){
        _IR1_low = Wire.read();
        _IR1_med = Wire.read();
        _IR1_high = Wire.read();

        _IR2_low = Wire.read();
        _IR2_med = Wire.read();
        _IR2_high = Wire.read();

        _temp_low = Wire.read();
        _temp_high = Wire.read();

        _LED_forward_volt_low = Wire.read();
        _LED_forward_volt_high = Wire.read();
    }

    _IR1 = _IR1_low<<16 | _IR1_med<<8 | _IR1_high;
    _IR2 = _IR2_low<<16 | _IR2_med<<8 | _IR2_high;
    _temp = _temp_low<<8 | _temp_high;
    _LED_for_voltage = _LED_forward_volt_low<<8 | _LED_forward_volt_high;

    
    _IR1 = convert_to_CO2_milli_volts(_IR1);
    _IR2 = convert_to_CO2_milli_volts(_IR2);
    _temp = convert_to_temperature(_temp);
    _forward_volt = convert_to_LED_milli_volts(_LED_for_voltage);

}