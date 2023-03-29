#ifndef displaySSD1322_h
#define displaySSD1322_h
#include "../core/options.h"

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include "../SSD1322/SSD1322.h"

#if CLOCKFONT_MONO
  #include "fonts/DS_DIGI28pt7b_mono.h"                          // https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
#else
  #include "fonts/DS_DIGI28pt7b.h"
#endif
#include "tools/l10n.h"

#define CHARWIDTH   6
#define CHARHEIGHT  8

#define DSP_OLED

typedef GFXcanvas1 Canvas;
#include "widgets/widgets.h"
#include "widgets/pages.h"

#if __has_include("conf/displaySSD1322conf_custom.h")
  #include "conf/displaySSD1322conf_custom.h"
#else
  #include "conf/displaySSD1322conf.h"
#endif

class DspCore: public Jamis_SSD1322 {
#include "tools/commongfx.h"
};

extern DspCore dsp;

/*
 * OLED COLORS
 */
#define BOOT_PRG_COLOR    WHITE
#define BOOT_TXT_COLOR    WHITE
#define PINK              WHITE
#define SILVER            WHITE
#define TFT_BG            BLACK
#define TFT_FG            WHITE
#define TFT_LOGO          WHITE

#endif
