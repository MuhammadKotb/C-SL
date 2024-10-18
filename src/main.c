#include <hashtable.h>
#include <inttypes.h>
#include <murmur3.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

uint32_t rotl32(uint32_t x, int8_t r) { return (x << r) | (x >> (32 - r)); }

#define ROTL32(x, y) rotl32(x, y)
#define ROTL64(x, y) rotl64(x, y)

uint32_t getblock32(const uint32_t *p, int i) { return p[i]; }

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

uint32_t fmix32(uint32_t h) {
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32(const void *key, int len, uint32_t seed, void *out) {
  const uint8_t *data = (const uint8_t *)key;
  const int nblocks = len / 4;

  uint32_t h1 = seed;

  const uint32_t c1 = 0xcc9e2d51;
  const uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);

  for (int i = -nblocks; i; i++) {
    uint32_t k1 = getblock32(blocks, i);

    k1 *= c1;
    k1 = ROTL32(k1, 15);
    k1 *= c2;

    h1 ^= k1;
    h1 = ROTL32(h1, 13);
    h1 = h1 * 5 + 0xe6546b64;
  }

  //----------
  // tail

  const uint8_t *tail = (const uint8_t *)(data + nblocks * 4);

  uint32_t k1 = 0;

  switch (len & 3) {
  case 3:
    k1 ^= tail[2] << 16;
  case 2:
    k1 ^= tail[1] << 8;
  case 1:
    k1 ^= tail[0];
    k1 *= c1;
    k1 = ROTL32(k1, 15);
    k1 *= c2;
    h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix32(h1);

  *(uint32_t *)out = h1;
}

int main(int argc, char *argv[]) {

  // const char *key = "5";
  // u_int32_t out = 0;
  // MurmurHash3_x86_32(key, sizeof(int), 0xbc9f1d34, &out);
  // printf("%u\n", out);
  // const char *key1 = "3";
  // u_int32_t out1 = 0;
  // MurmurHash3_x86_32(key1, sizeof(int), 0xbc9f1d34, &out1);
  // printf("%u\n", out1);
  // const char *key2 = "355";
  // u_int32_t out2 = 0;
  // MurmurHash3_x86_32(key2, sizeof(int), 0xbc9f1d34, &out2);
  // printf("%u\n", out2);

  hashtable map = create_hashtable(INT, INT);
  map._insert_int(&map, 77, 10);
  map._insert_int(&map, 3, 44);
  map._insert_int(&map, 3225, 5);
  map._insert_int(&map, 555557, 322);
  map._insert_int(&map, 58802, 233);
  map._insert_int(&map, 16, 2);

  map._delete_int(&map, 3);

  printf("inserted\n");

  int* keys = (int*)map.keys;
  for(size_t i = 0; i < map.size; i++) {
    int value = 0;
    map._get_int(&map, keys[i], &value);
    printf("%d %d\n", keys[i], value);
  }

  map._destroy(&map);
}