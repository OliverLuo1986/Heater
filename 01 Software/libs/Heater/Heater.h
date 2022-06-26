#ifndef _HEATER_H_
#define _HEATER_H_

#include <Arduino.h>
#include <Ticker.h>

#define TICK_MS_DEFAULT		(1000)  // 1S
#define INVALID_TEMP		(500)

#define PWM_ON_LEVEL 		(LOW)

#define HEATER_DEBUG		(1)



typedef float (*getTempCallback)(void) ;


class Heater{
	public:
	
	Heater();
	void setTargetTemp(float temp);
	
	void setKp(float Kp);
	void setKi(float Ki);
	void setKd(float Kd);
	
	void setPWMpin(int pin);
	
	void setGetTempCallback(getTempCallback cb);
	
	bool begin(void);
	
	void handle(void);
	
	private:
	float currentTemp;
	float targetTemp;
	
	float Kp;
	float Ki;
	float Kd;
	
	int pwmPin;
	int pwmValue;
	
	bool inited;
	
	getTempCallback callBack;
};


#endif // _HEATER_H_