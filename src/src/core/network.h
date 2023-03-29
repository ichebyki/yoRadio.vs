#ifndef network_h
#define network_h
#include <Ticker.h>
#include "time.h"

#define apSsid      "yoRadioAP"
#define apPassword  "12345987"
//#define TSYNC_DELAY 10800000    // 1000*60*60*3 = 3 hours
#define TSYNC_DELAY       3600000     // 1000*60*60   = 1 hour
#define WEATHER_STRING_L  254

enum n_Status_e { CONNECTED, SOFT_AP, FAILED };

class Network {
  public:
    n_Status_e status;
    struct tm timeinfo;
    bool firstRun, forceTimeSync, forceWeather;
    //uint8_t tsFailCnt, wsFailCnt;
  public:
    Network() {};
    void begin();
    void requestTimeSync(bool withTelnetOutput=false, uint8_t clientId=0);
    void requestWeatherSync();
    Ticker ctimer;
    char *weatherBuf;
    bool trueWeather;
  private:
    Ticker rtimer;
    void raiseSoftAP();
};

extern Network network;

extern __attribute__((weak)) void network_on_connect();

#endif
