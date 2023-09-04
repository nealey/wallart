#pragma once

void network_reset();
void network_setup(char *password);
bool connected();
void pause(uint32_t dwMs);
void netget(int count);
