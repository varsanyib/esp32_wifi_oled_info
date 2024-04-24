#include <Arduino.h>
#include <U8g2lib.h>
#include <Wifi.h>

#define SettingsButton 35
#define SetButton 34
#define StatesNum 3

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1106_128X64_NONAME_1_HW_I2C Display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

char NetworkSSID[] = "<<SSID helye>>";
char NetworkPassword[] = "<<JELSZÓ helye>>";

enum DisplayState {
  MAIN,
  NETWORK,
  NETWORK_SETTINGS
};

DisplayState CurrentState = MAIN;

int SettingsState = LOW;
int SetState = LOW;
unsigned long Tick = 0;

void StateDefiner();
void ShowNetworkConfig();
void ShowNetworkInfo();
void ManageWiFiConnection();

void setup(void) {
  //Initialize Serial Communication Baudrate
  Serial.begin(115200);
  //Initialize Display
  Display.begin();
  //Initialize Buttons
  pinMode(SettingsButton, INPUT);
  pinMode(SetButton, INPUT);
  //Initialize Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(NetworkSSID, NetworkPassword);
}

void loop(void) {
  StateDefiner();
  switch (CurrentState)
  {
  case MAIN:
    break;
  case NETWORK:
    if (millis() / 1000 > Tick)
    {
      Tick = millis() / 1000;
      ShowNetworkInfo();
    }
    break;
  case NETWORK_SETTINGS:
    ShowNetworkConfig();
    if (millis() / 1000 > Tick)
    {
      Tick = millis() / 1000;
      ManageWiFiConnection();
    }
    break;
  default:
    break;
  }
  if (ULONG_MAX - 10 < Tick) Tick = 0;
}
void StateDefiner() {
  if (digitalRead(SettingsButton) == HIGH && SettingsState != HIGH)
  {
    Display.clearDisplay();
    SettingsState = HIGH;
    switch (CurrentState)
    {
    case MAIN:
      CurrentState = NETWORK;
      break;
    case NETWORK:
      CurrentState = NETWORK_SETTINGS;
      break;
    case NETWORK_SETTINGS:
      CurrentState = MAIN;
      break;
    default:
      break;
    }
  }
  if (digitalRead(SettingsButton) == LOW && SettingsState != LOW)
  {
    SettingsState = LOW;
  }
}

void ShowNetworkInfo() {
  Display.firstPage();
  String NetworkIP = "IP: " + (String)WiFi.localIP().toString().c_str() + "/" + (String)WiFi.subnetCIDR();
  String NetworkSSID = "SSID: " + (String)WiFi.SSID().c_str();
  String NetworkChannelEncrypt = "Csatorna: " + (String)WiFi.channel();
  String NetworkRSSI = "Jelszint: " + (String)WiFi.RSSI() + " dBm";
  do
  {
    Display.setFont(u8g2_font_timR08_tf);
    String NetworkStatus = "Hálózat: ";
    if (WiFi.status() == WL_CONNECTED)
    {
      NetworkStatus += "Csatlakozva!";
    }
    else if (WiFi.status() == WL_DISCONNECTED){
      NetworkStatus += "Lecsatlakozva!";
    } 
    else if (WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_NO_SSID_AVAIL) {
      NetworkStatus += "Nem található!";
    }
    else if (WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_NO_SSID_AVAIL) {
      NetworkStatus += "Sikertelen!";
    }
    else if (WiFi.status() == WL_IDLE_STATUS) {
      NetworkStatus += "Tétlen!";
    }
    else {
      NetworkStatus += "Hiba! (" + (String)WiFi.status() + ")";
    }

    Display.drawUTF8(0, 10, NetworkStatus.c_str());
    if (WiFi.status() == WL_CONNECTED) {
      Display.drawUTF8(0, 20, NetworkSSID.c_str());
      Display.drawUTF8(0, 30, NetworkIP.c_str());
      Display.drawUTF8(0, 40, NetworkRSSI.c_str());
      Display.drawUTF8(0, 50, NetworkChannelEncrypt.c_str());
    } else {

    }
    Display.drawHLine(0,64-11,127);
    Display.drawUTF8(30, 63, "WiFi információk");

  } while (Display.nextPage());
}

void ShowNetworkConfig() {
  Display.firstPage();
  do {
    Display.setFont(u8g2_font_timR08_tf);
    if (WiFi.status() == WL_CONNECTED) Display.drawUTF8(0, 10, "Lecsatlakozás: [OK] 1 mp.");
    else Display.drawUTF8(0, 10, "Újracsatlakozás: [OK] 1 mp.");
    Display.drawHLine(0,64-11,127);
    Display.drawUTF8(30, 63, "WiFi beállítások");
  } while (Display.nextPage());
}

void ManageWiFiConnection() {
  if (digitalRead(SetButton) == HIGH && SetState != HIGH)
  {
    SetState = HIGH;
    if (WiFi.status() == WL_CONNECTED) WiFi.disconnect();
    else WiFi.begin(NetworkSSID, NetworkPassword);
  }
  if (digitalRead(SetButton) == LOW && SetState != LOW)
  {
    SetState = LOW;
  }
}