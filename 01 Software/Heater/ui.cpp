//#include "User.h"
#include "bmp.h"
#include "ui.h"
#include "EC11.h"
#include "Oled.h"
#include "Heater.h"
#include "eeprom_flash.h"
#include "reflow.h"
UI ui;

#define page2_menu_num_max 8

extern Heater pwm;
extern Reflow reflow;

typedef enum
{
    temp_mode_menu_num,
    backflow_menu_num,
    constant_temperature_menu_num,
    iot_menu_num,
    oled_light_menu_num,
    use_menu_num,
    about_menu_num,
    error_temp_fix_num
} page2_menu_num_type;

int8_t page_num = 0;

/*菜单页定义*/
int8_t page2_menu_num = 0;
int8_t page2_move_tmp = 0;
int8_t page2_move_flg = 0;

const char page2_menu0[] = {"温控模式"};
const char page2_menu1[] = {"回流参数"};
const char page2_menu2[] = {"恒温参数"};
const char page2_menu3[] = {"物联网"};
const char page2_menu4[] = {"屏幕亮度"};
const char page2_menu5[] = {"触摸校准"};
const char page2_menu6[] = {"关于"};
const char page2_menu7[] = {"温度校准"};

const char *page2_str_ptr[] = {page2_menu0, page2_menu1, page2_menu2, page2_menu3, page2_menu4, page2_menu5, page2_menu6, page2_menu7};
const char *page2_bmp_ptr[] = {page2_bmp0, page2_bmp1, page2_bmp2, page2_bmp3, page2_bmp4, page2_bmp5, page2_bmp6, page2_bmp7};
/************************/

/*设置页定义*/
int8_t page3_switch_flg = 0;

const char menu0_option0[] = {"回流焊模式"};
const char menu0_option1[] = {"恒温模式"};

const char menu1_option0[] = {"活性区"};
const char menu1_option1[] = {"回流区"};
const char menu1_option_s[] = {"秒"};
const char menu1_option_t[] = {"摄"};
int8_t temp_mode0_option = 0; //回流曲线设置项 0：活性区温度 1：活性区时间 2：回流区温度 3：回流区时间
short curve_temp_buf[] = {    // 0：保温区温度110-200 1：保温区时长60-120 2：回流区温度220-上限  3：回流区时长 30-90
    140, 90, 255, 60};

const char menu2_option0[] = {"分钟"};

const char menu3_option0_0[] = {"单击开启"};
const char menu3_option0_1[] = {"单击关闭"};
const char menu3_option1_0[] = {"单击配置网络密匙"};
const char menu3_option1_1[] = {"请连接至热点"};
const char menu3_option1_2[] = {"QF_HP"};

const char menu5_option0_0[] = {"这么简单自己摸索"};

const char menu6_option0_0[] = {"启凡科创智能加热台"};
const char menu6_option0_1[] = {"Version:V1.5"};

const char menu7_option0_0[] = {"实测最高"};
const char menu7_option0_1[] = {"校准温度"};
const char menu7_option0_2[] = {":   `"};
const char menu7_option0_3[] = {"校准结束后将"};
const char menu7_option0_4[] = {"自动返回"};

/************************/

bool oled_flg = true;

Ticker ticker25ms;
Ticker ticker100_ms;

uint8_t count=0;

void ms25_tic()
{
  oled_flg = 1;
  
}

void ms100_tic()
{ 
  if(eeprom.write_t < 21)
  {
    eeprom.write_t ++;

    if(eeprom.write_t >= 20)
    {
      if(eeprom.write_flg)
        eeprom.write_flg = 2;
      if(ui.show_temp_mode == show_set_temp)
      {
        ui.show_temp_mode = show_now_temp;
        ui.temp_move_flg = 1;
      }
    }
  }
}

void state_change(void * arg, E_REFLOW_STATE state){
	UI *ui = (UI *)arg;
	
	if(state == REFLOW_STATE4_COOLING)
	{
		ui->heat_flg = 1;
		ui->fan_flg = 1;
	}
	if(state == REFLOW_STATE5_END){
		ui->fan_flg = 1;
	}
}

void UI::init()
{
	ticker25ms.attach_ms(25,ms25_tic);	
	ticker100_ms.attach_ms(100,ms100_tic);
	
	reflow.set_state_change_callback(state_change, &ui);
	
	reflow.set_soak_temp(curve_temp_buf[0]);
	reflow.set_soak_time(curve_temp_buf[1]);
	reflow.set_reflow_temp(curve_temp_buf[2]);
	reflow.set_reflow_time(curve_temp_buf[3]);	
}

