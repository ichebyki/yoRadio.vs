#include "network.h"
#include "WiFi.h"
#include <WiFiMulti.h>
#include "display.h"
#include "options.h"
#include "config.h"
#include "telnet.h"
#include "netserver.h"
#include "player.h"

Network network;
WiFiMulti wifiMulti;

TaskHandle_t syncTaskHandle;
bool getWeather(char *wstr);
void doSync(void * pvParameters);

void ticks() {
  static const uint16_t weatherSyncInterval=1800;
  //static const uint16_t weatherSyncIntervalFail=10;
  static const uint16_t timeSyncInterval=3600;
  static uint16_t timeSyncTicks = 0;
  static uint16_t weatherSyncTicks = 0;
  static bool divrssi;
  timeSyncTicks++;
  weatherSyncTicks++;
  divrssi = !divrssi;
  if(network.forceTimeSync || network.forceWeather){
    xTaskCreatePinnedToCore(doSync, "doSync", 1024 * 4, NULL, 0, &syncTaskHandle, 0);
  }
  if(timeSyncTicks >= timeSyncInterval){
    timeSyncTicks=0;
    network.forceTimeSync = true;
  }
  if(weatherSyncTicks >= weatherSyncInterval){
    weatherSyncTicks=0;
    network.forceWeather = true;
  }
  
  if(network.timeinfo.tm_year>100) {
    network.timeinfo.tm_sec++;
    mktime(&network.timeinfo);
    display.putRequest(CLOCK);
  }
  if(player.isRunning() && config.store.play_mode==PM_SDCARD) netserver.requestOnChange(SDPOS, 0);
  if(divrssi) {
    int rs = WiFi.RSSI();
    netserver.setRSSI(rs);
    netserver.requestOnChange(NRSSI, 0);
    display.putRequest(DSPRSSI, rs);
    
  }
}

#define DBGAP false
/***********************************************************************************************************************
 *                                         C O N N E C T   TO   W I F I *
 ***********************************************************************************************************************/
bool connectToWiFi() {
  String s_ssid = "", s_password = "", s_info = "";
  wifiMulti.addAP("yyyy", "yyyy"); // SSID and PW in code
  WiFi.setHostname("yoRadio.vs");
  if (psramFound())
    WiFi.useStaticBuffers(true);
  File file = SPIFFS.open(SSIDS_PATH,
                          "r"); // try credentials
  if (file) {
    String str = "";
    while (file.available()) {
      str = file.readStringUntil('\n'); // read the line
      if (str[0] == '*')
        continue; // ignore this, goto next line
      if (str[0] == '\n')
        continue; // empty line
      if (str[0] == ' ')
        continue; // space as first char
      if (str.indexOf('\t') < 0)
        continue; // no tab
      str += "\t";
      uint p = 0, q = 0;
      s_ssid = "", s_password = "", s_info = "";
      for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\t') {
          if (p == 0)
            s_ssid = str.substring(q, i);
          if (p == 1)
            s_password = str.substring(q, i);
          if (p == 2)
            s_info = str.substring(q, i);
          p++;
          i++;
          q = i;
        }
      }
      // log_i("s_ssid=%s  s_password=%s  s_info=%s", s_ssid.c_str(),
      // s_password.c_str(), s_info.c_str());
      if (s_ssid == "")
        continue;
      if (s_password == "")
        continue;
      wifiMulti.addAP(s_ssid.c_str(), s_password.c_str());
    }
    file.close();
  }

  // WiFi.begin(ssid, password);
  wifiMulti.run();

  if (WiFi.isConnected()) {
    // Connected to WiFi network
    Serial.println("Connected to WiFi");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.setSleep(false);
    return true;
  }

  Serial.println("WiFi credentials are not correct or other problem\n");
  return false; // can't connect to any network
}

void Network::begin() {
  BOOTLOG("network.begin");
  config.initNetwork();
  ctimer.detach();
  forceTimeSync = forceWeather = true;
  if (config.ssidsCount == 0 || DBGAP) {
    raiseSoftAP();
    return;
  }
  byte ls = (config.store.lastSSID == 0 || config.store.lastSSID > config.ssidsCount) ? 0 : config.store.lastSSID - 1;
  byte startedls = ls;
  byte errcnt = 0;
  WiFi.mode(WIFI_STA);

  // Connect to the best WiFi
  if (!connectToWiFi()) {
    raiseSoftAP();
    return;
  }

  Serial.println(".");
  Serial.println("##[BOOT]#\tdone");
  if(LED_BUILTIN!=255) digitalWrite(LED_BUILTIN, LOW);
  status = CONNECTED;
  WiFi.setSleep(false);
  
  weatherBuf=NULL;
  trueWeather = false;
  #if (DSP_MODEL!=DSP_DUMMY || defined(USE_NEXTION)) && !defined(HIDE_WEATHER)
    weatherBuf = (char *) malloc(sizeof(char) * WEATHER_STRING_L);
    memset(weatherBuf, 0, WEATHER_STRING_L);
  #endif
  
  if(strlen(config.store.sntp1)>0 && strlen(config.store.sntp2)>0){
    configTime(config.store.tzHour * 3600 + config.store.tzMin * 60, config.getTimezoneOffset(), config.store.sntp1, config.store.sntp2);
  }else if(strlen(config.store.sntp1)>0){
    configTime(config.store.tzHour * 3600 + config.store.tzMin * 60, config.getTimezoneOffset(), config.store.sntp1);
  }
  ctimer.attach(1, ticks);
  if (network_on_connect) network_on_connect();
}

