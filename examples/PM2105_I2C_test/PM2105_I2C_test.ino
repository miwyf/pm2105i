#include <pm2105i.h>

// #ifdef PM2008N

PM2105i pm2105i;

void setup() {
#ifdef PM2008N
  // wait for PM2008N to be changed to I2C mode
  delay(10000);
#endif
  pm2105i.begin();
  Serial.begin(9600);
  pm2105i.command();
  delay(1000);
}

void loop() {
  uint8_t ret = pm2105i.read();
  if (ret == 0) {
    Serial.print("PM 1.0 (GRIMM) : ");
    Serial.println(pm2105i.pm1p0_grimm);
    Serial.print("PM 2.5 (GRIMM) : : ");
    Serial.println(pm2105i.pm2p5_grimm);
    Serial.print("PM 10 (GRIMM) : : ");
    Serial.println(pm2105i.pm10_grimm);
/*
    Serial.print("PM 1.0 (TSI) : ");
    Serial.println(pm2105i.pm1p0_tsi);
    Serial.print("PM 2.5 (TSI) : : ");
    Serial.println(pm2105i.pm2p5_tsi);
    Serial.print("PM 10 (TSI) : : ");
    Serial.println(pm2105i.pm10_tsi);
    Serial.print("Number of 0.3 um : ");
    Serial.println(pm2105i.number_of_0p3_um);
    Serial.print("Number of 0.5 um : ");
    Serial.println(pm2105i.number_of_0p5_um);
    Serial.print("Number of 1 um : ");
    Serial.println(pm2105i.number_of_1_um);
    Serial.print("Number of 2.5 um : ");
    Serial.println(pm2105i.number_of_2p5_um);
    Serial.print("Number of 5 um : ");
    Serial.println(pm2105i.number_of_5_um);
    Serial.print("Number of 10 um : ");
    Serial.println(pm2105i.number_of_10_um);
*/

  }
  delay(1000);
}
