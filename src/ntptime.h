//从ntp服务器更新时间(依赖 ntp 库)
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);

void loopNtp();

struct dt_data {
  String localDate = "";
  String localTime = "";
  String y;
  String m;
  String d;
  String h;
  String i;
  String s;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} dt;

/**
   获取时间
   做一些准备工作（在setup() 函数中调用）
*/
void initNtp() {
  timeClient.begin();
  //28800 = +8时区（我们的北京时间）
  //timeClient.setTimeOffset(28800);
  loopNtp();
}

/**
   刷新时间
*/
void loopNtp() {
  timeClient.update();
   Serial.print("localTime (getFormattedTime): ");
  Serial.println(timeClient.getFormattedTime());
  
  //获取unix时间戳（1970年至今的总秒数）
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("epochTime: ");
  Serial.println(epochTime);

  //格式化得到 时:分:秒
  dt.localTime = timeClient.getFormattedTime();
  // Serial.print("localTime: ");
  // Serial.println(dt.localTime);

  //重新计算得到 年-月-日
  time_t rawtime = epochTime;
  struct tm * ti;
  ti = gmtime (&rawtime);

  dt.year = ti->tm_year + 1900;
  dt.y = String(dt.year);

  dt.month = ti->tm_mon + 1;
  dt.m = dt.month < 10 ? "0" + String(dt.month) : String(dt.month);

  dt.day = ti->tm_mday;
  dt.d = dt.day < 10 ? "0" + String(dt.day) : String(dt.day);

  dt.hours = ti->tm_hour;
  dt.h = dt.hours < 10 ? "0" + String(dt.hours) : String(dt.hours);

  dt.minutes = ti->tm_min;
  dt.i = dt.minutes < 10 ? "0" + String(dt.minutes) : String(dt.minutes);

  dt.seconds = ti->tm_sec;
  dt.s = dt.seconds < 10 ? "0" + String(dt.seconds) : String(dt.seconds);

  //将得到的年月日写入weather_data 结构体
  dt.localDate = dt.y + "-" + dt.m + "-" + dt.d;
}

/**关于日期的计算函数**/

struct LunarDate {
  int year;
  int month;
  int day;
  int leap;
};