void Network::requestTimeSync(bool withTelnetOutput, uint8_t clientId) {
  if (withTelnetOutput) {
    char timeStringBuff[50];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    if (config.store.tzHour < 0) {
      telnet.printf(clientId, "##SYS.DATE#: %s%03d:%02d\n> ", timeStringBuff, config.store.tzHour, config.store.tzMin);
    } else {
      telnet.printf(clientId, "##SYS.DATE#: %s+%02d:%02d\n> ", timeStringBuff, config.store.tzHour, config.store.tzMin);
    }
  }
}

void rebootTime() {
  ESP.restart();
}

void Network::raiseSoftAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid, apPassword);
  Serial.println("##[BOOT]#");
  BOOTLOG("************************************************");
  BOOTLOG("Running in AP mode");
  BOOTLOG("Connect to AP %s with password %s", apSsid, apPassword);
  BOOTLOG("and go to http:/192.168.4.1/ to configure");
  BOOTLOG("************************************************");
  status = SOFT_AP;
  if(config.store.softapdelay>0)
    rtimer.once(config.store.softapdelay*60, rebootTime);
}

void Network::requestWeatherSync(){
  display.putRequest(NEWWEATHER);
}


void doSync( void * pvParameters ) {
  static uint8_t tsFailCnt = 0;
  //static uint8_t wsFailCnt = 0;
  if(network.forceTimeSync){
    network.forceTimeSync = false;
    if(getLocalTime(&network.timeinfo)){
      tsFailCnt = 0;
      network.forceTimeSync = false;
      mktime(&network.timeinfo);
      display.putRequest(CLOCK);
      network.requestTimeSync(true);
    }else{
      if(tsFailCnt<4){
        network.forceTimeSync = true;
        tsFailCnt++;
      }else{
        network.forceTimeSync = false;
        tsFailCnt=0;
      }
    }
  }
  if(network.weatherBuf && (strlen(config.store.weatherkey)!=0 && config.store.showweather) && network.forceWeather){
    network.forceWeather = false;
    network.trueWeather=getWeather(network.weatherBuf);
  }
  vTaskDelete( NULL );
}