void UI::run_task()
{
    wake_sleep_page();

    if (oled_sleep_flg)
        return;

    switch (page_num)
    {
	case 0: // 初始页面
		break;
    case 1:
        temp_move();  		// 温度值移动显示
        temp_mode_move();   // 加热模式移动显示
        heat_move();        // 加热图标移动显示
		fan_move();
        //temp_time_switch();
        if(show_warning_flg)
        {
            show_warning();
            show_warning_flg = 0;
        }
        break;
    case 2:
        page2_move();
        break;
    case 3:
        page3_switch();
        blinker_config();
        error_temp_fix_page_move();
        break;
    }

    if (page2_move_flg)
        return;
	
    page_switch(switch_buf);

    if (!oled_flg)
        return;
    oled_flg = 0;

	
    oled.clr();

    show_page(0, 0, page_num);
    write_oled_light();

    oled.refresh();
}

void UI::show_warning()
{
    for (int8_t i = 32; i >= 0; i--)
    {
        oled.clr();
        show_page(0, i, 4);
        show_page(0, i - 32, 1);
        oled.refresh();
        delay(1);
    }
    delay(1000);
    for (int8_t i = 0; i < 32; i++)
    {
        oled.clr();
        show_page(0, i, 4);
        show_page(0, i - 32, 1);
        oled.refresh();
        delay(1);
    }
}

void UI::page1_key(ec11_task_result_type ec_type, int16_t ec_value) //主界面按键对应功能
{

    if (ec_type == ec11_task_is_key)
    {	
        switch (ec_value)
        {
        case sw_click:
			heat_flg = 1;
			if(pwm.temp_mode == E_TEMP_MODE1_REFLOW)
			{
				if(reflow.is_start())
				{
					reflow.stop();
				}
				else{
					if(pwm.fan_power)
					{
						pwm.fan_stop();
					}
					
					reflow.start();
				}
			}
			else{ //E_TEMP_MODE2_CONST_TEMP
				if (pwm.power)
				{
					pwm.end();
				}
				else
				{
					if(pwm.fan_power)
					{
						pwm.fan_stop();
					}					
					pwm.begin();
				}	
			}				
            break;
        case sw_long:
            switch_buf = sure;
            page_switch_flg = 1;
            ec11.speed_up(false);
            break;
        case sw_double:
			
			if(pwm.fan_power)
			{
				pwm.fan_stop();
				fan_flg = 1;
			}
			else
			{
				if(!pwm.fan_power && !pwm.power)
				{
					pwm.fan_start();
					fan_flg = 1;
				}
			}
			
            break;

        default:
            break;
        }	
    }
    else
    {
        if (pwm.temp_mode)	
        {		
            pwm.temp_buf += ec_value;
			
			//pwm.setTargetTemp(pwm.temp_buf);

            if (pwm.temp_buf > 250)
                pwm.temp_buf = 250;
            else if (pwm.temp_buf < 10)
                pwm.temp_buf = 10;
		
            eeprom.write_flg = 1;

            if (show_temp_mode != show_set_temp)
            {
                show_temp_mode = show_set_temp;
                temp_move_flg = 1;
            }

        }
        else
        {
            show_warning_flg = 1;
        }
    }		
}

void UI::page2_key(ec11_task_result_type ec_type, int16_t ec_value) //界面2按键对应功能
{
    if (ec_type == ec11_task_is_key)
    {
        switch (ec_value)
        {

        case sw_long:
            switch_buf = sure;
            page_switch_flg = 1;
            if (page2_menu_num == constant_temperature_menu_num || page2_menu_num == oled_light_menu_num || page2_menu_num == backflow_menu_num)
                ec11.speed_up(true);
            ec11.double_click(false);
            break;
        case sw_double:
            switch_buf = back;
            page_switch_flg = 1;
            ec11.speed_up(true);
            break;

        default:
            break;
        }
    }
    else
    {
        if (page2_move_tmp < 0 && ec_value == 1)
            page2_move_tmp = -1;
        else if (page2_move_tmp > 0 && ec_value == -1)
            page2_move_tmp = 1;
        page2_move_tmp += ec_value;
        page2_move_flg = 1;
    }
}

