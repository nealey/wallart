#pragma once

// Short this to ground to reset the network
#define RESET_PIN 26

void network_reset();
void network_setup(char *password);
bool connected();
void pause(uint32_t dwMs);
void netget(int count);