bool getWeather(char *wstr) {
#if (DSP_MODEL!=DSP_DUMMY || defined(USE_NEXTION)) && !defined(HIDE_WEATHER)
  WiFiClient client;
  const char* host  = "api.openweathermap.org";
  
  if (!client.connect(host, 80)) {
    Serial.println("## OPENWEATHERMAP ###: connection  failed");
    return false;
  }
  char httpget[250] = {0};
  sprintf(httpget, "GET /data/2.5/weather?lat=%s&lon=%s&units=%s&lang=%s&appid=%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", config.store.weatherlat, config.store.weatherlon, weatherUnits, weatherLang, config.store.weatherkey, host);
  client.print(httpget);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 2000UL) {
      Serial.println("## OPENWEATHERMAP ###: client available timeout !");
      client.stop();
      return false;
    }
  }
  timeout = millis();
  String line = "";
  if (client.connected()) {
    while (client.available())
    {
      line = client.readStringUntil('\n');
      if (strstr(line.c_str(), "\"temp\"") != NULL) {
        client.stop();
        break;
      }
      if ((millis() - timeout) > 500)
      {
        client.stop();
        Serial.println("## OPENWEATHERMAP ###: client read timeout !");
        return false;
      }
    }
  }
  if (strstr(line.c_str(), "\"temp\"") == NULL) {
    Serial.println("## OPENWEATHERMAP ###: weather not found !");
    return false;
  }
  char *tmpe;
  char *tmps;
  char *tmpc;
  const char* cursor = line.c_str();
  char desc[120], temp[20], hum[20], press[20], icon[5];

  tmps = strstr(cursor, "\"description\":\"");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: description not found !"); return false;}
  tmps += 15;
  tmpe = strstr(tmps, "\",\"");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: description not found !"); return false;}
  strlcpy(desc, tmps, tmpe - tmps + 1);
  cursor = tmpe + 2;
  
  // "ясно","icon":"01d"}],
  tmps = strstr(cursor, "\"icon\":\"");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: icon not found !"); return false;}
  tmps += 8;
  tmpe = strstr(tmps, "\"}");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: icon not found !"); return false;}
  strlcpy(icon, tmps, tmpe - tmps + 1);
  cursor = tmpe + 2;
  
  tmps = strstr(cursor, "\"temp\":");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: temp not found !"); return false;}
  tmps += 7;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: temp not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + 1);
  cursor = tmpe + 1;
  float tempf = atof(temp);

  tmps = strstr(cursor, "\"feels_like\":");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: feels_like not found !"); return false;}
  tmps += 13;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: feels_like not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + 1);
  cursor = tmpe + 2;
  float tempfl = atof(temp); (void)tempfl;

  tmps = strstr(cursor, "\"pressure\":");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: pressure not found !"); return false;}
  tmps += 11;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: pressure not found !"); return false;}
  strlcpy(press, tmps, tmpe - tmps + 1);
  cursor = tmpe + 2;
  int pressi = (float)atoi(press) / 1.333;
  
  tmps = strstr(cursor, "humidity\":");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: humidity not found !"); return false;}
  tmps += 10;
  tmpe = strstr(tmps, ",\"");
  tmpc = strstr(tmps, "}");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: humidity not found !"); return false;}
  strlcpy(hum, tmps, tmpe - tmps + (tmpc>tmpe?1:0));
  
  tmps = strstr(cursor, "\"grnd_level\":");
  bool grnd_level_pr = (tmps != NULL);
  if(grnd_level_pr){
    tmps += 13;
    tmpe = strstr(tmps, ",\"");
    if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: grnd_level not found !"); return false;}
    strlcpy(press, tmps, tmpe - tmps + 1);
    cursor = tmpe + 2;
    pressi = (float)atoi(press) / 1.333;
  }
  
  tmps = strstr(cursor, "\"speed\":");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: wind speed not found !"); return false;}
  tmps += 8;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: wind speed not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + 1);
  cursor = tmpe + 1;
  float wind_speed = atof(temp); (void)wind_speed;
  
  tmps = strstr(cursor, "\"deg\":");
  if (tmps == NULL) { Serial.println("## OPENWEATHERMAP ###: wind deg not found !"); return false;}
  tmps += 6;
  tmpe = strstr(tmps, ",\"");
  if (tmpe == NULL) { Serial.println("## OPENWEATHERMAP ###: wind deg not found !"); return false;}
  strlcpy(temp, tmps, tmpe - tmps + 1);
  cursor = tmpe + 1;
  int wind_deg = atof(temp)/22.5;
  if(wind_deg<0) wind_deg = 16+wind_deg;
  
  
  #ifdef USE_NEXTION
    nextion.putcmdf("press_txt.txt=\"%dmm\"", pressi);
    nextion.putcmdf("hum_txt.txt=\"%d%%\"", atoi(hum));
    char cmd[30];
    snprintf(cmd, sizeof(cmd)-1,"temp_txt.txt=\"%.1f\"", tempf);
    nextion.putcmd(cmd);
    int iconofset;
    if(strstr(icon,"01")!=NULL)      iconofset = 0;
    else if(strstr(icon,"02")!=NULL) iconofset = 1;
    else if(strstr(icon,"03")!=NULL) iconofset = 2;
    else if(strstr(icon,"04")!=NULL) iconofset = 3;
    else if(strstr(icon,"09")!=NULL) iconofset = 4;
    else if(strstr(icon,"10")!=NULL) iconofset = 5;
    else if(strstr(icon,"11")!=NULL) iconofset = 6;
    else if(strstr(icon,"13")!=NULL) iconofset = 7;
    else if(strstr(icon,"50")!=NULL) iconofset = 8;
    else                             iconofset = 9;
    nextion.putcmd("cond_img.pic", 50+iconofset);
    nextion.weatherVisible(1);
  #endif
  
  Serial.printf("## OPENWEATHERMAP ###: description: %s, temp:%.1f C, pressure:%dmmHg, humidity:%s%%\n", desc, tempf, pressi, hum);
  #ifdef WEATHER_FMT_SHORT
  sprintf(wstr, weatherFmt, tempf, pressi, hum);
  #else
    #if EXT_WEATHER
      sprintf(wstr, weatherFmt, desc, tempf, tempfl, pressi, hum, wind_speed, wind[wind_deg]);
    #else
      sprintf(wstr, weatherFmt, desc, tempf, pressi, hum);
    #endif
  #endif
  network.requestWeatherSync();
  return true;
#endif // if (DSP_MODEL!=DSP_DUMMY || defined(USE_NEXTION)) && !defined(HIDE_WEATHER)
  return false;
}
