#include "Heater.h"

Heater::Heater(){
	callBack = NULL;
	targetTemp = INVALID_TEMP;
	heat_pwm_pin = -1;
	fan_pwm_pin = -1;
	
	Kp = 1;
	Ki = 0;
	Kd = 0;
	
	diff_sum = 0;
	
	inited = false;
}

bool Heater::init(){
	if(callBack == NULL)
		return false;
	
	if(targetTemp == INVALID_TEMP)
		return false;
	
	if(heat_pwm_pin == -1 || fan_pwm_pin == -1)
		return false;
	
	ledcSetup(0, 30000, 10); //通道0， 30KHz，10位解析度
	ledcAttachPin(heat_pwm_pin, 0); //pin14定义为通道0的输出引脚  	
	
	ledcWrite(0,1023);
	
	
	//ledcSetup(1, 30000, 10); //通道1， 30KHz，10位解析度
	//ledcAttachPin(fan_pwm_pin, 0); //pin14定义为通道0的输出引脚  	
	//ledcWrite(1,0);	
	
	inited = true;
	return true;
}

void Heater::begin(){
	power = true;
	diff_sum = 0;
	last_diff = targetTemp-callBack();
}

void Heater::setTargetTemp(float temp){
	targetTemp = temp;
	temp_buf = temp;
	
	//diff_sum = 0;
}

void Heater::setKp(float kp){
	Kp = kp;
}

void Heater::setKi(float ki){
	Ki = ki;
}

void Heater::setKd(float kd){
	Kd = kd;
}

void Heater::setPWMpin(int heat_pin, int fan_pin){
	heat_pwm_pin = heat_pin;
	fan_pwm_pin = fan_pin;
}

void Heater::temp_set(){
}

void Heater::fan(){
	
}

void Heater::fan_start(){
	fan_power = true;
	digitalWrite(fan_pwm_pin, HIGH);
}

void Heater::fan_stop(){
	fan_power = false;
	digitalWrite(fan_pwm_pin, LOW);
}

void Heater::end(){
	power = false;
	
	ledcWrite(0, 1023);
}


void Heater::setGetTempCallback(getTempCallback cb){
	callBack = cb;
}

float Heater::getCurrentTemp(){
	return currentTemp;
}

void Heater::handle(){
	float diff, diff_diff;
	
	if(inited == false){
		return; 
	}
	
	currentTemp = callBack();
	now_temp = currentTemp;
	targetTemp = temp_buf;
	diff = targetTemp - currentTemp;

	if(power)
	{
		diff_sum += diff;
		diff_diff = diff-last_diff;
		last_diff = diff;
		//if(diff > 0)
		if(1)
		{
			pwmValue = Kp*diff + Ki*diff_sum + Kd*diff_diff;
			
#if 0 //HEATER_DEBUG		
			printf("Kp:%.2f Ki:%.2f Kd:%.2f diff_sum:%.2f diff_diff:%.2f pwmValue:%d \n", 
					Kp, Ki, Kd, diff_sum, diff_diff, pwmValue);
#endif			
			
			if(pwmValue > 1023)
				pwmValue = 1023;
			
			if(pwmValue <= 0)
				pwmValue = 0;
			

			if(PWM_ON_LEVEL == LOW)
				pwmValue = 1023-pwmValue;
			
			
		}
		
		else{
			if(PWM_ON_LEVEL == LOW)
				pwmValue = 1023;
			else
				pwmValue = 0;
		}
		
		ledcWrite(0, pwmValue);
	}
	else
	{
		ledcWrite(0, 1023);
	}
	
#if HEATER_DEBUG	
#if 0
	printf("targetTemp:%.2f, currentTemp:%.2f, diff:%.2f diff_sum:%.2f pwmValue:%d\n",
			targetTemp, currentTemp, diff, diff_sum, pwmValue);
#else	
	printf("targetTemp:%.2f, currentTemp:%.2f",
			targetTemp, currentTemp);			
#endif			
#endif
	
};

