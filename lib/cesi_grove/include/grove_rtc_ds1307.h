#ifndef GROVE_RTC_DS1307_H
#define GROVE_RTC_DS1307_H

#include <stdbool.h>
#include "cesi_types.h"

bool GroveRTC_Init(void);
bool GroveRTC_GetDateTime(RTC_DateTime *dt);
bool GroveRTC_SetDateTime(const RTC_DateTime *dt);

bool GroveRTC_WriteRAM(uint8_t offset, const uint8_t *data, uint8_t length);
bool GroveRTC_ReadRAM(uint8_t offset, uint8_t *data, uint8_t length);

#endif