void UI::page3_key(ec11_task_result_type ec_type, int16_t ec_value) //界面3按键对应功能
{
#if 1
    if (ec_type == ec11_task_is_key)
    {
#if 1		
        if (ec_value == sw_click) //单击
        {
            if (page2_menu_num == backflow_menu_num)
            {
                if (!page3_switch_flg)
                {
                    page3_switch_flg = 1;
                    temp_mode0_option++;
                    if (temp_mode0_option == 4)
                        temp_mode0_option = 0;
                }
            }

            if (page2_menu_num == iot_menu_num)
            {
                if (!page3_switch_flg)
                {
#if 0					
                    if (miot_option_buf)
                    {
                        blinker_config_flg = 1;
                    }
                    else
                    {
                        page3_switch_flg = 1;
                        miot.miot_able = !miot.miot_able;
                    }
#endif					
                }
            }
            if (page2_menu_num == error_temp_fix_num)
            {
#if 0				
                switch (error_temp_fix_page_buf)
                {
                case 0:
                    error_temp_fix_page_buf = 1;
                    break;
                case 1:
                    error_temp_fix_page_buf = 0;
                    break;
                case 2:
                    if (pwm.power == 0 && adc.now_temp < 150)
                    {
                        error_temp_fix_page_buf = 3;
                        error_temp_fix_page_move_buf = 2;
                    }
                    break;
                default:
                    error_temp_fix_page_buf = 2;
                    error_temp_fix_page_move_buf = 2;
                    break;
                }
#endif				
            }
        }
        else
        {
#if 0			
            if (wifima.wifima_flg)
            {
                wifima.back_flg = 1;
            }
            else
#endif				
            {
                if (error_temp_fix_page_buf == 1)
                    error_temp_fix_page_buf = 0;
                if (error_temp_fix_page_buf == 3)
                    error_temp_fix_page_buf = 2;

                switch_buf = back;
                page_switch_flg = 1;
                ec11.speed_up(false);
#if 1				
                if (page2_menu_num < 6 || page2_menu_num == error_temp_fix_num)
                    eeprom.write_flg = 1;
#endif				
                ec11.double_click(true);

                if (pwm.power && page2_menu_num == temp_mode_menu_num)
                {
                    pwm.end();
                }
#if 0				
                if (page2_menu_num == iot_menu_num)
                {
                    if (miot.miot_able && !miot.open_flg)
                    {
                        eeprom.write(miot_able_add, 1);
                        ESP.reset();
                    }
                }
#endif				
            }
        }
#endif		
    }
    else
    {

        switch (page2_menu_num)
        {
        case temp_mode_menu_num: //温控模式
            if (!page3_switch_flg)
                pwm.temp_mode = !pwm.temp_mode;
            break;

        case backflow_menu_num: //回流参数
            if (!page3_switch_flg)
            {
                switch (temp_mode0_option)
                {
                case 0:
                    curve_temp_buf[0] += ec_value;
                    if (curve_temp_buf[0] < 110)
                        curve_temp_buf[0] = 110;
                    else if (curve_temp_buf[0] > 200)
                        curve_temp_buf[0] = 200;
                    break;

                case 1:
                    curve_temp_buf[1] += ec_value;
                    if (curve_temp_buf[1] < 60)
                        curve_temp_buf[1] = 60;
                    else if (curve_temp_buf[1] > 120)
                        curve_temp_buf[1] = 120;
                    break;

                case 2:
                    curve_temp_buf[2] += ec_value;
                    if (curve_temp_buf[2] < 210)
                        curve_temp_buf[2] = 210;
#if 0					
                    else if (curve_temp_buf[2] > adc.hotbed_max_temp)
                        curve_temp_buf[2] = adc.hotbed_max_temp;
#endif					
                    break;

                case 3:
                    curve_temp_buf[3] += ec_value;
                    if (curve_temp_buf[3] < 30)
                        curve_temp_buf[3] = 30;
                    else if (curve_temp_buf[3] > 90)
                        curve_temp_buf[3] = 90;
                    break;
                }
            }
            break;

        case constant_temperature_menu_num: //恒温参数
            pwm.temp_mode1_time += ec_value;
            if (pwm.temp_mode1_time < 0)
                pwm.temp_mode1_time = 0;
            else if (pwm.temp_mode1_time > 520)
                pwm.temp_mode1_time = 520;
            break;

        case iot_menu_num: // iot
#if 0		
            if (!page3_switch_flg)
                miot_option_buf = !miot_option_buf;
#endif			
            break;

        case oled_light_menu_num: // 屏幕亮度
            oled_light += ec_value;
            if (oled_light < 0)
                oled_light = 0;
            else if (oled_light > 255)
                oled_light = 255;
            write_oled_flg = 1;
            break;
        case error_temp_fix_num: //温度校准
#if 0		
            switch (error_temp_fix_page_buf)
            {
            case 0:
                if (ec_value > 0)
                {
                    error_temp_fix_page_buf = 2;
                    error_temp_fix_page_move_buf = 1;
                }
                break;
            case 1:
            {
                adc.hotbed_max_temp += ec_value;
                if (adc.hotbed_max_temp < 240)
                    adc.hotbed_max_temp = 240;
                else if (adc.hotbed_max_temp > 270)
                    adc.hotbed_max_temp = 270;
            }
            break;
            case 2:
                if (ec_value < 0)
                {
                    error_temp_fix_page_buf = 0;
                    error_temp_fix_page_move_buf = 1;
                }
                break;
            default:
                break;
            }
#endif			
			break;
        default:
            break;
        }
        if (page2_menu_num == temp_mode_menu_num || page2_menu_num == iot_menu_num)
            page3_switch_flg = ec_value;
    }
#endif	
}

