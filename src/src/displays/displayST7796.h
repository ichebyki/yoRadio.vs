#ifndef displayST7796_h
#define displayST7796_h
#include "../core/options.h"

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include "../Adafruit_ST7796S/Adafruit_ST7796S_kbv.h"

#if CLOCKFONT_MONO
  #include "fonts/DS_DIGI56pt7b_mono.h"        // https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
#else
  #include "fonts/DS_DIGI56pt7b.h"
#endif
#include "tools/l10n.h"

#define CHARWIDTH   6
#define CHARHEIGHT  8

typedef GFXcanvas16 Canvas;
#include "widgets/widgets.h"
#include "widgets/pages.h"

#if __has_include("conf/displayST7796conf_custom.h")
  #include "conf/displayST7796conf_custom.h"
#else
  #include "conf/displayST7796conf.h"
#endif

#define BOOT_PRG_COLOR    0xE68B
#define BOOT_TXT_COLOR    0xFFFF
#define PINK              0xF97F

class DspCore: public Adafruit_ST7796S_kbv {
#include "tools/commongfx.h"
};

extern DspCore dsp;

#endif
