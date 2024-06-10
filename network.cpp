#include <FastLED.h>
#include <WiFiManager.h>
#include <WiFiManagerTz.h>
#include <esp_wifi.h>
#include <esp_sntp.h>
#include <HTTPClient.h>	
#include <WiFiClientSecure.h>
#include "network.h"

WiFiManager wfm;

void network_reset() {
  Serial.println("Resetting network");
  wfm.resetSettings();
}


bool time_was_accurate_once = false;
bool clock_is_set() {
  return time_was_accurate_once;
}

void on_time_available(struct timeval *t) {
  struct tm timeInfo;
  getLocalTime(&timeInfo, 1000);
  Serial.println(&timeInfo, "%A, %B %d %Y %H:%M:%S %Z %z ");
  time_was_accurate_once = true;
}

void network_setup(char *password) {
  String hostname = "WallArt";

  WiFiManagerNS::NTP::onTimeAvailable(&on_time_available);
  WiFiManagerNS::init(&wfm, nullptr);

  std::vector<const char *> menu = {"wifi", "info", "custom", "param", "sep", "update", "restart", "exit"};
  wfm.setMenu(menu);
	wfm.setConfigPortalBlocking(false);
	wfm.setHostname(hostname);
	wfm.autoConnect(hostname.c_str(), password);
}

bool connected() {
	return WiFi.status() == WL_CONNECTED;
}

bool timeConfigured = false;

void pause(uint32_t dwMs) {
  if (connected() && !timeConfigured) {
    WiFiManagerNS::configTime();
    timeConfigured = true;
  }
  if (!digitalRead(RESET_PIN)) {
    network_reset();
  }

	for (uint32_t t = 0; t < dwMs; t += 10) {
		wfm.process();
		digitalWrite(LED_BUILTIN, !connected());
		delay(10);
	}
}