void UI::blinker_config()
{
    int8_t y;
    if (!blinker_config_flg)
        return;
    blinker_config_flg = 0;

    y = -1;
    for (;;)
    {
        if (y == -33)
            break;
        oled.clr();
        show_page(0, y, 3);
        oled.chinese(16, y + 32, menu3_option1_1, 16, 1, 0);
        oled.str(44, y + 48, menu3_option1_2, 16, 1, 0);
        oled.refresh();
        y--;
        yield();
    }
    Serial.println("Start ap config");
#if 0	
    wifima.startConfigPortal("QF_HP");
#endif	
    Serial.println("End ap config");

    y = 1;
    for (;;)
    {
        if (y == 32)
            break;
        oled.clr();
        show_page(0, y - 32, 3);
        oled.chinese(16, y, menu3_option1_1, 16, 1, 0);
        oled.str(44, y + 16, menu3_option1_2, 16, 1, 0);
        oled.refresh();
        y++;
        yield();
    }
}

bool UI::oled_display_set()
{
    if (ui.wake_sleep_change_flg)
        return 1;
    if (!ui.oled_sleep_flg)
    {
        //oled_sleep_t = 0;
        return 0;
    }
    else
    {
        ui.wake_sleep_change_flg = 1;
        //oled_sleep_t = 0;
        return 1;
    }
}

void UI::onoff_button_down()
{
	switch (page_num){
		case 1:
		{
			heat_flg = 1;
			if(pwm.temp_mode == E_TEMP_MODE1_REFLOW)
			{
				if(reflow.is_start())
				{
					reflow.stop();
				}
				else{
					if(pwm.fan_power)
					{
						pwm.fan_stop();
					}
					
					reflow.start();
				}
			}
			else{ //E_TEMP_MODE2_CONST_TEMP
				if (pwm.power)
				{
					pwm.end();
				}
				else
				{
					if(pwm.fan_power)
					{
						pwm.fan_stop();
					}					
					pwm.begin();
				}	
			}
		}
		break;
		default:break;
	};
}

void UI::fan_button_down()
{
	switch (page_num){
		case 1:
		{
			if(pwm.fan_power)
			{
				pwm.fan_stop();
				fan_flg = 1;
			}
			else
			{
				if(!pwm.fan_power && !pwm.power)
				{
					pwm.fan_start();
					fan_flg = 1;
				}
			}
		}
		break;
		default:break;
	};
}

void ui_key_callb(ec11_task_result_type ec_type, int16_t ec_value) //按键事件中断处理
{

    if (ui.oled_display_set())
        return;

    switch (page_num)
    {
    case 1:
        ui.page1_key(ec_type, ec_value);
        break;
    case 2:
        ui.page2_key(ec_type, ec_value);
        break;
    case 3:
        ui.page3_key(ec_type, ec_value);
        break;
    }
	
    eeprom.write_t = 0;	
}

void UI::write_oled_light()
{
    if (write_oled_flg)
    {
        write_oled_flg = 0;
        oled.light(oled_light);
    }
}

bool UI::page_switch(uint8_t mode)
{
    if (!page_switch_flg)
        return 0;
    page_switch_flg = false;

    int8_t next_page;
    int8_t show_y = 0;
    int8_t next_y;
    if (mode == back)
    {
        next_page = page_num - 1;
        next_y = -32;
    }
    else if (mode == sure)
    {
        next_page = page_num + 1;
        next_y = 32;
    }
    else
        return 0;

    for (;;)
    {
        oled.clr();
        show_page(0, show_y, page_num);
        show_page(0, next_y, next_page);
        oled.refresh();

        if (mode == back)
        {
            show_y++;
            next_y++;
        }
        else
        {
            show_y--;
            next_y--;
        }
        if (show_y == 33 || show_y == -33)
            break;
    }

    page_num = next_page;

    return 1;
}

