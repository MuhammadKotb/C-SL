#include "murmur3.h"
#include <hashtable.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


void _insert_int(hashtable *hashtable, int new_key, int new_value) {
  if (hashtable->keys == NULL) {
    hashtable->keys = calloc(hashtable->cap, sizeof(int));
    hashtable->buckets = calloc(hashtable->cap, sizeof(bucket_int *));
   
  } else {
    if (hashtable->size + 1 > hashtable->cap) {
      hashtable->cap += hashtable->initial_cap;
      _rehash_int(hashtable);
    }
  }
  ((int*)hashtable->keys)[hashtable->size] = new_key;
  hashtable->size++;
  u_int32_t hash = 0;
  murmurHash_32(&new_key, sizeof(int), HASH_SEED, &hash);
  size_t index = hash % hashtable->cap;
  bucket_int bucket;
  bucket.hash = hash;
  bucket.value = new_value;
  size_t bucket_len = _get_bucket_length(hashtable, index);
  bucket_int* last_bucket = NULL;
  if(bucket_len == 0) {
    hashtable->buckets[index] = calloc(1, sizeof(bucket_int));
  }
  else {
    hashtable->buckets[index] = realloc(hashtable->buckets[index], (bucket_len + 1) * sizeof(bucket_int));
    last_bucket = &((bucket_int*)hashtable->buckets[index])[bucket_len - 1];
  }
  ((bucket_int*)hashtable->buckets[index])[bucket_len].hash = hash;
  ((bucket_int*)hashtable->buckets[index])[bucket_len].value = new_value;
  ((bucket_int*)hashtable->buckets[index])[bucket_len].next = NULL;
  ((bucket_int*)hashtable->buckets[index])[bucket_len].prev = last_bucket;
  _insert_bucket_int(hashtable, index, &((bucket_int*)hashtable->buckets[index])[bucket_len]);
}

void _delete_int(hashtable* hashtable, int key) {
  u_int32_t hash = 0;
  murmurHash_32(&key, sizeof(int), HASH_SEED, &hash);
  size_t index = hash % hashtable->cap;
  _delete_bucket_int(hashtable, hash, index);
}


void _insert_bucket_int(hashtable* hashtable, size_t index, bucket_int* bucket) {

  if(bucket->prev != NULL) {
    bucket->prev->next = bucket;
  }
  else {
    hashtable->buckets[index] = bucket; 
  }
}

void _move_bucket_int(hashtable* hashtable, size_t old_index, size_t index, bucket_int* bucket) {
  bucket_int* next = bucket->next;
  bucket_int* prev = bucket->prev;
  bucket_int* last = hashtable->buckets[index];
  if(prev != NULL) prev->next = next;
  else {
    hashtable->buckets[old_index] = next;
    
  }
  if(next != NULL) next->prev = prev;
  bucket->next = NULL;


  if(last != NULL) {
    while(last->next != NULL) {
      last = last->next;
    }
  last->next = bucket;
  }
  else {
    // _insert_bucket_int(hashtable, index, bucket);
    hashtable->buckets[index] = bucket;
    return;
  }
  bucket->prev = last;
  
  
}


void _delete_bucket_int(hashtable* hashtable, u_int32_t hash, size_t index) {
  bucket_int **buckets = (bucket_int **)hashtable->buckets;
  bucket_int *bucket = buckets[index];
  size_t bucket_index = 0;
  bucket_int* current_bucket = buckets[index]; 
  while(current_bucket != NULL) {
    if (hash == current_bucket->hash) {
      bucket_int* prev_bucket = current_bucket->prev;
      bucket_int* next_bucket = current_bucket->next;
      if(prev_bucket == NULL) {
        buckets[index] = current_bucket->next;
      }
      else {
        prev_bucket->next = current_bucket->next;
        if(next_bucket != NULL) {
          next_bucket->prev = prev_bucket;
        }
      }
      free(current_bucket);
      break;
    }
    current_bucket = current_bucket->next;
    bucket_index++;
  }
}

bucket_int* _get_bucket_int(hashtable *hashtable, int key, size_t index) {
  bucket_int* res_bucket = NULL;
  u_int32_t hash = 0;
  murmurHash_32(&key, sizeof(int), HASH_SEED, &hash);
  bucket_int **buckets = (bucket_int **)hashtable->buckets;
  bucket_int *bucket = buckets[index];
  bucket_int* current_bucket = buckets[index];
  while (current_bucket != NULL) {
    if (hash == current_bucket->hash) {
      res_bucket = current_bucket;
      break;
    }
    current_bucket = current_bucket->next;
  }
  return res_bucket;
}


void _get_int(hashtable *hashtable, int key, int *value) {
  u_int32_t hash = 0;
  murmurHash_32(&key, sizeof(int), HASH_SEED, &hash);
  size_t index = hash % hashtable->cap;
  bucket_int* bucket = _get_bucket_int(hashtable, key, index);
  if(bucket == NULL) return;
  if(bucket->hash != 0) {
    *value = bucket->value;
  }
}

void _rehash_int(hashtable *hashtable) {
  hashtable->keys = realloc(hashtable->keys, hashtable->cap * sizeof(int));
  hashtable->buckets = realloc(hashtable->buckets, hashtable->cap * sizeof(bucket_int *));
  for (size_t i = 0; i < hashtable->size; i++) {
    u_int32_t hash = 0;
    murmurHash_32(&((int*)hashtable->keys)[i], sizeof(int), HASH_SEED, &hash);
    size_t index = hash % hashtable->cap;
    size_t old_index = hash % (hashtable->cap - hashtable->initial_cap);
    bucket_int* bucket = _get_bucket_int(hashtable, ((int*)hashtable->keys)[i], old_index);
    if(index != old_index) _move_bucket_int(hashtable, old_index, index, bucket);
  }
}

size_t _get_bucket_length(hashtable *hashtable, size_t index) {
  bucket_int **buckets = (bucket_int **)hashtable->buckets;
  size_t len = 0;
  bucket_int* current_bucket = buckets[index]; 
  while (current_bucket != NULL) {
    current_bucket = current_bucket->next;
    len++;
  }
  return len;
}

void _insert_float(hashtable *hashtable, float new_key, float new_value) {
  hashtable->size++;
}

void _insert_double(hashtable *hashtable, double new_key, double new_value) {
  hashtable->size++;
}

void _destroy(hashtable* hashtable) {
  free(hashtable->keys);
  for(size_t i = 0; i < hashtable->cap; i++) {
    if(hashtable->buckets[i]) {
      free(hashtable->buckets[i]);
    }
  }
  free(hashtable->buckets);
}

hashtable create_hashtable(Type key_type, Type value_type) {
  hashtable map;
  switch (key_type) {
  case INT: {
    map._insert_int = _insert_int;
    map._get_int = _get_int;
    map._delete_int = _delete_int;
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
  map._destroy = _destroy;
  map.size = 0;
  map.initial_cap = 5;
  map.cap = map.initial_cap;
  map.keys = NULL;
  map.buckets = NULL;
  return map;
}
