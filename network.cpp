#include <FastLED.h>
#include <WiFiManager.h>
#include <esp_wifi.h>
#include <HTTPClient.h>	
#include <WiFiClientSecure.h>
#include "network.h"

WiFiManager wfm;

void network_setup(char *password) {
	String hostid = String(ESP.getEfuseMac(), HEX);
	String hostname = "Wall Art " + hostid;

	wfm.setConfigPortalBlocking(false);
	wfm.setHostname(hostname);
	wfm.autoConnect(hostname.c_str(), password);

	pinMode(LED_BUILTIN, OUTPUT);
}

bool connected() {
	return WiFi.status() == WL_CONNECTED;
}

void pause(uint32_t dwMs) {
	for (uint32_t t = 0; t < dwMs; t += 10) {
		wfm.process();
		digitalWrite(LED_BUILTIN, !connected());
		delay(10);
	}
}