void UI::show_page(short x, short y, uint8_t page)
{
    uint8_t mode_tmp = ui.show_temp_mode;
    switch (page)
    {
    case 1:
        if (pwm.temp_mode == Re_So)
		{			
			E_REFLOW_STATE state = reflow.get_state();
			switch(state){
				case REFLOW_STATE1_PRHEAT:
				show_temp_ext(x, y, pwm.now_temp, 93,y+18, reflow.get_soak_temp());
				break;
				case REFLOW_STATE2_SOAK:
				show_temp_ext(x, y, pwm.now_temp, 93,y+18, reflow.get_time_left());
				break;
				case REFLOW_STATE3_RELOW:
				show_temp_ext(x, y, pwm.now_temp, 93,y+18, reflow.get_reflow_temp());
				break;
				case REFLOW_STATE3_1_RELOW:
				show_temp_ext(x, y, pwm.now_temp, 93,y+18, reflow.get_time_left());
				break;
				default:
				show_temp_ext(x, y, pwm.now_temp, 93,y+18, reflow.get_reflow_temp());
				break;
			}
		}
		else{
			if (show_temp_mode == show_now_temp)
				show_temp_ext(x, y, pwm.now_temp, 93,y+18, pwm.temp_buf);
			else
				show_temp(x, y, 0, 0);
		}
		
        oled.xy_set(0, 0, 128, 4);
        if (pwm.temp_mode == Re_So)
		{
            oled.chinese(69, y, "回流", 16, 1, 0);
		}
        else
            oled.chinese(69, y, "恒温", 16, 1, 0);
		
        if (pwm.power)
            oled.BMP(95, y + 2, 32, 28, heating, 1);			
		if(pwm.fan_power)
			oled.BMP(99, y + 0, 32, 28, fan, 1);
		
        break;
    case 2:

        oled.xy_set(0, 0, 128, 4);
        oled.chinese(64, y + 8, page2_str_ptr[page2_menu_num], 16, 1, 0);
        oled.BMP(y, page2_bmp_ptr[page2_menu_num]);
        break;
    case 3:
        switch (page2_menu_num)
        {
        case temp_mode_menu_num: //模式设置
            if (pwm.temp_mode)
                oled.chinese(32, y + 8, menu0_option1, 16, 1, 0);
            else
                oled.chinese(24, y + 8, menu0_option0, 16, 1, 0);
            break;

        case backflow_menu_num: //回流曲线
            if (page3_switch_flg)
            {
                show_curve(0, y);
            }
            else
            {
                show_curve(y, y);
            }
            break;

        case constant_temperature_menu_num: //恒温时长
            ui.show_temp_mode = show_temp_mode1_time;
            show_temp(12, y, 0, 0);
            oled.chinese(84, y + 16, menu2_option0, 16, 1, 0);
            ui.show_temp_mode = mode_tmp;
            break;
#if 0
        case iot_menu_num: //物联网
            if (miot_option_buf)
            {
                oled.chinese(0, y + 8, menu3_option1_0, 16, 1, 0);
            }
            else
            {
                if (miot.miot_able)
                    oled.chinese(32, y + 8, menu3_option0_1, 16, 1, 0);
                else
                    oled.chinese(32, y + 8, menu3_option0_0, 16, 1, 0);
            }
            break;
#endif
        case oled_light_menu_num: //屏幕亮度
            ui.show_temp_mode = show_set_light;
            show_temp(28, y, 0, 0);
            ui.show_temp_mode = mode_tmp;
            break;

        case use_menu_num: //使用说明
            oled.chinese(0, y + 8, menu5_option0_0, 16, 1, 0);
            //           oled.str(64, y + 8, menu5_option0_1, 16, 1, 0);
            break;

        case about_menu_num: //关于
            oled.chinese(0, y, menu6_option0_0, 16, 1, 1);
            oled.str(32, y + 16, menu6_option0_1, 16, 1, 0);
            break;
#if 0
        case error_temp_fix_num: //温度校准

            oled.chinese(0, y, menu7_option0_0, 16, 1, 1);
            oled.chinese(0, y + 16, menu7_option0_1, 16, 1, 1);
            oled.str(64, y, menu7_option0_2, 16, 1, 0);
            oled.str(64, y + 16, menu7_option0_2, 16, 1, 0);
            oled.num(72, y, adc.hotbed_max_temp, 3, 16, LEFT, 1);
            oled.num(72, y + 16, adc.adc_max_temp, 3, 16, LEFT, 1);

            switch (error_temp_fix_page_buf)
            {
            case 0:
                oled.BMP(118, y + 4, circle_kong);
                break;
            case 1:
                oled.BMP(118, y + 4, circle_shi);
                break;
            default:
                oled.BMP(118, y + 20, circle_kong);
                break;
            }
            break;
#endif			
        }
        break;
		
    case 4: //显示提示
        oled.chinese(x + 8, y, "回流模式请到菜", 16, 1, 0);
        oled.chinese(x + 16, y + 16, "单内设置参数", 16, 1, 0);
        break;
    default:
        break;
    }

}

void UI::wake_sleep_page()
{
    if (wake_sleep_change_flg)
    {

        oled.display_on();
        if (oled_sleep_flg)
        {
            for (int8_t i = 32; i > 0; i--)
            {
                oled.clr();
                show_page(0, i, page_num);
                oled.refresh();
                yield();
            }
        }
        else
        {
            oled.roll(0, 0, 128, 4, 1, UP, 32);
            oled.display_off();
        }
        oled_sleep_flg = !oled_sleep_flg;
        wake_sleep_change_flg = 0;
    }
}

void UI::page3_switch()
{

    if (!page3_switch_flg)
        return;
    int8_t y;

    if (page3_switch_flg < 0)
    {
        oled.roll(0, 0, 128, 4, 2, DOWN, 16);
        y = -32;
    }
    else
    {
        if (page2_menu_num == backflow_menu_num)
            oled.roll(72, 0, 48, 4, 1, UP, 32);
        else
            oled.roll(0, 0, 128, 4, 2, UP, 16);
        y = 32;
    }

    for (;;)
    {
        if (y == 0)
            break;
        oled.clr();
        show_page(0, y, 3);
        oled.refresh();
        if (y < 0)
            y++;
        else
            y--;
        yield();
    }

    page3_switch_flg = 0;
}

