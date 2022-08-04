
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Oled.h"

Oled oled;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//屏幕缓存定义  根据不同平台更改内存位置修饰符
unsigned char Oled_GRAM[PAGE_SIZE][WIDTH];
unsigned char Oled_roll_x[4]={0,WIDTH,0,PAGE_SIZE};//滚动显示边界设定，依次为左上角xy和右下角xy
/*************************************
以下两个函数请对应移植平台进行修改
***************************************/
unsigned char *buff = NULL;

void write_oled(unsigned char dat,unsigned char mode) //根据不同平台更改IIC或SPI发送函数
{
  /* ***********IIC*************************/
  Wire.beginTransmission(Oled_SLAVE);//开始IIC并写物理地址
  if(mode)//判断是指令还是数据
  {
    Wire.write(0x40);
  }
  else
  {
    Wire.write(0x00);
  }
  Wire.write(dat);//写数据
  Wire.endTransmission();//结束
  /**************************************/
  
  /* ***********SPI*************************/
//  OLED_CS_CLR;
//  if(mode){OLED_DC_SET;}else{OLED_DC_CLR;}
//  write_oled(dat);//写数据
//  OLED_CS_SET;
  /**************************************/

}


void Oled::begin()//IIC/SPI初始化函数，根据平台自行修改
{

}

/*******************************************/

void Oled::init()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  buff = display.getBuffer();
}  
/*******************************************/







 


void Oled::clr()  
{ 
  unsigned char i,n; 
  unsigned char *p = &Oled_GRAM[0][0];
  for(i=0;i<PAGE_SIZE;i++)  
  {    
    for(n=0;n<WIDTH;n++)
    {
      *p++=0;
    }
  } 
}

void Oled::choose_clr(unsigned char x,unsigned char y,unsigned char width, unsigned char height)  
{ 
  unsigned char i,n; 
  unsigned char *p;
  for(i=0;i<height;i++)  
  {    
    p=&Oled_GRAM[y+i][x];
    for(n=0;n<width;n++)
    {
      *p++=0;
    }
  } 
}



void Oled::write_char(short x,short y,char chr,unsigned char Size,unsigned char mode)
{       
  unsigned char c,i=1,j; 
  short buf;
  unsigned char k,l;
  unsigned char tmp;
  unsigned char flg;
  unsigned char y_buf=Oled_roll_x[2]>>3;
  unsigned char csize;
  
  
  c=chr-' ';//得到偏移后的值   
  
  if(Size==16)
    csize=8;
  
  buf=Oled_roll_x[2]-y;
  if(buf>15)
    return;
  
  if(buf>7&&buf<16)
  {
    y +=8;
    i=0;
  }
  l=y>>3;
  if(y<=Oled_roll_x[2])
  {
    k = Oled_roll_x[2]-y;
    for(j=0;j<csize;j++)
    {
      buf=x+j;
      if(buf>=Oled_roll_x[0]&&buf<Oled_roll_x[1])
      {
        if(i)
        {
            tmp=F8X16[c*16+j];
        }
        else
        {
            tmp=F8X16[c*16+j+8];
        }
        flg=tmp>>k;
        if(!mode)
        {
          Oled_GRAM[y_buf][buf] &= ~flg;
        }
        else
        {
          Oled_GRAM[y_buf][buf] |= flg;
        }
        
        
        
        if(i&&Size==16)
        {
          tmp=F8X16[c*16+j+8];
          flg=tmp<<(8-k);
          if(!mode)
          {
            Oled_GRAM[y_buf][buf] &= ~flg;
          }
          else
          {
            Oled_GRAM[y_buf][buf] |= flg;
          }
          
          
          if(Oled_roll_x[3]>1)
          {
            flg=tmp>>k;
            if(!mode)
            {
              Oled_GRAM[y_buf+1][buf] &= ~flg; 
            }
            {
              Oled_GRAM[y_buf+1][buf] |= flg; 
            }
            
          }           
        }
          
      }
    }
  }
  else
  {
    k=y%8;
    flg=2;
    for(i=0;i<flg;i++)
    {
      if((l+i)>=Oled_roll_x[3])
        return;
      
      for(j=0;j<csize;j++)
      {
        buf=x+j;
        if(buf>=Oled_roll_x[0]&&buf<Oled_roll_x[1])
        {

          tmp=F8X16[c*16+j+i*8];
          Size=tmp<<k;
          if(!mode)
            Oled_GRAM[l+i][buf] &= ~Size;
          else
            Oled_GRAM[l+i][buf] |= Size;

          if(k&&(l+i)<(Oled_roll_x[3]+y_buf-1))
          {
            Size=tmp>>(8-k);
            if(!mode)
              Oled_GRAM[l+i+1][buf] &= ~Size;
            else
              Oled_GRAM[l+i+1][buf] |= Size;

          }
        }

      }
      
    }
  }
}


