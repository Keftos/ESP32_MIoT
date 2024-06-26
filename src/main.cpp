/* *****************************************************************
 *接入米家实现小爱同学控制led
 *使用Blinker平台，手机app按钮控制
 *PIN32触摸开关LED
 *OLED显示网络时间和引脚变化状态
 * *****************************************************************/

#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET

#include <Blinker.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ntptime.h"

// 屏幕分辨率
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 定义测试引脚
#define GPIO_TEST 4

#define BUTTON_1 "btn-abc"               // 定义按钮
BlinkerButton Button1((char *)BUTTON_1); // 初始化BlinkerButton对象

// blinker平台key以及wifi信息
/****************************/
char auth[] = "0aecdf657f20";

#define wifichoose 1 // 选择使用场景  0：家 1：公司

#if (wifichoose == 0)
#define SSID "QQQQ"//替换成自己的wifi名称和密码
#define PSWD "000000000"

#elif (wifichoose == 1)
#define SSID "qqqq"//替换成自己的wifi名称和密码
#define PSWD "11111111"

#endif

char ssid[] = SSID;
char pswd[] = PSWD;
/****************************/

// 电源控制变量
bool oState = false;
int io4_hl = 0;

unsigned long currentMillis = 0;  // 存储当前时间
unsigned long previousMillis = 0; // 存储上次更新显示的时间
const long interval = 2000;       // 显示状态的时间间隔，单位：毫秒
bool showState = false;           // 是否显示状态
String currentState = "";         // 当前状态信息
bool initialized = false;         // 标记是否完成初始化

// 函数声明部分
void update_ntp();
void button1_callback(const String &state);
void miotPowerState(const String &state);
void miotQuery(int32_t queryCode);
void displayState(String state);
void read_io4_oled();
void TouchEvent();

void setup()
{
    Serial.begin(115200);

    // Pin: T0(GPIO4), 函数指针:TouchEvent, 阈值: 40
    touchAttachInterrupt(T9, TouchEvent, 100); // pin32 touch中断

    pinMode(GPIO_TEST, OUTPUT);
    digitalWrite(GPIO_TEST, LOW);

    // blinker
    /*********************************/
    Blinker.begin(auth, ssid, pswd);

    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachQuery(miotQuery);
    Button1.attach(button1_callback);
    /*************************/

    // oled
    /********************************/
    Serial.println("OLED FeatherWing test2");
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
    Serial.println("OLED begun2");

    // Show image buffer on the display hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will display the splashscreen.
    display.display();
    delay(1000);

    // Clear the buffer.
    display.clearDisplay();
    display.display();
    /********************************/

    // 网络时间
    /********************************/
    // 请求网络时间
    initNtp();
    /********************************/
}

unsigned long last_ntp = 0;
const long interval_ntp = 1000; // 时间的刷新频率（毫秒）
void loop()
{
    // 获取单片机启动至今的毫秒数
    currentMillis = millis();
    // Serial.printf("touch:%d\r\n", touchRead(T9));

    read_io4_oled(); // 更新时间并显示

    // 点灯科技
    Blinker.run();
}

// 按钮1回调函数
void button1_callback(const String &state)
{
    Serial.println("点击按钮");
    digitalWrite(GPIO_TEST, !digitalRead(GPIO_TEST)); // 翻转引脚
    BLINKER_LOG("get button state: ", state);

    if (state == BLINKER_CMD_BUTTON_TAP)
    {
        BLINKER_LOG("Button tap!");

        Button1.icon("fas fa-air-freshener");
        Button1.color("#00F5FF");
        Button1.text("开关");
        Button1.print();
    }
}

// 设备控制  高电平触发
void miotPowerState(const String &state)
{
    Serial.println("正在控制设备");

    if (state == BLINKER_CMD_ON)
    {
        digitalWrite(GPIO_TEST, HIGH);
        Serial.println("设备已打开");
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();

        oState = true;
    }
    else if (state == BLINKER_CMD_OFF)
    {
        digitalWrite(GPIO_TEST, LOW);
        Serial.println("设备已关闭");
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();

        oState = false;
    }
}

// 设备状态查询
void miotQuery(int32_t queryCode)
{
    Serial.println("正在查询电源状态");
    switch (queryCode)
    {
    case BLINKER_CMD_QUERY_ALL_NUMBER:
        Serial.printf("当前查询的所有电源状态为:%s\n", oState ? "on" : "off");

        BlinkerMIOT.powerState(oState ? "on" : "off"); // 查询电源状态
        BlinkerMIOT.print();                           // 向小爱发送状态
        break;
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER:
        Serial.printf("当前查询的所有电源状态为:%s\n", oState ? "on" : "off");

        BlinkerMIOT.powerState(oState ? "on" : "off");
        BlinkerMIOT.print();
        break;
    default:
        Serial.println("其他查询");

        BlinkerMIOT.powerState(oState ? "on" : "off");
        BlinkerMIOT.print();
        break;
    }
}

void update_ntp()
{
    

    // update ntp 时间
    if (last_ntp == 0 || currentMillis - last_ntp >= interval_ntp)
    {
        last_ntp = currentMillis;
        loopNtp();

        display.clearDisplay(); // 清屏
        display.setTextColor(SSD1306_WHITE);

        display.setTextSize(2);
        display.setCursor(5, 10);
        display.print(dt.localDate);

        display.setCursor(18, 40);
        display.print(dt.localTime);
        display.display(); // actually display all of the above
    }
}

// 读取引脚电平
void read_io4_oled()
{
    io4_hl = digitalRead(GPIO_TEST);

    if (io4_hl == HIGH && currentState != "State: ON")
    {
        currentState = "State: ON";
        showState = true;
        previousMillis = currentMillis;
    }
    else if (io4_hl == LOW && currentState != "State: OFF")
    {
        currentState = "State: OFF";
        showState = true;
        previousMillis = currentMillis;
    }

    if (showState)
    {
        displayState(currentState);
        if (currentMillis - previousMillis >= interval)
        {
            showState = false;
        }
    }
    else
    {
        update_ntp();
    }
}

//显示引脚状态
void displayState(String state)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 10);
    display.println(state);
    display.display();
}

const int debounceDelay = 3000;  // 防抖延迟3000毫秒
volatile unsigned long lastTouchTime = 0;
void TouchEvent()
{
    if(currentMillis  - lastTouchTime > debounceDelay)
    {
        digitalWrite(GPIO_TEST, !digitalRead(GPIO_TEST)); // 翻转引脚
        lastTouchTime = currentMillis;  // 记录这次触发时间
    }
    
}
