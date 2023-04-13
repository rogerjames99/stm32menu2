/* List of Supported Fonts

  Arial14,
  Arial_bold_14,
  Callibri11,
  Callibri11_bold,
  Callibri11_italic,
  Callibri15,
  Corsiva_12,
  fixed_bold10x15,SSD1306Ascii.ino
  font5x7,
  font8x8,
  Iain5x7,
  lcd5x7,
  Stang5x7,
  System5x7,
  TimesNewRoman16,
  TimesNewRoman16_bold,
  TimesNewRoman16_italic,
  utf8font10x16,
  Verdana12,
  Verdana12_bold,
  Verdana12_italic,
  X11fixed7x14,
  X11fixed7x14B,
  ZevvPeep8x16

*/
#include <Arduino.h>

#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/serialIO.h>

static int LED = D13;

using namespace Menu;

// Define font
#define menuFont font5x7
#define fontW 5
#define fontH 8

constexpr int OLED_SDA=4;
constexpr int OLED_SDC=5;
#define I2C_ADDRESS 0x3C

// Instantiate the display
SSD1306AsciiWire oled;

// Define menu structure
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
    ,OP("Option 1",doNothing,noEvent)
    ,OP("Option 2",doNothing,noEvent)
);

// Define outputs
#define MAX_DEPTH 2

#ifdef USE_MACROS_FOR_MENU_OUTPUTS
#define VAR_SSD1306ASCII_OUT(id,md,n,gfx,color,fontW,fontH,...)\
Menu::idx_t id##Tops##n[md];\
PANELS(id##Panels##n,__VA_ARGS__);\
Menu::SSD1306AsciiOut id##n(gfx,color,id##Tops##n,id##Panels##n,fontW,fontH);

#define REF_SSD1306ASCII_OUT(id,md,n,...) &id##n

MENU_OUTPUTS(out,MAX_DEPTH
  ,SSD1306ASCII_OUT(myMenuDevice, 1, &oled, myMenuDevice_tops, myMenuDevice_pList, 8, 1+((fontH-1)>>3) )
  ,NONE
);
#else
const panel panels[] MEMMODE = {{0, 0, 128 / fontW, 32 / fontH}};
navNode* nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, 1); //a list of panels and nodes
idx_t tops[MAX_DEPTH] = {0, 0}; //store cursor positions for each level
SSD1306AsciiOut outOLED(&oled, tops, pList, 8, 1+((fontH-1)>>3) ); //oled output device menu driver
menuOut* constMEM outputs[] MEMMODE = {&outOLED}; //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut*)); //outputs list
#endif

void setup() {
    pinMode(LED, OUTPUT);
    Serial.begin(9600);
    while(!Serial);
    Serial.printf("\nstm32menu2\n");
    Serial.flush();
    Wire.begin();
    oled.begin(&Adafruit128x32, I2C_ADDRESS);
    oled.setFont(menuFont);
    oled.clear();
    oled.setCursor(0, 0);
    oled.print("stm32menu2");
    oled.setCursor(0, 2);
    oled.print("SSD1306Ascii");
    delay(2000);
    oled.clear();
}

void loop() {
    digitalToggle(LED);
    delay(200);
}