void Oled::str(short x,short y,const void *chr,unsigned char Size,unsigned char mode,unsigned char fill)
{
  unsigned char csize;
  const char *p = (char*)chr;
  if(Size == 16)
  {
    csize = 8;
  }
  else
  {
    return;
  }
  while (*p)
  {   
    if((y/8)>(Oled_roll_x[2]+Oled_roll_x[3]-1))
      return;
    write_char(x,y,*p++,Size,mode);
    x+=csize;
    if(x>Oled_roll_x[1]-8)
    {
      if(fill)
      {
        x=Oled_roll_x[0];
        y+=Size;
      }
      else
      {
        if(x>Oled_roll_x[1])
          return;
      }
    }
  }
}


void Oled::refresh()
{
	unsigned char *p = &Oled_GRAM[0][0]; 

	memcpy(buff, p, PAGE_SIZE*WIDTH);
	display.display();
}

void Oled::choose_refresh(unsigned char x,unsigned char y,unsigned char width, unsigned char height)
{
	refresh();
}

static uint32_t mypow(unsigned char m,unsigned char n)
{
  uint32_t result=1;  
  while(n--)result*=m;    
  return result;
}

unsigned char Oled::num(short x,short y,long num,unsigned char len,unsigned char Size,unsigned char align,unsigned char mode)
{           
  unsigned char t;
  unsigned char enshow=0,csize;
  unsigned char count=0;
  unsigned char low_zero=0;
  unsigned char dat_buf[10];
  unsigned char tmp=0;
//  short rem_x=x;
  if(Size == 16)
  {
    csize = 8;
  }
  else if(Size == 8)
  {
    csize = 6;
  }   
  else
  {
    return 0;
  }
  
  if(num<0)
  {
    low_zero=1;
    num=-num;
  }

  for(t=0;t<len;t++)
  {
    dat_buf[t]=(num/mypow(10,len-t-1))%10;
    if(align==RIGHT&&dat_buf[t]&&!tmp)
      tmp=len-t;
  }
  if(!tmp)
    tmp=1;
  
  if(align==RIGHT)
  {
    x-=tmp*csize;
    if(low_zero)
      x-=csize;
  }
  
  if(low_zero)
  {
    write_char(x,y,'-',Size,mode);
    x+=csize; 
    count++;
  }

  for(t=0;t<len;t++)
  {
    if(enshow==0&&t<(len-1))
    {
      if(dat_buf[t]==0)
      {
        continue;
      }
      else 
      {
        enshow=1; 
      }
    }
    
    write_char(x,y,dat_buf[t]+'0',Size,mode); 
    x+=csize;
    count++;
  }
  return count;
} 



