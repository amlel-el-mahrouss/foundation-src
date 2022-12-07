#pragma once

#include "Foundation.h"

extern unsigned crc32_tab[256];

int
crc32(const char* byte, int len);