#include <Adafruit_ADS1X15.h>

#include <Wire.h>

Adafruit_ADS1115 ads; 

void setup(void)
{
Serial.begin(9600);
ads.setGain(GAIN_SIXTEEN); // 16x gain +/- 0.256V 1 bit = 0.125mV 0.0046875mV
ads.begin();
Serial.println("Amps: ");
}
void loop(void)
{
int16_t ads1;

ads1 = ads.readADC_Differential_0_1();

Serial.println(ads1);

float amps = ((float)ads1 * 256.0) / 32768.0;
amps = amps * 1.333; 

Serial.println(amps);

delay(500);
}