unsigned char OLED_ShowFont(short x,short y,const char *s,unsigned char Size,unsigned char mode)
{
  unsigned char i,j,k,tmp;
  unsigned char h_buf=Size/8;
  uint16_t num;
  const char *font;
  unsigned char y_buf;
  unsigned char font_buf;

  if(Size==16)
  {
    num = sizeof(cfont16)/sizeof(typFNT_GB16);
    font=&cfont16[0].Index[0];
    font_buf=36;
  }
  else
    return 0;
  

  if(y<Oled_roll_x[2])
  {
    if(y+Size<Oled_roll_x[2])
      return 0;
    else
    {
      i=(Oled_roll_x[2]-y)/8;
      h_buf-=i;
      y+=i<<3;
    }
  }
  if((y/8)>(Oled_roll_x[3]+Oled_roll_x[2]-1))
    return 0;

  for(i=0;i<num;i++)
  {
    if(*font==*s&&*(font+1)==*(s+1)&&*(font+2)==*(s+2))
    {
      font+=4;
      font+=(Size/8-h_buf)*Size;
      
      if(y<Oled_roll_x[2])
      {
        y_buf=Oled_roll_x[2]-y;
        for(j=0;j<Size;j++)
        { 
          tmp = *font++;
          tmp>>=y_buf;
          if((x+j)>=Oled_roll_x[0]&&(x+j)<Oled_roll_x[1])
          if(mode)
            Oled_GRAM[Oled_roll_x[2]][x+j]|=tmp;
          else
            Oled_GRAM[Oled_roll_x[2]][x+j]&=~tmp;
        }
        y+=8; 
        h_buf--;
      }

      y_buf=y%8;
      y>>=3;
      
      for(k=0;k<h_buf;k++)
      {
        if((y+k)>(Oled_roll_x[3]+Oled_roll_x[2]-1))
          return 1;
        for(j=0;j<Size;j++)
        {
          
          tmp = *font++;
          if((x+j)>=Oled_roll_x[0]&&(x+j)<Oled_roll_x[1])
          {
            font_buf=tmp<<y_buf;
            if(mode)
              Oled_GRAM[y+k][x+j]|=font_buf;
            else
              Oled_GRAM[y+k][x+j]&=~font_buf;
            if(y_buf&&(y+k+1)<(Oled_roll_x[3]+Oled_roll_x[2]))
            {
              font_buf=tmp>>(8-y_buf);
              if(mode)
                Oled_GRAM[y+k+1][x+j]|=font_buf;
              else
                Oled_GRAM[y+k+1][x+j]&=~font_buf;
            }
          }
        }
      } 
      return 1;
    }
    font+=font_buf;
  }
  return 0;
}



void Oled::chinese(short x,short y,const void *str,unsigned char hsize,unsigned char mode,unsigned char fill)
{ 
  const char *p = (char*)str;

  while(*p)
  {
    if(OLED_ShowFont(x,y,p,hsize,mode))
      x+=hsize;
    if(x>(Oled_roll_x[1]-hsize))
    {
      if(fill)
      {
        x=Oled_roll_x[0];
        y+=hsize;
        if(y>(WIDTH-1))
          return;
      }
      else
      {
        if(x>Oled_roll_x[1])
          return;
      }
    }
    p+=3;
  }     
}



void Oled::point(unsigned char x,unsigned char y,unsigned char mode)//x 0-127  y 0-63 
{
  unsigned char i;
  if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    return;
  i=y%8;
  y>>=3;
  if(mode)
  {
    Oled_GRAM[y][x]|=0x01<<i;
  }
  else
  {
    Oled_GRAM[y][x]&=~(0x01<<i);
  }
}

void Oled::line(unsigned char x,unsigned char y,unsigned char width, unsigned char height,unsigned char mode)
{
  unsigned char i,j;
  if(x > WIDTH - 1)
    return;
  if(x + width>WIDTH)
    width = WIDTH - x;
  for(i=0;i<height;i++)
  {
    for(j=0;j<width;j++)
      point(x+j,y+i,mode);
  }
}

void Oled::BMP(short x,short  y,short width, short height,const void *p, unsigned char mode)
{   
  unsigned char i,j;
  unsigned char k,l;
  unsigned char tmp;
  unsigned char num;
  short buf;
  unsigned char y_buf=Oled_roll_x[2]/8;
  const char *str=(const char*)p;
  
  if(y<(Oled_roll_x[2]-8))
  {
    buf=(Oled_roll_x[2]-y)/8;
    str += buf*width;
    height -= buf*8;
    if(height<1)
      return;
    y += buf*8;
  }
  
  if(y<Oled_roll_x[2])
  {
    k =Oled_roll_x[2]-y;
    for(j=0;j<width;j++)
    {
      buf=x+j;
      if(buf>=Oled_roll_x[0]&&buf<Oled_roll_x[1])
      {
        if(mode)
        {
          tmp=*str>>k;
          Oled_GRAM[y_buf][buf] |= tmp;
        }
        else
        {
          tmp=~(*str>>k);
          Oled_GRAM[y_buf][buf] &= tmp;
        }
          
        
        
      }
      str++;
    }
    y+=8;
    height-=8;
    if(height<1)
      return;
  }
  k=y%8;
  l=y>>3;
  num=((height-1)>>3)+1; 
  for(i=0;i<num;i++)
  {
    if((l+i)>(Oled_roll_x[3])+y_buf-1)
      return;
    
      
    for(j=0;j<width;j++)
    {
      buf=x+j;
      if(buf>=Oled_roll_x[0]&&buf<Oled_roll_x[1])
      {
        if(mode)
        {
          tmp=*str<<k;
          Oled_GRAM[l+i][buf] |= tmp;
        }
        else
        {
          tmp=~(*str<<k);
          Oled_GRAM[l+i][buf] &= tmp;
        }
        
        
        
        if(k&&(l+i)<((Oled_roll_x[3])+y_buf-1))
        {
          if(mode)
          {
            tmp=*str>>(8-k);
            Oled_GRAM[l+i+1][buf] |= tmp;
          }
          else
          {
            tmp=~(*str>>(8-k));
            Oled_GRAM[l+i+1][buf] &= tmp;
          }
          
        }
      }
      str++;
    }
  }
}