void UI::page2_move()
{

    if (!page2_move_flg)
        return;
    int8_t num_tmp;
    int8_t now_y = 0;
    int8_t next_y;
    int8_t bmp_x;

    if (page2_move_tmp < 0)
    {
        num_tmp = page2_menu_num - 1;
        next_y = -32;
    }
    else
    {
        num_tmp = page2_menu_num + 1;
        next_y = 32;
    }
    if (num_tmp < 0)
        num_tmp = page2_menu_num_max - 1;
    else if (num_tmp == page2_menu_num_max)
        num_tmp = 0;

    for (;;)
    {
        if (page2_move_tmp == 0)
        {
            if (next_y < 0)
                page2_move_tmp = -1;
            else
                page2_move_tmp = 1;
        }
        now_y -= page2_move_tmp;
        next_y -= page2_move_tmp;
        if (now_y < -32 || now_y > 32)
            break;
        oled.clr();
        oled.chinese(64, now_y + 8, page2_str_ptr[page2_menu_num], 16, 1, 0);
        oled.BMP(now_y, page2_bmp_ptr[page2_menu_num]);

        oled.chinese(64, next_y + 8, page2_str_ptr[num_tmp], 16, 1, 0);
        oled.BMP(next_y, page2_bmp_ptr[num_tmp]);

        oled.refresh();
        yield();
    }
    if (page2_move_tmp < 0)
    {
        page2_move_tmp++;
    }
    else
    {
        page2_move_tmp--;
    }
    if (page2_move_tmp == 0)
        page2_move_flg = 0;
    page2_menu_num = num_tmp;
}

void UI::heat_move()
{

    if (!heat_flg)
        return;
    heat_flg = 0;
    int8_t y;
    if (pwm.power)
        y = 32;
    else
        y = 2;
    oled.xy_set(0, 0, 128, 4);
    for (;;)
    {
        if (pwm.power)
            y--;
        else
            y++;
        oled.choose_clr(101, 0, 27, 2);
        oled.choose_clr(95, 2, 33, 2);
        oled.BMP(95, y, 32, 28, heating, 1);
        oled.choose_refresh(95, 0, 32, 4);
        if (y == 2 || y == 32)
            return;
    }
}

void UI::fan_move()
{

    if (!fan_flg)
        return;
    fan_flg = 0;
    int8_t y;
    if (pwm.fan_power)
        y = 32;
    else
        y = 2;
    oled.xy_set(0, 0, 128, 4);
    for (;;)
    {
        if (pwm.fan_power)
            y--;
        else
            y++;
        //oled.choose_clr(101, 0, 26, 2);
		oled.choose_clr(99, 0, 32, 4);
		oled.BMP(99, y, 32, 28, fan, 1);
		oled.choose_refresh(99, 0, 32, 4);
        if (y == 2 || y == 32)
            return;
    }
}

void UI::temp_mode_move()
{
    if (!temp_mode_flg)
        return;
    temp_mode_flg = 0;
    int8_t y = 0;
    oled.xy_set(0, 0, 128, 2);
    for (y = 0; y >= -16; y--)
    {
        oled.choose_clr(69, 0, 32, 2);
        if (pwm.temp_mode == Re_So)
        {
            oled.chinese(69, y + 16, "回流", 16, 1, 0);
            oled.chinese(69, y, "恒温", 16, 1, 0);
        }
        else
        {
            oled.chinese(69, y, "回流", 16, 1, 0);
            oled.chinese(69, y + 16, "恒温", 16, 1, 0);
        }
        oled.choose_refresh(69, 0, 32, 2);
        delay(8);
    }
}

extern Ticker adc_max_temp_tic;

