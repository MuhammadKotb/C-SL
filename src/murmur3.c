#include <murmur3.h>
#include <stddef.h>
#include <stdio.h>


void murmurHash_32(const void *key, const size_t len, const u_int32_t seed, void *out) {
  
  const u_int32_t c1 = 0xcc9e2d51;
  const u_int32_t c2 = 0x1b873593;
  const u_int32_t r1 = 15;
  const u_int32_t r2 = 13;
  const u_int32_t m = 5;
  const u_int32_t n = 0xe6546b64;

  u_int32_t hash = seed;

  const int nblocks = len / 4;

  const u_int32_t* blocks_arr = (const u_int32_t*)((const u_int8_t*)key + nblocks * 4);

  for(size_t i = -nblocks; i; i++) {
    
    u_int32_t fourByteBlock = blocks_arr[i];
    fourByteBlock = fourByteBlock * c1;
    fourByteBlock = (fourByteBlock << r1) | (fourByteBlock >> (32 - r1));
    fourByteBlock = fourByteBlock * c2;

    hash = hash ^ fourByteBlock;
    hash = (hash << r2) | (hash >> (32 - r2));

    hash = hash * m;
    hash = hash + n;

  }
  const u_int8_t* tail = (const u_int8_t *)((const u_int8_t*)key + nblocks * 4);
  
  u_int32_t fourByteBlock = 0;

  switch(len & 3) {
    case 3: fourByteBlock ^= tail[2] << 16;
    case 2: fourByteBlock ^= tail[1] << 8;
    case 1: fourByteBlock ^= tail[0];
            fourByteBlock *= c1; 
            fourByteBlock = (fourByteBlock << r1) | (fourByteBlock >> (32 - r1)); 
            fourByteBlock *= c2; 
            hash ^= fourByteBlock;
  };
  hash = hash ^ len;
  hash = hash ^ (hash >> 16);
  hash = hash * 0x85ebca6b;
  hash = hash ^ (hash >> 13);
  hash = hash * 0xc2b2ae35;
  hash = hash ^ (hash >> 16);

  *(u_int32_t*)(out) = hash;
}

void murmurHash_128(const void *key, const size_t len, const u_int32_t seed, void *out) {

}