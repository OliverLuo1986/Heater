#include <SPI.h>
#include <Wire.h>
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include "max6675.h"
#include "Heater.h"
#include "Ticker.h"
#include "Oled.h"
#include "EC11.h"
#include "ui.h"
#include "eeprom_flash.h"
#include "reflow.h"
#include <Adafruit_NeoPixel.h>
#include "B3950.h"

int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

Heater pwm;
Ticker heaterTick;
Ticker adcTick;
Ticker touchTick;
Ticker ledTick;
Reflow reflow;

#define PIN        13
#define NUMPIXELS   6
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int adc_vol1;
int adc_vol2;
int vol1, vol2;
int res_temp,tempture;

int touch_mode, touch_fan, touch_power;

float temp;

float getTemp(){
  return temp;
}

void heater_handle(){
  temp = thermocouple.readCelsius();
  pwm.handle();
}

void heaterInit(){
  pwm.setTargetTemp(80.0);
  pwm.setGetTempCallback(getTemp);
  pwm.setPWMpin(14,12);
  pwm.setKp(17);
  pwm.setKi(0.08);
  //pwm.setKp(22);
  //pwm.setKi(0.2);  
  pwm.setKd(20);
  pwm.init(); 

  heaterTick.attach_ms(400, heater_handle);
}

void reflow_init()
{
  reflow.init(&pwm);
}

void adcTask(){
 uint8_t j = 0;
 
 adc_vol1 = analogRead(37);
 vol1 = 3640*adc_vol1/4095;

 res_temp = 3294*100*10/vol1-100*10;
 
  for(j=0;j<140;j++)
  {
    if(res_temp>B3950[j])
      break;
  }
  tempture = j-20-1; 
  
  printf("  BoardTemp:%d\n", tempture);
}

void ledTask(){
 static uint8_t i = 0;
#if 0
 pixels.setPixelColor(0, pixels.Color(i, i, i));
 pixels.show(); 
 i++;
 if(i>100)
  i=0;
#else
 if(temp < 40)
    pixels.setPixelColor(0, pixels.Color(0, 0, 10));
 else
 {
    if(temp > 100)
      pixels.setPixelColor(0, pixels.Color(100, 0, 0));
    else
      pixels.setPixelColor(0, pixels.Color(temp-40, 0, 0));
 }
 pixels.show();   

#endif
}

void touchTask(){
  static uint16_t i=0,j=0,k=0;

  touch_mode = touchRead(T7);
  touch_fan = touchRead(T8);
  touch_power = touchRead(T9); 

  if(touch_power<35)
  {
    i++;
    if(i==5)
    {
      ui.onoff_button_down();
    }
  }
  else
  {
    i = 0;
  }

  if(touch_fan<35){
    j++;
    if(j==5)
    {
      ui.fan_button_down();
    }      
  }
  else{ 
    j = 0;
  }

  if(touch_mode<40){
    k++;
    if(k==5)
    {
      pwm.temp_mode = !pwm.temp_mode;
    }      
  }
  else{ 
    k = 0;
  }  
}

void setup()
{
	Serial.begin(115200);

  heaterInit();
  eeprom.read_all_data();

  oled.init();

  ec11.begin(26,35, 23, ui_key_callb);
  ec11.speed_up(true);
  ec11.speed_up_max(20);

  ui.page_switch_flg = true;
  ui.init();

  reflow_init();
  delay(1000);

  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

  adcTick.attach_ms(400, adcTask);
  touchTick.attach_ms(100, touchTask);
  ledTick.attach_ms(100, ledTask);

  pixels.begin();
}

void loop()
{
  ui.run_task();
  reflow.task();
  eeprom.write_task();
	delay(50); 
  
  //printf("T7:%d T8:%d T9:%d\n", touch_mode,touch_fan,touch_power);
}
