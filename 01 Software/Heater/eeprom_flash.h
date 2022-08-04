#ifndef _eeprom_flash_H_
#define _eeprom_flash_H_

#include <EEPROM.h>
#include <Arduino.h>



#define eeprom_size 4096


#define first_download_add 2432 //初次烧录固件后判定初始化设置参数
#define set_temp_low_add 2433 //预设温度值低八位
#define set_temp_high_add 2434 //预设温度高八位
#define set_temp_mode_add 2435  //温度模式 0：回流焊 1：恒温
#define miot_able_add 2436  //是否启用接入小爱 0：启用 1：不启用
#define temp_mode1_time_low_add 2437 //恒温时长低八位
#define temp_mode1_time_high_add 2438 //恒温时长高八位
#define set_oled_light_add 2439 //屏幕亮度
#define temp0_data_start_add 2440 //回流参数

#define blinker_id_add 2450 //...点灯注册设备号

#define hotbed_max_temp_low_add 2470 //设备最高温度低八位
#define hotbed_max_temp_high_add 2471 //设备最高温度高八位

#define adc_max_temp_low_add 2472 //设备读取最高温度低八位
#define adc_max_temp_high_add 2473 //设备读取最高温度高八位


class eeprom_flash{
    public:

    void data_init();//数据初始化
    void read_all_data();//读取所有数据
    void write(int add,uint8_t dat);//写一个字节
    uint8_t read(int add);//读一个字节
    void write_task();
    void write_str(int add,String str);//写字符串

    uint8_t write_flg = 0;
    uint8_t write_t = 21;

};


extern eeprom_flash eeprom;






#endif
