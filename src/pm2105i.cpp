#include "pm2105i.h"

/**
 * Start PM2105 I2C library
 */
void PM2105i::begin() {
  // Start the Wire library
  Wire.begin();
}

/**
 * Send command data
 */
void PM2105i::command() {
  _buffer[0] = PM2105i_FRAME_HEADER;
  _buffer[1] = 0x7; // frame length
  _buffer[2] = PM2105i_CONTROL_MODE;

  uint16_t data;

  switch (PM2105i_CONTROL_MODE) {
    case PM2105i_CTRL_SET_UP_CONTINUOUSLY_MEASUREMENT:
      data = 0xFFFF;
      break;
    case PM2105i_CTRL_SET_UP_CALIBRATION_COEFFICIENT:
      data = PM2105i_CALIBRATION_COEFFICIENT;
      break;
    default:
      data = PM2105i_MEASURING_TIME;
      break;
  }

  _buffer[3] = data >> 8;
  _buffer[4] = data & 0xFF;
  _buffer[5] = 0; // Reserved

  // Calculate checksum
  _buffer[6] = _buffer[0];

  for (uint8_t i = 1; i < 6; i++) {
    _buffer[6] ^= _buffer[i];
  }

  Wire.beginTransmission(PM2105i_ADDRESS);
  Wire.write(_buffer, 7);
  Wire.endTransmission();
}

/**
 * Read PM2105 value
 * @return {@code 0} Reading PM2105 value succeeded
 *         {@code 1} Buffer(index) is short
 *         {@code 2} Frame header is different
 *         {@code 3} Frame length is not 22
 *         {@code 4} Checksum is wrong
 */
uint8_t PM2105i::read() {
  Wire.requestFrom(PM2105i_ADDRESS, 22);
  uint8_t idx = 0;

  while (Wire.available()) { // slave may send less than requested
    uint8_t b = Wire.read();
    _buffer[idx++] = b;
    if (idx == 22) break;
  }

  if (idx < 22) {
#ifdef PM2105i_DEBUG
    Serial.println("PM2105i::read : buffer is short!");
#endif
    return 1;
  }

  // Check frame header
  if (_buffer[0] != PM2105i_FRAME_HEADER) {
#ifdef PM2105i_DEBUG
    Serial.print("PM2105i::read : frame header is different ");
    Serial.println(_buffer[0], HEX);
#endif
    return 2;
  }

  // Check frame length
  if (_buffer[1] != 22) { 
#ifdef PM2105i_DEBUG
    Serial.println("PM2105i::read : frame length is not 22");
    Serial.println(_buffer[1], HEX);
#endif
    return 3;
  }

  // Check checksum
  uint8_t check_code = _buffer[0];

  for (uint8_t i = 1; i < 21; i++) {
    check_code ^= _buffer[i];
#ifdef PM2105i_DEBUG
    Serial.println(_buffer[i], HEX);
#endif
  }

  if (_buffer[21] != check_code) {
#ifdef PM2105i_DEBUG
    Serial.print("PM2105i::read failed : check code is different - _buffer[21] : ");
    Serial.print(_buffer[21], HEX);
    Serial.print(", check_code : ");
    Serial.println(check_code, HEX);
#endif
    return 4;
  }

  /// Status
  status = _buffer[2];
  measuring_mode = (_buffer[9] << 8) + _buffer[10];
  calibration_coefficient = (_buffer[11] << 8) + _buffer[12];
  pm1p0_grimm = (_buffer[3] << 8) + _buffer[4];
  pm2p5_grimm = (_buffer[5] << 8) + _buffer[6];
  pm10_grimm = (_buffer[7] << 8) + _buffer[8];
  pm1p0_tsi = (_buffer[13] << 8) + _buffer[14];
  pm2p5_tsi= (_buffer[15] << 8) + _buffer[16];
  pm10_tsi= (_buffer[17] << 8) + _buffer[18];
  number_of_0p3_um= (_buffer[19] << 8) + _buffer[20];
  number_of_0p5_um= (_buffer[21] << 8) + _buffer[22];
  number_of_1_um= (_buffer[23] << 8) + _buffer[24];
  number_of_2p5_um= (_buffer[25] << 8) + _buffer[26];
  number_of_5_um= (_buffer[27] << 8) + _buffer[28];
  number_of_10_um= (_buffer[29] << 8) + _buffer[30];
  parseAQI();
  AQI = getAQI(AQI_BASE_CN);
  MainPollutedn = getMainPolluted(AQI_BASE_CN);
  MainPolluted = getMainPollu(AQI_BASE_CN);
  AQILevel = getAQILevel(AQI_BASE_CN);
  AQILevel_CN = level2cn(AQILevel);

  return 0;
}

/**
 * level to cn
 */ 
