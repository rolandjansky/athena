/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPHAESTUS_HASHTABLE_H
#define HEPHAESTUS_HASHTABLE_H

/*
  Trivial memory-address based hash table
*/

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

struct hhh_Cell {
   void *key;
   void *value;

   struct hhh_Cell *next;
};

struct hhh_HashTable {
   pthread_mutex_t lock;

   struct hhh_Cell **table;

   unsigned int size;
   unsigned int nentries;

   struct hhh_Cell *lastfound;
};


/* create a new hash table of at least size minsize */
struct hhh_HashTable *hhh_HashTable_new( unsigned long minsize );

/* insert pair (key, value); return 0 on failure */
int hhh_HashTable_insert( struct hhh_HashTable *ht, void *key, void *value );

/* remove pair (key, value) from table; return 0 on failure */
int hhh_HashTable_remove(
   struct hhh_HashTable *ht, void *key, void (*cleanup)( void *ptr ) );

/* find value for pair; return NULL if not found */
void *hhh_HashTable_find( struct hhh_HashTable *ht, void *key );

/* merge one hash table into another */
int hhh_HashTable_merge( struct hhh_HashTable *into, struct hhh_HashTable *from );

/* delete table and (optional) contents */
void hhh_HashTable_delete( struct hhh_HashTable *ht, void (*cleanup)( void *ptr ) );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !HEPHAESTUS_HASHTABLE_H */
