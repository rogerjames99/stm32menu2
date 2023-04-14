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

/*
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
//joystick button pin
#define joyBtn 4
keyMap btnsMap[]={{-joyBtn,defaultNavCodes[enterCmd].ch}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> btns(btnsMap);// 1 is the number of keys
MENU_INPUTS(in,&ay,&btns,&menuSerialIn);
*/
#include <Arduino.h>

#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

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
// The following macros define the structure of the menu system.
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
    ,OP("Option 1",doNothing,noEvent)
    ,OP("Option 2",doNothing,noEvent)
);

// Define pins for navigation buttons
#define UP_BUTTON D2
#define DOWN_BUTTON D4
#define SELECT_BUTTON D7
#define ESC_BUTTON D8
#define NUMBER_OF_BUTTONS 4

//negative pin numbers use internal pull-up
keyMap btnsMap[]={{-UP_BUTTON,defaultNavCodes[upCmd].ch}
                ,{-DOWN_BUTTON,defaultNavCodes[downCmd].ch} 
                ,{-SELECT_BUTTON,defaultNavCodes[enterCmd].ch} 
                ,{-ESC_BUTTON,defaultNavCodes[escCmd].ch} 
                };

keyIn<NUMBER_OF_BUTTONS> btns(btnsMap);

MENU_INPUTS(in,&btns);

// Define outputs
#define MAX_DEPTH 1

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
// I cannot use macros for the SSD1306AsciiOut driver. Some fundamental parts of the underlying
// structure appears not to have been properly implemented yet. My attemmpt at a work around
// hit a brick wall.
const panel panels[] MEMMODE = {{0, 0, 128 / fontW, 32 / fontH}};
navNode* nodes[sizeof(panels) / sizeof(panel)]; 
panelsList pList(panels, nodes, 1); //a list of panels and nodes
idx_t tops[MAX_DEPTH] = {0}; //store cursor positions for each level
SSD1306AsciiOut outOLED(&oled, tops, pList, 8, 1+((fontH-1)>>3) ); //oled output device menu driver
menuOut* constMEM outputs[] MEMMODE = {&outOLED}; //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut*)); //outputs list
#endif

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void setup() {
    btns.begin(); // Does all the setup for mapping gpio btns to simualted keystrokes.
    
    // Blinky
    pinMode(LED, OUTPUT);

    // Not teensy so actual setup needed
    Serial.begin(9600);
    while(!Serial);

    Serial.printf("\nstm32menu2\n");
    Serial.flush();

    // Initialise the I2C and the OLED display
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
    // Blinky
    digitalToggle(LED);
    delay(200);

    // Poll for button presses
    nav.poll();
}

