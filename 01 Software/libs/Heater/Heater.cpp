#include "Heater.h"

Heater::Heater(){
	callBack = NULL;
	targetTemp = INVALID_TEMP;
	pwmPin = -1;
	
	Kp = 1;
	Ki = 0;
	Kd = 0;
	
	inited = false;
}

bool Heater::begin(){
	if(callBack == NULL)
		return false;
	
	if(targetTemp == INVALID_TEMP)
		return false;
	
	if(pwmPin == -1)
		return false;
	
	ledcSetup(0, 100, 10); //通道0， 35KHz，10位解析度
	ledcAttachPin(pwmPin, 0); //pin14定义为通道0的输出引脚  	
	
	inited = true;
	return true;
}

void Heater::setTargetTemp(float temp){
	targetTemp = temp;
}

void Heater::setKp(float kp){
	Kp = kp;
}

void Heater::setKi(float ki){
	Kp = ki;
}

void Heater::setKd(float kd){
	Kp = kd;
}

void Heater::setPWMpin(int pin){
	pwmPin = pin;
}



void Heater::setGetTempCallback(getTempCallback cb){
	callBack = cb;
}

void Heater::handle(){
	float diff;
	
	if(inited == false){
		return; 
	}
	
	currentTemp = callBack();
	diff = targetTemp - currentTemp;

	//printf("%.2f\n", currentTemp);
	if(diff > 0){
		pwmValue = Kp * diff;
		
		if(pwmValue > 1023)
			pwmValue = 1023;
		

		if(PWM_ON_LEVEL == LOW)
			pwmValue = 1023-pwmValue;
		
#if 0 //HEATER_DEBUG		
		printf("pwmValue:%d\n", pwmValue);
#endif		
		
		
	}
	else{
		if(PWM_ON_LEVEL == LOW)
			pwmValue = 1023;
		else
			pwmValue = 0;
	}
	
	ledcWrite(0, pwmValue);
	
#if HEATER_DEBUG	
	printf("targetTemp:%.2f, currentTemp:%.2f, diff:%.2f pwmValue:%d\n",
			targetTemp, currentTemp, diff,pwmValue);
#endif
	
};