void UI::error_temp_fix_page_move()
{
#if 0	
    if (error_temp_fix_page_move_buf == 0)
        return;

    int8_t tmp;

    if (error_temp_fix_page_move_buf == 1)
    {
        if (error_temp_fix_page_buf)
            tmp = 4;
        else
            tmp = 20;
        for (uint8_t x = 1; x < 17; x++)
        {
            oled.choose_clr(118, 0, 9, 4);
            oled.BMP(118, tmp, circle_kong);
            oled.choose_refresh(118, 0, 9, 4);
            if (error_temp_fix_page_buf)
                tmp++;
            else
                tmp--;
            delay(10);
        }
    }
    else
    {

        if (error_temp_fix_page_buf == 2)
            tmp = -32;
        else
            tmp = 0;
        for (uint8_t x = 0; x < 33; x++)
        {
            oled.clr();
            show_page(0, tmp, 3);
            oled.chinese(0, tmp + 32, menu7_option0_3, 16, 1, 1);
            oled.chinese(0, tmp + 48, menu7_option0_4, 16, 1, 1);
            oled.BMP(95, tmp + 34, 32, 28, heating, 1);
            oled.num(64, tmp + 48, adc.now_temp, 3, 16, LEFT, 1);
            oled.refresh();
            if (error_temp_fix_page_buf == 2)
                tmp++;
            else
                tmp--;
            yield();
        }
        if (error_temp_fix_page_buf == 3)
        {
			
            adc.adc_max_temp_auto_flg = 0;
			
            digitalWrite(PWM_IO, HIGH);
            adc_max_temp_tic.attach(20, adc_max_temp_auto_feed);
            error_temp_fix_page_buf = 4;
            while (error_temp_fix_page_buf == 4)
            {
                if (adc.adc_max_temp_auto())
                {
                    digitalWrite(PWM_IO, LOW);
                    break;
                }
                oled.choose_clr(64, 2, 24, 2);
                oled.num(64, 16, adc.now_temp, 3, 16, LEFT, 1);
                oled.refresh();
                delay(1000);
            }
            adc_max_temp_tic.detach();
            error_temp_fix_page_buf = 2;
            adc.adc_max_temp_auto_flg = 1;
            digitalWrite(PWM_IO, LOW);
            tmp = -32;
            for (uint8_t x = 0; x < 33; x++)
            {
                oled.clr();
                show_page(0, tmp, 3);
                oled.chinese(0, tmp + 32, menu7_option0_3, 16, 1, 1);
                oled.chinese(0, tmp + 48, menu7_option0_4, 16, 1, 1);
                oled.BMP(95, tmp + 34, 32, 28, heating, 1);
                oled.num(64, tmp + 48, adc.now_temp, 3, 16, LEFT, 1);
                oled.refresh();
                tmp++;
                yield();
            }
        }
    }
    error_temp_fix_page_move_buf = 0;
#endif	
}

void UI::show_temp_ext(int8_t x, int8_t y, uint16_t tmp, int8_t x2, int8_t y2, uint16_t temp2)
{
	uint8_t dat_buf[3];
	
	oled.xy_set(68, 0, 128, 4);
	oled.num(x2, y2, temp2, 3, 16, RIGHT, 1);
	//oled.num(93, 18, pwm.temp_buf, 3, 16, RIGHT, 1);
	
    dat_buf[0] = tmp / 100 % 10;
    dat_buf[1] = tmp / 10 % 10;
    dat_buf[2] = tmp % 10;
    if (show_temp_mode < 3)
        oled.xy_set(0, 0, 68, 4);
    else
        oled.xy_set(0, 0, 128, 4);
    if (dat_buf[0])
        oled.BMP(x, y, 20, 32, number[dat_buf[0]], 1);
    if (dat_buf[0] || dat_buf[1])
        oled.BMP(x + 24, y, 20, 32, number[dat_buf[1]], 1);
    oled.BMP(x + 48, y, 20, 32, number[dat_buf[2]], 1);
}

void UI::temp_move()
{	
    int8_t temp_x;
    uint8_t temp_y;
    uint8_t small_x;
	
    if (!temp_move_flg)
        return;
    temp_move_flg = 0;

    if (show_temp_mode == show_now_temp)
    {
        temp_x = 0;
    }
    else
    {
        oled.roll(0, 0, 68, 4, 2, UP, 16);
        temp_x = 68;
    }
    for (;;)
    {
		// 从显示设置温度-》显示当前温度，设置温度向右下角移动出去
        if (show_temp_mode == show_now_temp)
        {
            temp_x += 4;
        }
        else if (show_temp_mode == show_set_temp)
        {
            temp_x -= 4;
        }

        temp_y = temp_x * 1000 / 2125;
        small_x = temp_x * 100 / 283 + 69;

        oled.choose_clr(0, 0, 68, 4);
        oled.choose_clr(68, 2, 24, 2);
	
        show_temp(temp_x, temp_y, small_x, 18);

        oled.choose_refresh(0, 0, 68, 4);
        oled.choose_refresh(68, 2, 24, 2);
        if (temp_x >= 68 || temp_x <= 0)
            break;
        
    }
	// 从下到上，显示当前温度
    if (show_temp_mode == show_now_temp)
    {
        for (temp_y = 32; temp_y > 0; temp_y -= 2)
        {
            oled.choose_clr(0, 0, 68, 4);
            //show_temp(0, temp_y, 0, 18);
			show_temp_ext(0, temp_y, pwm.now_temp,0,18, pwm.temp_buf);
            oled.choose_refresh(0, 0, 68, 4);
            
        }
    }
}



