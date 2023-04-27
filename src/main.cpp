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
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

// Local scope
#ifdef ARDUINO_TEENSY41
    static constexpr int LED = LED_BUILTIN;
    static constexpr int UP_BUTTON = A0;
    static constexpr int DOWN_BUTTON = A1;
    static constexpr int SELECT_BUTTON = A2;
    static constexpr int ESC_BUTTON = A3;
#else
    #ifdef ARDUINO_NUCLEO_F103RB
        static constexpr int LED = D13;
        static constexpr int UP_BUTTON = D2;
        static constexpr int DOWN_BUTTON = D4;
        static constexpr int SELECT_BUTTON = D7;
        static constexpr int ESC_BUTTON = D8;
    #else
        #pragma GCC error "Unknown board type"
        #include "/stophere" // Trick to stop further compilation I hope this does not exist!
    #endif
#endif
static constexpr int NUMBER_OF_BUTTONS = 4;
static int ledCtrl=LOW;
static int test=55;
static int selTest=0;
static int chooseTest=-1;
static int last_millis = 0;
static bool blinking = false;
static constexpr int BLINK_EVERY = 10000; // Milliseconds
static constexpr int BLINK_DURATION = 100; // Milliseconds

using namespace Menu;

// Define font
#define menuFont font5x7
#define fontW 5
#define fontH 8

constexpr int OLED_SDA=4;
constexpr int OLED_SDC=5;
static constexpr int I2C_ADDRESS = 0x3C;

// Instantiate the display
static SSD1306AsciiWire oled;

// Menu action functions
static result action1(eventMask e,navNode& nav, prompt &item) {
  Serial.print("action1 event:");
  Serial.println(e);
  Serial.flush();
  return proceed;
}

static result action2(eventMask e) {
  Serial.print("action2 event:");
  Serial.println(e);
  Serial.flush();
  return quit;
}

static result showEvent(eventMask e) {
  Serial.print("event: ");
  Serial.println(e);
  return proceed;
}

/*
static result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o.print("alert test");
    o.setCursor(0,1);
    o.print("[select] to continue...");
  }
  return proceed;
*/

static result doAlert(eventMask e, prompt &item);

static result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}

static result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);;
  }
};

// Define menu structure
// The following macros define the structure of the menu system.
SELECT(selTest,selMenu,"Select",doNothing,noEvent,wrapStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,wrapStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,wrapStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

MENU(subMenu,"Sub-Menu",doNothing,anyEvent,wrapStyle
  ,OP("Sub1",showEvent,enterEvent)
  ,OP("Sub2",showEvent,enterEvent)
  ,OP("Sub3",showEvent,enterEvent)
  ,altOP(altPrompt,"",showEvent,enterEvent)
  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

//negative pin numbers use internal pull-up
keyMap btnsMap[]={{-UP_BUTTON,defaultNavCodes[upCmd].ch}
                ,{-DOWN_BUTTON,defaultNavCodes[downCmd].ch} 
                ,{-SELECT_BUTTON,defaultNavCodes[enterCmd].ch} 
                ,{-ESC_BUTTON,defaultNavCodes[escCmd].ch} 
                };

keyIn<NUMBER_OF_BUTTONS> btns(btnsMap);

MENU_INPUTS(in,&btns);

// Define outputs
#define MAX_DEPTH 2

#define VAR_SSD1306ASCII_OUT(id,md,n,device,resX,resY,...)\
Menu::idx_t id##Tops##n[md];\
PANELS(id##Panels##n,__VA_ARGS__);\
Menu::SSD1306AsciiOut id##n(&device,id##Tops##n,id##Panels##n,resX,resY);

#define REF_SSD1306ASCII_OUT(id,md,n,...) &id##n,

// This works for me on the cheap 128x32 display I have - I suspect X and Y are reversed.
MENU_OUTPUTS(out, MAX_DEPTH, SSD1306ASCII_OUT(oled,
                                            fontH,
                                            1+((fontH-1)>>3),
                                            {0, 0, 128 / fontW, 32 / fontH}),
                                            NONE);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

// This has to be after NAVROOT because the nav object is declared by the NAVROOT macro.
result doAlert(eventMask e, prompt &item) {
 // nav.idleOn(alert);
  return proceed;
}

void setup() {
    btns.begin(); // Does all the setup for mapping gpio btns to simulated keystrokes.
    
    // Blinky
    pinMode(LED, OUTPUT);
    digitalToggle(LED);
    delay(1000);
    digitalToggle(LED);

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
    oled.print("menutest");
    oled.setCursor(0, 2);
    oled.print("SSD1306Ascii");
    delay(2000);
    oled.clear();
}

void loop() {
    // Minimise use of delays in loop
    int current_millis = millis();
    if (blinking)
    {
        if ((current_millis - last_millis) > BLINK_DURATION)
        {
            last_millis = current_millis;
            digitalToggle(LED);
            blinking = false;
        }
    }
    else
    {
        if ((current_millis - last_millis) > BLINK_EVERY)
        {
            last_millis = current_millis;
            digitalToggle(LED);
            blinking = true;
        }
    }

    // Poll for button presses
    nav.poll();
}

