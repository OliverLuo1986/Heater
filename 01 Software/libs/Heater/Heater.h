#ifndef _HEATER_H_
#define _HEATER_H_

#include <Arduino.h>
#include <Ticker.h>

#define TICK_MS_DEFAULT		(1000)  // 1S
#define INVALID_TEMP		(500)

#define PWM_ON_LEVEL 		(LOW)

#define HEATER_DEBUG		(1)

#define PWM_IO 14
#define FAN_IO 15





#define OFF 0
#define ON 1

#define Re_So 0
#define Co_Temp 1

typedef enum{
	E_TEMP_MODE1_REFLOW = 0,
	E_TEMP_MODE2_CONST_TEMP,
}E_TEMP_MODE;



typedef float (*getTempCallback)(void) ;


class Heater{
	public:
	
	Heater();
	void setTargetTemp(float temp);
	
	void setKp(float Kp);
	void setKi(float Ki);
	void setKd(float Kd);
	
	void setPWMpin(int heat_pin, int fan_pin);
	
	void setGetTempCallback(getTempCallback cb);
	
	float getCurrentTemp();
	
	bool init();
	void begin(void);
	
	void handle(void);

#if 1	
    
    void end();
    void temp_set();
    void fan();
	void fan_start();
	void fan_stop();
    bool power = OFF;
    bool sleep_flg = OFF;
	bool fan_power = false;
    
    int16_t temp_buf = 41;//设置温度

    uint8_t temp_mode = 1; //0 回流 1：恒温

    int16_t temp_mode1_time = 10;//恒温时间

    uint8_t percent = 0;

    bool temp_reached_flg = false;

    uint8_t backflow_working_state = 0;
    // int16_t backflow_temp_buf = 0;
    int16_t backflow_temp_tmp = 0;
	
	int16_t now_temp=31;
#endif	
	private:
	float currentTemp;
	float targetTemp;
	
	float Kp;
	float Ki;
	float Kd;
	
	float diff_sum;
	float last_diff;
	float diff_diff;
	
	int heat_pwm_pin;
	int pwmValue;
	
	int fan_pwm_pin;
	
	bool inited;
	
	getTempCallback callBack;
};


#endif // _HEATER_H_