void UI::show_temp(int8_t x, int8_t y, int8_t xx, int8_t yy)
{

    uint8_t dat_buf[3];
    uint16_t tmp;
    if (show_temp_mode == show_now_temp)
    {
        tmp = pwm.temp_buf;//pwm.now_temp;
        if (tmp <= 38)
            oled.BMP(x + 2, y, less);
    }
    else if (show_temp_mode == show_set_temp)
    {
        tmp = pwm.temp_buf;
    }
    else if (show_temp_mode == show_set_light)
    {
        tmp = oled_light;
    }
    else if (show_temp_mode == show_temp_mode1_time)
    {
        tmp = pwm.temp_mode1_time;
    }
    else
        return;
    oled.xy_set(68, 0, 128, 4);

    if (pwm.temp_mode)
    {
        if (pwm.temp_reached_flg)
            //oled.num(xx, yy, temp_time_buf, 3, 16, RIGHT, 1);
			oled.num(xx, yy, pwm.temp_buf, 3, 16, RIGHT, 1);
        else
            oled.num(xx, yy, pwm.temp_buf, 3, 16, RIGHT, 1);
    }
    else
        oled.num(xx, yy, pwm.percent, 3, 16, RIGHT, 1);

    dat_buf[0] = tmp / 100 % 10;
    dat_buf[1] = tmp / 10 % 10;
    dat_buf[2] = tmp % 10;
    if (show_temp_mode < 3)
        oled.xy_set(0, 0, 68, 4);
    else
        oled.xy_set(0, 0, 128, 4);
    if (dat_buf[0])
        oled.BMP(x, y, 20, 32, number[dat_buf[0]], 1);
    if (dat_buf[0] || dat_buf[1])
        oled.BMP(x + 24, y, 20, 32, number[dat_buf[1]], 1);
    oled.BMP(x + 48, y, 20, 32, number[dat_buf[2]], 1);

}

void UI::show_curve(int8_t y, int8_t data_y)
{
    int8_t y_tmp;
    int8_t i;
    int8_t y_buf;
    int8_t x_tmp;
    int8_t x_buf;
    int8_t tmp;
    int8_t last_y;

    y_tmp = -((curve_temp_buf[0] - 40) * 100000 / 671875) + 32;
    y_buf = y_tmp;

    oled.point(11, y_buf + 1 + y, 1);

    for (i = 10; i > 0; i--) //升温曲线1
    {
        oled.point(i, ++y_buf + y, 1);
    }
    x_tmp = curve_temp_buf[1] / 5;
    x_buf = curve_temp_buf[3] / 5;

    for (i = 0; i < x_tmp; i++) //保温曲线
    {
        oled.point(12 + i, y_tmp + y, 1);
    }
    x_tmp += 12;

    y_tmp--;

    oled.point(x_tmp++, y_tmp + y, 1);

    y_buf = -((curve_temp_buf[2] - 40) * 100000 / 671875) + 32;

    for (i = 10; i > 0; i--) //升温曲线2
    {
        oled.point(x_tmp++, y_tmp + y, 1);
        y_tmp--;
        if (y_tmp == y_buf)
            break;
        if (y_tmp < 6)
        {
            x_buf -= 2;
        }
    }

    oled.point(x_tmp++, y_tmp + 1 + y, 1);
    x_buf -= 2;

    for (i = 0; i < x_buf; i++)
    {
        oled.point(x_tmp++, y_tmp + y, 1);
    }
    y_tmp++;
    oled.point(x_tmp++, y_tmp + y, 1);

    for (i = 0; i < 10; i++)
    {
        oled.point(x_tmp++, y_tmp + y, 1);
        y_tmp++;
        if (x_tmp == 72)
            break;
    }

    if (temp_mode0_option < 2)
        oled.chinese(72, data_y, menu1_option0, 16, 1, 0);
    else
        oled.chinese(72, data_y, menu1_option1, 16, 1, 0);

    oled.num(100, data_y + 16, curve_temp_buf[temp_mode0_option], 3, 16, RIGHT, 1);

    if (temp_mode0_option == 0 || temp_mode0_option == 2)
        oled.chinese(100, data_y + 16, menu1_option_t, 16, 1, 0);
    else
        oled.chinese(100, data_y + 16, menu1_option_s, 16, 1, 0);
}

void UI::temp_time_switch()
{
#if 0	
    if (temp_time_switch_flg == false)
        return;
    if (pwm.temp_reached_flg == false)
        min_count = 0;
    Serial.println(pwm.temp_reached_flg);
    pwm.temp_reached_flg = !pwm.temp_reached_flg;
	
    oled.roll(69, 2, 24, 2, 1, UP, 16);
#endif
    for (int8_t i = 32; i > 18; i--)
    {
        oled.choose_clr(69, 2, 24, 2);
#if 1		
        if (pwm.temp_reached_flg)
            //oled.num(93, i, temp_time_buf, 3, 16, RIGHT, 1);
			oled.num(93, i, pwm.temp_buf, 3, 16, RIGHT, 1);
        else
            oled.num(93, i, pwm.temp_buf, 3, 16, RIGHT, 1);
#endif		
		oled.num(93, i, 112, 3, 16, RIGHT, 1);
        oled.choose_refresh(69, 2, 24, 2);
        delay(10);
    }
    temp_time_switch_flg = false;

}