void Oled::BMP(short x,short  y,const void *p,unsigned char mode)
{   
  const char *str=(char*)p;
  BMP(x,y,str[0],str[1],str+2,mode);
}
void Oled::BMP(short  y,const void *p)
{   
  const char *str=(char*)p;
  BMP(str[0],y,str[1],str[2],str+3,1);
}
void Oled::BMP(short x,short  y,const void *p)
{   
  const char *str=(char*)p;
  BMP(x,y,str[0],str[1],str+2,1);
}

void Oled::light(unsigned char light)
{
  display.ssd1306_command(0x81); // contract control
  display.ssd1306_command(light);//   
}

void Oled::xy_set(unsigned char x,unsigned char y,unsigned char xx,unsigned char yy)
{
  Oled_roll_x[0]=x;
  Oled_roll_x[1]=xx;
  Oled_roll_x[2]=y<<3;
  if((y+yy)>PAGE_SIZE)
    Oled_roll_x[3]=PAGE_SIZE-y;
  else
    Oled_roll_x[3]=yy;
}

void Oled::roll(unsigned char x,unsigned char y,unsigned char width,unsigned char height,unsigned char cont,unsigned char roll_mode,unsigned char roll_cont)
{
  signed char i;
  unsigned char j;
  unsigned char tmp;
  short buf;
  unsigned char flg;
//  
  tmp=8-cont;
  
  while(roll_cont--)
  {
    if(roll_mode==UP)
    {
      for(i=0;i<height;i++)
      {
        buf=y+i;
        if((buf+1)<(height+y))
          flg=1;
        else
          flg=0;
        for(j=0;j<width;j++)
        {
          Oled_GRAM[buf][x+j]>>=cont;
          if(flg)
            Oled_GRAM[buf][x+j]|=Oled_GRAM[buf+1][x+j]<<tmp;
        }
      }
    }
    else if(roll_mode==DOWN)
    {
      for(i=height-1;i>-1;i--)
      {
        buf=y+i;
        if(buf>y)
          flg=1;
        else
          flg=0;
        for(j=0;j<width;j++)
        {
          Oled_GRAM[buf][x+j]<<=cont;
          if(flg)
            Oled_GRAM[buf][x+j]|=Oled_GRAM[buf-1][x+j]>>tmp;
        }
      } 
    }
    else if(roll_mode==LEFT)
    {
      tmp=x+width-1;
      for(flg=0;flg<height;flg++)
      {
        buf=x+cont;
        j=x;
        for(;;)
        {
          if(buf>tmp)
            break;
          Oled_GRAM[y+flg][j++]=Oled_GRAM[y+flg][buf];  
          Oled_GRAM[y+flg][buf++]=0;
        }
      }
  
    }
    else if(roll_mode==RIGHT)
    {
      for(flg=0;flg<height;flg++)
      {
        buf=x+width-cont-1;
        j=x+width-1;
        for(;;)
        {
          if(buf<x)
            break;
          Oled_GRAM[y+flg][j--]=Oled_GRAM[y+flg][buf];  
          Oled_GRAM[y+flg][buf--]=0;
        }
      }
    }
    choose_refresh(x,y,width,height);
    yield();
  }
  
}


void Oled::display_on(void)
{
  display.ssd1306_command(0X8D);  //SET DCDC命令
  display.ssd1306_command(0X14);  //DCDC ON
  display.ssd1306_command(0XAF);  //DISPLAY ON
}
//关闭OLED显示     
void Oled::display_off(void)
{
  display.ssd1306_command(0X8D);  //SET DCDC命令
  display.ssd1306_command(0X10);  //DCDC OFF
  display.ssd1306_command(0XAE);  //DISPLAY OFF
} 

