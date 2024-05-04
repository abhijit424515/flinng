#ifndef _MMH_H_
#define _MMH_H_

#include <stdint.h>

void MurmurHash3_x86_32(const void *key, int len, uint32_t seed, void *out);

#endif
