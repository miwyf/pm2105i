#ifndef _PM2105i_h
#define _PM2105i_h

#include <Arduino.h>
#include <Wire.h>

#define PM2105i_ADDRESS                    0x28

// Control modes
#define PM2105i_CTRL_CLOSE_MEASUREMENT                 0x1
#define PM2105i_CTRL_OPEN_SINGLE_MEASUREMENT           0x2
#define PM2105i_CTRL_SET_UP_CONTINUOUSLY_MEASUREMENT   0x3
#define PM2105i_CTRL_SET_UP_TIMING_MEASUREMENT         0x4
#define PM2105i_CTRL_SET_UP_DYNAMIC_MEASUREMENT        0x5
#define PM2105i_CTRL_SET_UP_CALIBRATION_COEFFICIENT    0x6
#define PM2105i_CTRL_SET_UP_WARM_MODE                  0x7

#define PM2105i_CONTROL_MODE               PM2105i_CTRL_SET_UP_CONTINUOUSLY_MEASUREMENT

#define PM2105i_MEASURING_TIME             180
#define PM2105i_CALIBRATION_COEFFICIENT    70
#define PM2105i_FRAME_HEADER               0x16

// Status
#define PM2105i_STATUS_CLOSE               0x1
#define PM2105i_STATUS_UNDER_MEASURING     0x2
#define PM2105i_STATUS_FAILED              0x7
#define PM2105i_STATUS_DATA_STABLE         0x80

 #define PM2105i_DEBUG

  #define AQI_BASE_US             0
  #define AQI_BASE_CN             1

  #define AQI_DATA                0
  #define AQI_LEVEL               1
  #define AQI_MAIN_POLLUTANT      2

  #define POLLUTANT_PM2_5         0
  #define POLLUTANT_PM10          1

  const uint16_t AQIindex[8][6] = {
      {0,     0,      0,      0,      0,      0},
      {120,   350,    540,    500,    500,    0},//35 50
      {354,   750,    1540,   1500,   1000,   1},//75 150
      {554,   1150,   2540,   2500,   1500,   2},//115 250
      {1504,  1500,   3540,   3500,   2000,   3},//150 350
      {2504,  2500,   4240,   4200,   3000,   4},//250 420
      {3504,  3500,   5040,   5000,   4000,   5},//350 500
      {5004,  5000,   6040,   6000,   5000,   5}//500 600C I
  };
  

class PM2105i {

public:
  void begin();
  void command();
  uint8_t read();

  uint8_t     status;
  uint16_t    measuring_mode;
  uint16_t    calibration_coefficient;
  uint16_t    pm1p0_grimm;
  uint16_t    pm2p5_grimm;
  uint16_t    pm10_grimm;
  uint16_t    pm1p0_tsi;
  uint16_t    pm2p5_tsi;
  uint16_t    pm10_tsi;
  uint16_t    number_of_0p3_um;
  uint16_t    number_of_0p5_um;
  uint16_t    number_of_1_um;
  uint16_t    number_of_2p5_um;
  uint16_t    number_of_5_um;
  uint16_t    number_of_10_um;
  uint16_t    AQI;
  uint8_t     AQILevel;
  uint8_t     MainPollutedn;

  String      MainPolluted;
  String      AQILevel_CN;

  uint16_t AQIBUFFER[2][3] = {{0,0,0},{0,0,0}};

private:
  uint8_t _buffer[32];

  uint16_t getAQI(uint8_t _base);
  uint8_t getAQILevel(uint8_t _base);
  String getMainPollu(uint8_t _base);
  uint8_t getMainPolluted(uint8_t _base);
  void parseAQI();
  String level2cn(uint16_t level);
};

#endif
