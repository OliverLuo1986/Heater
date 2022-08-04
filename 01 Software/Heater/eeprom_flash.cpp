#include "eeprom_flash.h"
#include "Heater.h"
#include "ui.h"
#include "Oled.h"
//#include "User.h"
extern Heater pwm;


eeprom_flash eeprom;

void eeprom_flash::data_init()
{
    // EEPROM初始化数据

    pwm.temp_buf = 100;
    pwm.temp_mode = 0;
    //miot.miot_able = 0;
    pwm.temp_mode1_time = 10;
    ui.oled_light = 127;
    //adc.adc_max_temp = 255;
    //adc.hotbed_max_temp = 255;

    write_flg = 2;

    for (int x = first_download_add; x < eeprom_size; x++)
        EEPROM.write(x, 0);
}
void eeprom_flash::read_all_data()
{

    ec11.int_close();
    EEPROM.begin(eeprom_size);
    if (EEPROM.read(first_download_add))
    {
        data_init();
    }
    else
    {
        pwm.temp_buf = ((uint16_t)(EEPROM.read(set_temp_high_add) << 8)) | EEPROM.read(set_temp_low_add);
        pwm.temp_mode = EEPROM.read(set_temp_mode_add);
        //miot.miot_able = EEPROM.read(miot_able_add);
        pwm.temp_mode1_time = ((uint16_t)(EEPROM.read(temp_mode1_time_high_add) << 8)) | EEPROM.read(temp_mode1_time_low_add);
        ui.oled_light = EEPROM.read(set_oled_light_add);
        curve_temp_buf[0] = (EEPROM.read(temp0_data_start_add) * 256) + EEPROM.read(temp0_data_start_add + 1);
        curve_temp_buf[1] = (EEPROM.read(temp0_data_start_add + 2) * 256) + EEPROM.read(temp0_data_start_add + 3);
        curve_temp_buf[2] = (EEPROM.read(temp0_data_start_add + 4) * 256) + EEPROM.read(temp0_data_start_add + 5);
        curve_temp_buf[3] = (EEPROM.read(temp0_data_start_add + 6) * 256) + EEPROM.read(temp0_data_start_add + 7);
        char tmp;
#if 0		
        for (int x = blinker_id_add; x < eeprom_size; x++)
        {
            tmp = EEPROM.read(x);
            if (tmp != 0)
                wifima.blinker_id += tmp;
            else
                break;
        }
        Serial.print("Blinker ID:");
        Serial.println(wifima.blinker_id);
        adc.hotbed_max_temp = ((uint16_t)EEPROM.read(hotbed_max_temp_high_add) << 8) | EEPROM.read(hotbed_max_temp_low_add);
        if (adc.hotbed_max_temp > 270 || adc.hotbed_max_temp < 240)
        {
            adc.hotbed_max_temp = 255;
            EEPROM.write(hotbed_max_temp_low_add, adc.hotbed_max_temp & 0xff);
            EEPROM.write(hotbed_max_temp_high_add, adc.hotbed_max_temp >> 8);
        }
        adc.adc_max_temp = ((uint16_t)EEPROM.read(adc_max_temp_high_add) << 8) | EEPROM.read(adc_max_temp_low_add);
        if (adc.adc_max_temp > 400 || adc.adc_max_temp < 190)
        {
            adc.adc_max_temp = 255;
            EEPROM.write(adc_max_temp_low_add, adc.adc_max_temp & 0xff);
            EEPROM.write(adc_max_temp_high_add, adc.adc_max_temp >> 8);
        }
#endif		
    }
    EEPROM.commit();
    EEPROM.end();

    ec11.int_work();
    oled.light(ui.oled_light);
}
uint8_t eeprom_flash::read(int add)
{
    uint8_t x;
    EEPROM.begin(eeprom_size);
    x = EEPROM.read(add);
    EEPROM.end();
    return x;
}
void eeprom_flash::write(int add, uint8_t dat)
{

    EEPROM.begin(eeprom_size);
    EEPROM.write(add, dat);
    EEPROM.end();
}

void eeprom_flash::write_task()
{
    if (write_flg == 2)
    {
        write_flg = 0;
        ec11.int_close();
        Serial.println("EE WRITE");

        EEPROM.begin(eeprom_size);

        EEPROM.write(set_temp_low_add, pwm.temp_buf & 0xff);
        EEPROM.write(set_temp_high_add, pwm.temp_buf >> 8);

        EEPROM.write(set_temp_mode_add, pwm.temp_mode);
#if 0
        EEPROM.write(miot_able_add, miot.miot_able);
#endif
        EEPROM.write(temp_mode1_time_low_add, pwm.temp_mode1_time & 0xff);
        EEPROM.write(temp_mode1_time_high_add, pwm.temp_mode1_time >> 8);

        EEPROM.write(set_oled_light_add, ui.oled_light & 0xff);

        EEPROM.write(temp0_data_start_add, curve_temp_buf[0] >> 8);
        EEPROM.write(temp0_data_start_add + 1, curve_temp_buf[0] & 0xff);

        EEPROM.write(temp0_data_start_add + 2, curve_temp_buf[1] >> 8);
        EEPROM.write(temp0_data_start_add + 3, curve_temp_buf[1] & 0xff);

        EEPROM.write(temp0_data_start_add + 4, curve_temp_buf[2] >> 8);
        EEPROM.write(temp0_data_start_add + 5, curve_temp_buf[2] & 0xff);

        EEPROM.write(temp0_data_start_add + 6, curve_temp_buf[3] >> 8);
        EEPROM.write(temp0_data_start_add + 7, curve_temp_buf[3] & 0xff);
#if 0
        EEPROM.write(hotbed_max_temp_low_add, adc.hotbed_max_temp & 0xff);
        EEPROM.write(hotbed_max_temp_high_add, adc.hotbed_max_temp >> 8);

        EEPROM.write(adc_max_temp_low_add, adc.adc_max_temp & 0xff);
        EEPROM.write(adc_max_temp_high_add, adc.adc_max_temp >> 8);
#endif
        EEPROM.commit();

        EEPROM.end();

        Serial.println("END");
        ec11.int_work();
    }
}

void eeprom_flash::write_str(int add, String str)
{
    const char *p = &str[0];
    EEPROM.begin(eeprom_size);
    while (*p)
    {
        EEPROM.write(add++, *p++);
        yield();
    }
    EEPROM.write(add, '\0');
    EEPROM.end();
}
