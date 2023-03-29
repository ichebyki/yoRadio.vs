#ifndef displayLC1602_h
#define displayLC1602_h
#include "../core/options.h"

#include "Arduino.h"
#include "tools/l10n.h"

#define DSP_NOT_FLIPPED
#define DSP_LCD

#define CHARWIDTH   1
#define CHARHEIGHT  1

#include "widgets/widgets.h"
#include "widgets/pages.h"

#if DSP_MODEL==DSP_2004 || DSP_MODEL==DSP_2004I2C
  #define LCD_2004
#endif

#if DSP_MODEL==DSP_1602I2C || DSP_MODEL==DSP_2004I2C
  #define LCD_I2C
  #include "../LiquidCrystalI2C/LiquidCrystalI2CEx.h"
#else
  #include <LiquidCrystal.h>
#endif

#ifdef LCD_I2C
  #ifdef LCD_2004
    #define DSP_INIT LiquidCrystal_I2C(SCREEN_ADDRESS, 20, 4, I2C_SDA, I2C_SCL)
  #else
    #define DSP_INIT LiquidCrystal_I2C(SCREEN_ADDRESS, 16, 2, I2C_SDA, I2C_SCL)
  #endif
#else
  #define DSP_INIT LiquidCrystal(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7)
#endif

#ifdef LCD_2004
  #if __has_include("conf/displayLCD2004conf_custom.h")
    #include "conf/displayLCD2004conf_custom.h"
  #else
    #include "conf/displayLCD2004conf.h"
  #endif
#else
  #if __has_include("conf/displayLCD1602conf_custom.h")
    #include "conf/displayLCD1602conf_custom.h"
  #else
    #include "conf/displayLCD1602conf.h"
  #endif
#endif

#define BOOT_PRG_COLOR    0x1
#define BOOT_TXT_COLOR    0x1
#define PINK              0x1

/* not used required */
#define bootLogoTop     0
const char           rssiFmt[]    PROGMEM = "";
const MoveConfig    clockMove     PROGMEM = { 0, 0, -1 };
const MoveConfig   weatherMove    PROGMEM = { 0, 0, -1 };
const MoveConfig   weatherMoveVU    PROGMEM = { 0, 0, -1 };
const char  const_lcdApMode[]    PROGMEM = "YORADIO AP MODE";
const char  const_lcdApName[]    PROGMEM = "AP NAME: ";
const char  const_lcdApPass[]    PROGMEM = "PASSWORD: ";

#ifdef LCD_I2C
  class DspCore: public LiquidCrystal_I2C {
#else
  class DspCore: public LiquidCrystal {
#endif
#include "tools/commongfx.h"
};

extern DspCore dsp;

#endif
