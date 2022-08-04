#ifndef _REFLOW_H_
#define _REFLOW_H_

#include <Arduino.h>
#include <Heater.h>

#define REFLOW_DEBUG	(1)

typedef enum {
	REFLOW_STATE1_PRHEAT = 0,
	REFLOW_STATE2_SOAK,
	REFLOW_STATE3_RELOW,
	REFLOW_STATE3_1_RELOW,
	REFLOW_STATE4_COOLING,
	REFLOW_STATE5_END,
}E_REFLOW_STATE;

typedef void (*state_change_callback)(void * arg,E_REFLOW_STATE state);

class Reflow{
	public:
	
	Reflow();
	
	bool init(Heater *ht);
	
	void start();
	void stop();
	bool is_start();
	
	void set_soak_temp(uint16_t temp);
	void set_soak_time(uint8_t ms);
	
	uint16_t get_soak_temp();
	uint8_t get_soak_time();
	
	void set_reflow_temp(uint16_t temp);
	void set_reflow_time(uint8_t ms);
	
	uint16_t get_reflow_temp();
	uint8_t get_reflow_time();	
	
	void task();
	
	E_REFLOW_STATE get_state();
	
	int8_t get_time_left();
	
	void set_state_change_callback(state_change_callback cb, void * arg);
	
	private:
	bool start_work;
	Heater *heater;
	bool inited=false;
	E_REFLOW_STATE reflow_state;
	uint16_t soak_temp;
	uint8_t soak_time;
	uint16_t reflow_temp;
	uint8_t reflow_time;
	int8_t second_count;
	
	long start_mills;
	
	state_change_callback call_back;
	
	void *priv;
};



#endif 