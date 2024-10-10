#include <stdio.h>
#include <murmur3.h>
#include <inttypes.h>
#include <string.h>


int main(int argc, char* argv[]) {

  const char* key = "Hello, world!";
  u_int32_t out = 0;
  murmurHash_32(key, strlen(key), 0, &out);
  printf("%u\n", out);
  
}