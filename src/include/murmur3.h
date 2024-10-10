#ifndef MURMUR3
#define MURMUR3

#include <sys/types.h>

void murmurHash_32(const void* key, const size_t len, u_int32_t seed, void* out);
void murmurHash_128(const void* key, const size_t len, u_int32_t seed, void* out);

#endif

