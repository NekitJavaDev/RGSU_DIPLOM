#include <openGLCD.h>		//#include <stdlib.h>
#include <troyka-imu.h>		// библиотека для работы с модулями IMU
#include <Wire.h>			// библиотека для работы I²C

float time;
float old_lightness = 300;
float old_audio = 60;

int step;

Barometer barometer;

void setup()
{
  Serial.begin(9600);
  Serial.println(goodness(100, 200, 300, 200));
  Serial.println(goodness(100, 200, 300, 100));
  Serial.println(goodness(100, 200, 300, 150));
  Serial.println(goodness(100, 200, 300, 230));
  Serial.println(goodness(100, 200, 300, 295));
  
  GLCD.Init();

  GLCD.SelectFont(System5x7);
  step = GLCD.Height / 4;
  
  barometer.begin();
}

void loop()
{
  time += 0.1;

  float temperature = barometer.readTemperatureC(); // C
  
  float pressure = barometer.readPressureMillibars() * 0.7501; // mm bar
  
  float lightness = 800 - analogRead(4); // lux
  lightness = exp(float(lightness)/80.0);
  lightness = old_lightness + (lightness - old_lightness) * 0.1;
  old_lightness = lightness;
  
  float audio = min(256, analogRead(5)-20); // db
  audio = old_audio + (audio - old_audio) * 0.1;
  old_audio = audio;
  
  char buffT[10]; // temperature
  char buffP[10]; // pressure
  char buffL[10]; // lightness
  char buffA[10]; // audio  
  //sprintf(buff, "hello: %f", temperature);
  
  dtostrf(temperature, 3, 0, buffT);  //4 is mininum width, 6 is precision
  dtostrf(pressure, 3, 0, buffP);
  dtostrf(lightness, 3, 0, buffL);
  dtostrf(audio, 3, 0, buffA);  
  //Serial.println(analogRead(5));

  if (time > 5) {
    time = 0;
    
    GLCD.SelectFont(System5x7);
    
    GLCD.DrawString("17-29", 0, 0, eraseNONE);
    GLCD.DrawString("740-755", 0, step, eraseNONE);
    GLCD.DrawString("250-1k", 0, step*2, eraseNONE);
    GLCD.DrawString("10-100", 0, step*3, eraseNONE);
    
    int s = 45;
    GLCD.DrawString(buffT, s, 0, eraseNONE);
    GLCD.DrawString(buffP, s, step, eraseNONE);
    GLCD.DrawString(buffL, s-2, step*2, eraseNONE);
    GLCD.DrawString(buffA, s, step*3, eraseNONE);
    
    s = 65;
    GLCD.DrawString("C", s, 0, eraseNONE);
    GLCD.DrawString("mm", s, step, eraseNONE);
    GLCD.DrawString("lu", s, step*2, eraseNONE);
    GLCD.DrawString("db", s, step*3, eraseNONE);
    
    // mark renderings
    GLCD.DrawString("/ 100", 90, 40, eraseNONE);
    
    GLCD.SelectFont(fixednums7x15);
    
    float mark = (goodness(17, 24, 29, temperature) +
                  goodness(740, 747, 755, pressure) +
                  goodness(250, 500, 1000, lightness) +
                  goodness(10, 55, 100, audio)) / 4 * 100;
    mark = fromTo(0, mark, 100);
    char markstring[10];    
    //Serial.println(mark);
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(pressure);
    Serial.print(",");
    Serial.print(lightness);
    Serial.print(",");
    Serial.print(audio);
    Serial.println();
    dtostrf(mark, 3, 0, markstring);  //4 is mininum width, 6 is precision
    GLCD.DrawString(markstring, 90, 20, eraseNONE);
  }
  //GLCD.DrawRect(8, 8, 128 - 16, 64 - 16, PIXEL_ON); // border  
  int s = 76;
  GLCD.DrawHBarGraph(0, 8, s,  5, 1, 17, 29, fromTo(17, temperature, 29));
  GLCD.DrawHBarGraph(0, step+8, s, 5, 1, 740, 755, fromTo(740, pressure, 755));
  GLCD.DrawHBarGraph(0, step*2+8, s, 5, 1, 250, 1000, fromTo(250, lightness, 1000));
  GLCD.DrawHBarGraph(0, step*3+8, s, 5, 1, 10, 100, fromTo(10, audio, 100));
  
  GLCD.DrawRect(80, 0, GLCD.Right - 80, GLCD.Height, PIXEL_ON); // border
  //delay(100);
}

float goodness (float min, float good, float max, float current) {
  float modulo = max - min;
  good = (good - min) / modulo;
  max = (max - min) / modulo;
  current = (current - min) / modulo;
  if (current < good) {
    return current * 1/good;
  } else {
	return 1 - (current-good) * 1/good;
  }
}

float fromTo (float from, float what, float to) {
	return min(to, max(from, what));
}

float mapTo (float from, float fmin, float fmax, float tmin, float tmax) {
	return (from - fmin) / (fmax - fmin) * (tmax - tmin) + tmin;
}

