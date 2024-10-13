#include "murmur3.h"
#include <hashtable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


void _insert_int(hashtable *hashtable, int new_key, int new_value) {


  if (hashtable->keys == NULL) {
    hashtable->keys = calloc(hashtable->cap, sizeof(int));
    hashtable->buckets = calloc(hashtable->cap, sizeof(bucket_int *));
    for (size_t i = 0; i < hashtable->cap; i++) {
      hashtable->buckets[i] = calloc(1, sizeof(bucket_int));
      ((bucket_int*)hashtable->buckets[i])[0].hash = 0;
    }
  } else {
    if (hashtable->size + 1 > hashtable->cap) {
      hashtable->cap += hashtable->initial_cap;
      _rehash_int(hashtable);
    }
  }
  ((int*)hashtable->keys)[hashtable->size] = new_key;
  hashtable->size++;
  u_int32_t hash = 0;
  murmurHash_32(&new_key, sizeof(int), 0xbc9f1d34, &hash);
  size_t index = hash % hashtable->cap;
  bucket_int bucket;
  bucket.hash = hash;
  bucket.value = new_value;
  _insert_value_int(hashtable, bucket, index);
  printf("insert\n");
}

void _insert_float(hashtable *hashtable, float new_key, float new_value) {
  hashtable->size++;
}

void _insert_double(hashtable *hashtable, double new_key, double new_value) {
  hashtable->size++;
}

void _insert_value_int(hashtable *hashtable, bucket_int bucket, size_t index) {

  size_t bucketIndex = 0;
  bucket_int **buckets = (bucket_int **)hashtable->buckets;
  if (buckets[index][bucketIndex].hash == 0) {
    buckets[index][bucketIndex].hash = bucket.hash;
    buckets[index][bucketIndex].value = bucket.value;
  } else {
    while (buckets[index][bucketIndex].hash != 0) {
      if(buckets[index][bucketIndex].hash == bucket.hash) {
        printf("dup\n");
        return;
      }
      bucketIndex++;
    }
    buckets[index] = (bucket_int *)realloc(
        buckets[index], (bucketIndex + 2) * sizeof(bucket_int));
    buckets[index][bucketIndex].hash = bucket.hash;
    buckets[index][bucketIndex].value = bucket.value;
    buckets[index][bucketIndex + 1].hash = 0;
  }
}

bucket_int _get_bucket_int(hashtable *hashtable, int key, size_t index) {
  bucket_int res_bucket;
  res_bucket.hash = 0;
  res_bucket.value = 0;
  u_int32_t hash = 0;
  murmurHash_32(&key, sizeof(int), 0xbc9f1d34, &hash);
  bucket_int **buckets = (bucket_int **)hashtable->buckets;
  bucket_int *bucket = buckets[index];
  size_t bucketIndex = 0;
  while (bucket[bucketIndex].hash != 0) {
    if (hash == bucket[bucketIndex].hash) {
      res_bucket.hash = bucket[bucketIndex].hash;
      res_bucket.value = bucket[bucketIndex].value;

    }
    bucketIndex++;
  }
  return res_bucket;
}

void _get_int(hashtable *hashtable, int key, int *value) {
  u_int32_t hash = 0;
  murmurHash_32(&key, sizeof(int), 0xbc9f1d34, &hash);
  size_t index = hash % hashtable->cap;
  bucket_int bucket = _get_bucket_int(hashtable, key, index);
  if(bucket.hash != 0) {
    *value = bucket.value;
  }
}

void _rehash_int(hashtable *hashtable) {

  hashtable->keys = realloc(hashtable->keys, hashtable->cap * sizeof(int));
  hashtable->buckets = realloc(hashtable->buckets, hashtable->cap * sizeof(bucket_int *));
  for (size_t i = hashtable->size; i < hashtable->cap; i++) {
    hashtable->buckets[i] = calloc(1, sizeof(bucket_int));
    ((bucket_int*)hashtable->buckets[i])[0].hash = 0;
  }
  for (size_t i = 0; i < hashtable->size; i++) {
    u_int32_t hash = 0;
    murmurHash_32(&((int*)hashtable->keys)[i], sizeof(int), 0xbc9f1d34, &hash);
    size_t index = hash % hashtable->cap;
    size_t old_index = hash % (hashtable->cap - hashtable->initial_cap);
    bucket_int bucket = _get_bucket_int(hashtable, ((int*)hashtable->keys)[i], old_index); 
    _insert_value_int(hashtable, bucket, index);    
  }
}

hashtable create_hashtable(Type key_type, Type value_type) {
  hashtable map;
  switch (key_type) {
  case INT: {
    map._insert_int = _insert_int;
    map._get_int = _get_int;
    break;
  }
  case FLOAT: {
    map._insert_float = _insert_float;
    break;
  }
  case DOUBLE: {
    map._insert_double = _insert_double;
    break;
  }
  default:
    break;
  }
  map.size = 0;
  map.initial_cap = 5;
  map.cap = map.initial_cap;
  map.keys = NULL;
  map.buckets = NULL;
  return map;
}
