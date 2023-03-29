#ifndef _my_theme_h
#define _my_theme_h

/*
    Theming of color displays
    DSP_ST7735, DSP_ST7789, DSP_ILI9341, DSP_GC9106, DSP_ILI9225, DSP_ST7789_240
    ***********************************************************************
    *    !!! This file must be in the root directory of the sketch !!!    *
    ***********************************************************************
    Uncomment (remove double slash //) from desired line to apply color
*/
#define ENABLE_THEME
#ifdef  ENABLE_THEME

/*----------------------------------------------------------------------------------------------------------------*/
/*       | COLORS             |   values (0-255)  |                                                               */
/*       | color name         |    R    G    B    |                                                               */
/*----------------------------------------------------------------------------------------------------------------*/
//#define COLOR_BACKGROUND        255, 255,   0     /*  background                                                */
#define COLOR_STATION_NAME        250, 220,   0     /*  station name                                              */
#define COLOR_STATION_BG            0,   0,   0     /*  station name background                                   */
#define COLOR_STATION_FILL        195, 195, 195     /*  station name fill background                              */
#define COLOR_SNG_TITLE_1         255, 255, 255     /*  first title                                               */
#define COLOR_SNG_TITLE_2         185, 185, 185     /*  second title                                              */
#define COLOR_WEATHER               0, 200, 255     /*  weather string                                            */
#define COLOR_VU_MAX              210,   0,   0     /*  max of VU meter                                           */
#define COLOR_VU_MIN                0, 128,   0     /*  min of VU meter                                           */
//#define COLOR_CLOCK              60, 224,  33     /*  clock color                                               */
//#define COLOR_SECONDS             0, 255, 255     /*  seconds color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)      */
//#define COLOR_DAY_OF_W          255,   0,   0     /*  day of week color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)  */
//#define COLOR_DATE                0,   0, 255     /*  date color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)         */
//#define COLOR_HEAP              255, 168, 162     /*  heap string                                               */
#define COLOR_BUFFER              255, 105, 180     /*  buffer line                                               */
//#define COLOR_IP                 41, 189, 207     /*  ip address                                                */
//#define COLOR_VOLUME_VALUE      165, 162, 132     /*  volume string (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)      */
//#define COLOR_RSSI              255, 148, 156     /*  rssi                                                      */
//#define COLOR_VOLBAR_OUT        198,  93,   0     /*  volume bar outline                                        */
//#define COLOR_VOLBAR_IN         189, 189, 189     /*  volume bar fill                                           */
//#define COLOR_DIGITS            100, 100, 255     /*  volume / station number                                   */
//#define COLOR_DIVIDER             0, 255,   0     /*  divider color (DSP_ST7789, DSP_ILI9341, DSP_ILI9225)      */
#define COLOR_BITRATE             250, 250, 100     /*  bitrate                                                   */
#define COLOR_PL_CURRENT          255, 215,   0     /*  playlist current item                                     */
#define COLOR_PL_CURRENT_BG         0,   0,   0     /*  playlist current item background                          */
#define COLOR_PL_CURRENT_FILL       0,   0,   0     /*  playlist current item fill background                     */
#define COLOR_PLAYLIST_0          165, 165, 165     /*  playlist string 0                                         */
#define COLOR_PLAYLIST_1          145, 145, 145     /*  playlist string 1                                         */
#define COLOR_PLAYLIST_2          120, 120, 120     /*  playlist string 2                                         */
#define COLOR_PLAYLIST_3          100, 100, 100     /*  playlist string 3                                         */
#define COLOR_PLAYLIST_4           80,  80,  80     /*  playlist string 4                                         */


#endif  /* #ifdef  ENABLE_THEME */
#endif  /* #define _my_theme_h  */
