#pragma once

#include <HTTPClient.h>
#include <WiFiClientSecure.h>


void network_setup(char *password);
bool connected();
void pause(uint32_t dwMs);
void netget(int count);