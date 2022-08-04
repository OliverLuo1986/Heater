#include "reflow.h"


Reflow::Reflow(){
	
	start_work = false;
}

bool Reflow::init(Heater *ht){
	heater = ht;
	
	reflow_state = REFLOW_STATE1_PRHEAT;
	start_work = false;
}

void Reflow::start(){
	
	start_mills = millis();
	reflow_state = REFLOW_STATE1_PRHEAT;
	start_work = true;
	
	if(heater->getCurrentTemp() < soak_temp){
		heater->setTargetTemp(soak_temp);
	}
	
	heater->begin();
	
#if REFLOW_DEBUG	
	Serial.printf("reflow start...\n");
#endif	
}

void Reflow::stop(){
	heater->end();
	start_work = false;
}

void Reflow::set_soak_temp(uint16_t temp){
	
	soak_temp = temp;
	
#if REFLOW_DEBUG	
	Serial.printf("set soak temp:%d\n", temp);
#endif	
}

void Reflow::set_soak_time(uint8_t s){
	soak_time = s;
}

void Reflow::set_reflow_temp(uint16_t temp){
	reflow_temp = temp;
	
#if REFLOW_DEBUG	
	Serial.printf("set reflow temp:%d\n", temp);
#endif	
}

void Reflow::set_reflow_time(uint8_t s){
	reflow_time = s;
}

uint16_t Reflow::get_soak_temp(){
	return soak_temp;
}

uint8_t Reflow::get_soak_time(){
	return soak_time;
}

uint16_t Reflow::get_reflow_temp(){
	return reflow_temp;
}

uint8_t Reflow::get_reflow_time(){
	return reflow_time;
}

E_REFLOW_STATE Reflow::get_state(){
	return reflow_state;
}

bool Reflow::is_start(){
	
	return start_work;
}

int8_t Reflow::get_time_left(){
	return second_count;
}

void Reflow::set_state_change_callback(state_change_callback cb,void * arg){
	call_back = cb; 
	priv = arg;
}

void Reflow::task(){
	long ms;
	
	if(!start_work){
		return;		
	}
	
	switch(reflow_state){
		
	case REFLOW_STATE1_PRHEAT:{
		
		if(heater->getCurrentTemp() >= soak_temp){
			
			second_count = soak_time;
			reflow_state = (E_REFLOW_STATE)(reflow_state + 1);
			
			start_mills = millis();
			
			call_back(priv, reflow_state);
#if REFLOW_DEBUG			
			Serial.printf("reflow state -> soak\n");
#endif			
		}
		break;
	}
	case REFLOW_STATE2_SOAK:{
		
		second_count = soak_time - (millis()-start_mills)/1000;
		
		if(second_count <= 0){
			
			heater->setTargetTemp(reflow_temp);
			reflow_state = (E_REFLOW_STATE)(reflow_state + 1);
			
			call_back(priv, reflow_state);
			
#if REFLOW_DEBUG			
			Serial.printf("reflow state -> reflow\n");
#endif			
		}
		break;
	}
	case REFLOW_STATE3_RELOW:{
		
		if(heater->getCurrentTemp() >= reflow_temp){
			
			second_count = reflow_time;
			start_mills = millis();
			reflow_state = (E_REFLOW_STATE)(reflow_state + 1);
			
			call_back(priv, reflow_state);
			
#if REFLOW_DEBUG			
			Serial.printf("reflow state -> reflow 1\n");
#endif			
		}
		break;
	}
	case REFLOW_STATE3_1_RELOW:{

		second_count = reflow_time - (millis()-start_mills)/1000;
		
		if(second_count <= 0){
			
			reflow_state = (E_REFLOW_STATE)(reflow_state + 1);
			heater->end();
			heater->fan_start();
			
			call_back(priv, reflow_state);
			
#if REFLOW_DEBUG			
			Serial.printf("reflow state -> colling\n");
#endif			
		}		
		break;
	}
	case REFLOW_STATE4_COOLING:{
		if(heater->getCurrentTemp() < 50)
		{
			start_work = false;
			heater->fan_stop();
			reflow_state = (E_REFLOW_STATE)(reflow_state + 1);
			
			call_back(priv, reflow_state);
		}
		break;
	}
	case REFLOW_STATE5_END:{
		break;
	}
	default:break;
	}
	
	
	
	
}