String PM2105i::level2cn(uint16_t level)
{ 
   switch(level){
     case 0:  
       return "优";
       break;
     case 1: 
       return "良";
     break;
     case 2:  
       return "轻度污染";
     break;
     case 3: 
       return "中度污染";
     break;
     case 4: 
       return "重度污染";
     break;
     case 5: 
       return "严重污染";
     break;
     default:
       return "优";
       break;
   }
}
/**
 * get AQI
 */ 
uint16_t PM2105i::getAQI(uint8_t _base) {
    if (_base >= AQI_BASE_US && _base <= AQI_BASE_CN) {
        return AQIBUFFER[_base][AQI_DATA];
    }
    else {
        return AQIBUFFER[AQI_BASE_US][AQI_DATA];
    }
}
/**
 * get AQI level
 */ 
uint8_t PM2105i::getAQILevel(uint8_t _base) {
    if (_base >= AQI_BASE_US && _base <= AQI_BASE_CN) {
        return AQIBUFFER[_base][AQI_LEVEL];
    }
    else {
        return AQIBUFFER[AQI_BASE_US][AQI_LEVEL];
    }
}
/**
 * get main pollu
 */ 
String PM2105i::getMainPollu(uint8_t _base) {
    if (_base >= AQI_BASE_US && _base <= AQI_BASE_CN) {
        return AQIBUFFER[_base][AQI_MAIN_POLLUTANT] ? "PM10" : "PM2.5";
    }
    else {
        return AQIBUFFER[AQI_BASE_US][AQI_MAIN_POLLUTANT] ? "PM10" : "PM2.5";
    }
}
/**
 * 0: PM2.5, 1: PM10
 */ 
uint8_t PM2105i::getMainPolluted(uint8_t _base) {
    if (_base >= AQI_BASE_US && _base <= AQI_BASE_CN) {
        return AQIBUFFER[_base][AQI_MAIN_POLLUTANT] ? 1 : 0;
    }
    else {
        return AQIBUFFER[AQI_BASE_US][AQI_MAIN_POLLUTANT] ? 1 : 0;
    }
}
/**
 * parse PM to AQI
 */ 
void PM2105i::parseAQI() {
    uint16_t AQI25, AQI100, color;
    for (uint8_t Bnum = 0; Bnum < 2; Bnum++) {
    // uint8_t Bnum = 0;
        AQI25 = 0;
        AQI100 = 0;
        for (uint8_t Inum = 1; Inum < 8; Inum++) {
            if (pm2p5_grimm*10 <= AQIindex[Inum][0+Bnum]) {
                // IOT_DEBUG_PRINT4(F("Inum: "), Inum, F("Bnum: "), Bnum);
                // IOT_DEBUG_PRINT2(F("AQIindex[Inum][0+Bnum]: "), AQIindex[Inum][0+Bnum]);
                AQI25 = ((AQIindex[Inum][4] - AQIindex[Inum-1][4])*(pm2p5_grimm*10 - AQIindex[Inum-1][0+Bnum]) / (AQIindex[Inum][0+Bnum] - AQIindex[Inum - 1][0+Bnum]) + AQIindex[Inum-1][4])/10;
                color = AQIindex[Inum][5];
                break;
            }
            if (Inum == 7) {
                AQI25 = 500;
                color = 5;
            }
        }
        for (uint8_t Inum = 1; Inum < 8; Inum++) {
            if (pm10_grimm*10 <= AQIindex[Inum][2+Bnum]) {
                // IOT_DEBUG_PRINT4(F("Inum: "), Inum, F("Bnum: "), Bnum);
                // IOT_DEBUG_PRINT2(F("AQIindex[Inum][0+Bnum]: "), AQIindex[Inum][0+Bnum]);
                AQI100 = ((AQIindex[Inum][4] - AQIindex[Inum-1][4])*(pm10_grimm*10 - AQIindex[Inum-1][2+Bnum]) / (AQIindex[Inum][2+Bnum] - AQIindex[Inum - 1][2+Bnum]) + AQIindex[Inum-1][4])/10;
                
                // IOT_DEBUG_PRINT4(F("AQI25: "), AQI25, F("  AQI100: "), AQI100);
                if(AQI25 >= AQI100) {
                    // return String(AQI25);
                    AQIBUFFER[Bnum][AQI_DATA] = AQI25;
                    AQIBUFFER[Bnum][AQI_LEVEL] = color;
                    AQIBUFFER[Bnum][AQI_MAIN_POLLUTANT] = POLLUTANT_PM2_5;
                    break;
                }
                else {
                    AQIBUFFER[Bnum][AQI_DATA] = AQI100;
                    color = AQIindex[Inum][5];
                    AQIBUFFER[Bnum][AQI_LEVEL] = color;
                    AQIBUFFER[Bnum][AQI_MAIN_POLLUTANT] = POLLUTANT_PM10;
                    break;
                    // return String(AQI100);
                }
            }
            if (Inum == 7) {
                AQIBUFFER[Bnum][0] = 500;
                AQIBUFFER[Bnum][1] = 5;
            }
        }
    }
}