String weekly[] = {"*", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"};

const String ChDay[] = {"*", "初一", "初二", "初三", "初四", "初五",
                        "初六", "初七", "初八", "初九", "初十",
                        "十一", "十二", "十三", "十四", "十五",
                        "十六", "十七", "十八", "十九", "二十",
                        "二一", "二二", "二三", "二四", "二五",
                        "二六", "二七", "二八", "二九", "三十"
                       };

const String ChMonth[] = {"*", "正月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "腊月"};

unsigned int LunarCalendarDay;
unsigned int LunarCalendarTable[199] =
{
  0x04AE53, 0x0A5748, 0x5526BD, 0x0D2650, 0x0D9544, 0x46AAB9, 0x056A4D, 0x09AD42, 0x24AEB6, 0x04AE4A, /*1901-1910*/
  0x6A4DBE, 0x0A4D52, 0x0D2546, 0x5D52BA, 0x0B544E, 0x0D6A43, 0x296D37, 0x095B4B, 0x749BC1, 0x049754, /*1911-1920*/
  0x0A4B48, 0x5B25BC, 0x06A550, 0x06D445, 0x4ADAB8, 0x02B64D, 0x095742, 0x2497B7, 0x04974A, 0x664B3E, /*1921-1930*/
  0x0D4A51, 0x0EA546, 0x56D4BA, 0x05AD4E, 0x02B644, 0x393738, 0x092E4B, 0x7C96BF, 0x0C9553, 0x0D4A48, /*1931-1940*/
  0x6DA53B, 0x0B554F, 0x056A45, 0x4AADB9, 0x025D4D, 0x092D42, 0x2C95B6, 0x0A954A, 0x7B4ABD, 0x06CA51, /*1941-1950*/
  0x0B5546, 0x555ABB, 0x04DA4E, 0x0A5B43, 0x352BB8, 0x052B4C, 0x8A953F, 0x0E9552, 0x06AA48, 0x6AD53C, /*1951-1960*/
  0x0AB54F, 0x04B645, 0x4A5739, 0x0A574D, 0x052642, 0x3E9335, 0x0D9549, 0x75AABE, 0x056A51, 0x096D46, /*1961-1970*/
  0x54AEBB, 0x04AD4F, 0x0A4D43, 0x4D26B7, 0x0D254B, 0x8D52BF, 0x0B5452, 0x0B6A47, 0x696D3C, 0x095B50, /*1971-1980*/
  0x049B45, 0x4A4BB9, 0x0A4B4D, 0xAB25C2, 0x06A554, 0x06D449, 0x6ADA3D, 0x0AB651, 0x093746, 0x5497BB, /*1981-1990*/
  0x04974F, 0x064B44, 0x36A537, 0x0EA54A, 0x86B2BF, 0x05AC53, 0x0AB647, 0x5936BC, 0x092E50, 0x0C9645, /*1991-2000*/
  0x4D4AB8, 0x0D4A4C, 0x0DA541, 0x25AAB6, 0x056A49, 0x7AADBD, 0x025D52, 0x092D47, 0x5C95BA, 0x0A954E, /*2001-2010*/
  0x0B4A43, 0x4B5537, 0x0AD54A, 0x955ABF, 0x04BA53, 0x0A5B48, 0x652BBC, 0x052B50, 0x0A9345, 0x474AB9, /*2011-2020*/
  0x06AA4C, 0x0AD541, 0x24DAB6, 0x04B64A, 0x69573D, 0x0A4E51, 0x0D2646, 0x5E933A, 0x0D534D, 0x05AA43, /*2021-2030*/
  0x36B537, 0x096D4B, 0xB4AEBF, 0x04AD53, 0x0A4D48, 0x6D25BC, 0x0D254F, 0x0D5244, 0x5DAA38, 0x0B5A4C, /*2031-2040*/
  0x056D41, 0x24ADB6, 0x049B4A, 0x7A4BBE, 0x0A4B51, 0x0AA546, 0x5B52BA, 0x06D24E, 0x0ADA42, 0x355B37, /*2041-2050*/
  0x09374B, 0x8497C1, 0x049753, 0x064B48, 0x66A53C, 0x0EA54F, 0x06B244, 0x4AB638, 0x0AAE4C, 0x092E42, /*2051-2060*/
  0x3C9735, 0x0C9649, 0x7D4ABD, 0x0D4A51, 0x0DA545, 0x55AABA, 0x056A4E, 0x0A6D43, 0x452EB7, 0x052D4B, /*2061-2070*/
  0x8A95BF, 0x0A9553, 0x0B4A47, 0x6B553B, 0x0AD54F, 0x055A45, 0x4A5D38, 0x0A5B4C, 0x052B42, 0x3A93B6, /*2071-2080*/
  0x069349, 0x7729BD, 0x06AA51, 0x0AD546, 0x54DABA, 0x04B64E, 0x0A5743, 0x452738, 0x0D264A, 0x8E933E, /*2081-2090*/
  0x0D5252, 0x0DAA47, 0x66B53B, 0x056D4F, 0x04AE45, 0x4A4EB9, 0x0A4D4C, 0x0D1541, 0x2D92B5  /*2091-2099*/
};
int MonthAdd[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

LunarDate LunarCalendar(int year, int month, int day)
{
  int Spring_NY, Sun_NY, StaticDayCount;
  int index, flag;
  //Spring_NY 记录春节离当年元旦的天数。
  //Sun_NY 记录阳历日离当年元旦的天数。
  if ( ((LunarCalendarTable[year - 1901] & 0x0060) >> 5) == 1)
    Spring_NY = (LunarCalendarTable[year - 1901] & 0x001F) - 1;
  else
    Spring_NY = (LunarCalendarTable[year - 1901] & 0x001F) - 1 + 31;
  Sun_NY = MonthAdd[month - 1] + day - 1;
  if ( (!(year % 4)) && (month > 2))
    Sun_NY++;
  //StaticDayCount记录大小月的天数 29 或30
  //index 记录从哪个月开始来计算。
  //flag 是用来对闰月的特殊处理。
  //判断阳历日在春节前还是春节后
  if (Sun_NY >= Spring_NY)//阳历日在春节后（含春节那天）
  {
    Sun_NY -= Spring_NY;
    month = 1;
    index = 1;
    flag = 0;
    if ( ( LunarCalendarTable[year - 1901] & (0x80000 >> (index - 1)) ) == 0)
      StaticDayCount = 29;
    else
      StaticDayCount = 30;
    while (Sun_NY >= StaticDayCount)
    {
      Sun_NY -= StaticDayCount;
      index++;
      if (month == ((LunarCalendarTable[year - 1901] & 0xF00000) >> 20) )
      {
        flag = ~flag;
        if (flag == 0)
          month++;
      }
      else
        month++;
      if ( ( LunarCalendarTable[year - 1901] & (0x80000 >> (index - 1)) ) == 0)
        StaticDayCount = 29;
      else
        StaticDayCount = 30;
    }
    day = Sun_NY + 1;
  }
  else //阳历日在春节前
  {
    Spring_NY -= Sun_NY;
    year--;
    month = 12;
    if ( ((LunarCalendarTable[year - 1901] & 0xF00000) >> 20) == 0)
      index = 12;
    else
      index = 13;
    flag = 0;
    if ( ( LunarCalendarTable[year - 1901] & (0x80000 >> (index - 1)) ) == 0)
      StaticDayCount = 29;
    else
      StaticDayCount = 30;
    while (Spring_NY > StaticDayCount)
    {
      Spring_NY -= StaticDayCount;
      index--;
      if (flag == 0)
        month--;
      if (month == ((LunarCalendarTable[year - 1901] & 0xF00000) >> 20))
        flag = ~flag;
      if ( ( LunarCalendarTable[year - 1901] & (0x80000 >> (index - 1)) ) == 0)
        StaticDayCount = 29;
      else
        StaticDayCount = 30;
    }
    day = StaticDayCount - Spring_NY + 1;
  }
  LunarCalendarDay |= day;
  LunarCalendarDay |= (month << 6);

  LunarDate d;
  d.year = year;
  d.month = month;
  d.day = day;
  if (month == ((LunarCalendarTable[year - 1901] & 0xF00000) >> 20))
    d.leap =  1;
  else
    d.leap =  0;
  return d;
}

String outputLunarDate(int year, int month, int day) {
  LunarDate ld = LunarCalendar(year, month, day);
  String str = "";
  if (ld.leap) {
    str += "闰";
  } else {
    str += " ";
  }
  str += ChMonth[ld.month] + ChDay[ld.day];
  return str;
}

String weekOfDate1(int year, int month, int day)
{
  uint8_t n = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7 + 1;
  return weekly[n];
}
