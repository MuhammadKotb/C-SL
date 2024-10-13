#ifndef HASHTABLE
#define HASHTABLE
#include <stddef.h>
#include <sys/types.h>
#include <types.h>


#define EMPTY_BUCKET 

struct _hashtable_int;
struct _hashtable_float;
struct _hashtable_double;

typedef struct bucket_int bucket_int;
typedef struct hashtable hashtable;

struct _hashtable_int {
  int *keys;
};

struct _hashtable_double {
  double* keys;
};

struct _hashtable_float {
  float* keys;
};

typedef struct hashtable {
  void* keys;
  void** buckets;
  union {
    void (*_insert_int)(hashtable*, int, int);
    void (*_insert_float)(hashtable*, float, float);
    void (*_insert_double)(hashtable*, double, double);
  };
  union {
    void (*_get_int)(hashtable*, int, int*);
  };
  size_t size;
  size_t cap;
  size_t initial_cap;
} hashtable;

typedef struct bucket_int {
  int value;
  u_int32_t hash;
} bucket_int;


void _insert_int(hashtable* hashtable, int new_key, int new_value);
void _insert_value_int(hashtable *hashtable, bucket_int bucket, size_t index);
void _get_int(hashtable* hashtable, int key, int* value);
bucket_int _get_bucket_int(hashtable* hashtable, int key, size_t index);

void _rehash_int(hashtable* hashtable);

void _insert_float(hashtable* hashtable, float new_key, float new_value);
void _insert_value_float(float** values, float new_value, size_t index);

void _insert_double(hashtable* hashtable, double new_key, double new_value);
void _insert_value_double(double** values, double new_value, size_t index);


// struct _hashtable_int _init_hashtable_int() {
//   struct _hashtable_int hashtable_int;
//   return hashtable_int;
// }

// struct _hashtable_float _init_hashtable_float() {
//   struct _hashtable_float hashtable_float;
//   return hashtable_float;
// }

// struct _hashtable_double _init_hashtable_double() {
//   struct _hashtable_double hashtable_double;
//   return hashtable_double;
// }


hashtable create_hashtable(Type key_type, Type value_type);

#endif