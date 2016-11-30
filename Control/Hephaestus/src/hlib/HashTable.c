/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/HashTable.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


#define ADDR_HASH( key ) \
   (((unsigned long)key >> 2) * 2654435761ul)     /* ignore 2bits b/c alignment */


/* primes taken from g++ source, assumes long is at least 32 bits */
static const unsigned long primes[] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
  1610612741ul, 3221225473ul, 4294967291ul
};

static const unsigned int nprimes = sizeof(primes)/sizeof(primes[0]);
static const double maxload = 0.7;


/*- use an arena for alloction of cells ------------------------------------ */
static struct hhh_Cell *gPool = 0;
static pthread_mutex_t gPoolLock = PTHREAD_MUTEX_INITIALIZER;

static struct hhh_Cell *hhh_Cell_new() {
/* allocate objects in a single memory block, admin w/ linked list */
   int i;
   struct hhh_Cell *fresh;

   pthread_mutex_lock( &gPoolLock );

   if ( gPool == NULL ) {
      const long nobj = sysconf( _SC_PAGESIZE ) / sizeof( struct hhh_Cell );
      gPool = (struct hhh_Cell*)malloc( nobj * sizeof( struct hhh_Cell ) );

      if ( gPool == NULL ) {
         fprintf( stderr, "Hephaestus ERROR: memory allocation failed in Cell_new\n" );
         pthread_mutex_unlock( &gPoolLock );
         return NULL;
      }

      for ( i = 0; i < nobj-1; i++ ) {
	 gPool[ i ].next = &gPool[ i+1 ];
      }
      gPool[ nobj-1 ].next = NULL;
   }

   fresh = gPool;
   gPool = fresh->next;

   pthread_mutex_unlock( &gPoolLock );

   return fresh;
}

static void hhh_Cell_delete( struct hhh_Cell *cell ) {
/* recycle the object memory, by putting it back into the list */
   if ( cell == NULL )
      return;

   pthread_mutex_lock( &gPoolLock );

   cell->next = gPool;
   gPool = cell;

   pthread_mutex_unlock( &gPoolLock );
}


/* create a new hash table of at least size minsize */
struct hhh_HashTable *hhh_HashTable_new( unsigned long minsize )
{
   struct hhh_HashTable *ht;
   unsigned long i;

/* allocate new hash table */
   ht = (struct hhh_HashTable*)malloc( sizeof(struct hhh_HashTable) );
   if ( ! ht )
      return NULL;

   pthread_mutex_init( &ht->lock, NULL );

/* select proper prime for size, based on minimum requested size */
   for ( i = 0; i < nprimes-1; i++ ) {
      if ( minsize < primes[i] )
         break;
   }

   ht->table     = (struct hhh_Cell**)calloc( primes[i], sizeof(struct hhh_Cell*) );
   ht->size      = primes[i];
   ht->nentries  = 0;
   ht->lastfound = NULL;

   return ht;
}


/* expand the hash table; return 0 on failure */
static int hhh_HashTable_expand( struct hhh_HashTable *ht )
{
   unsigned long i, index;
   unsigned long size = 0;
   struct hhh_Cell **newtable;
   struct hhh_Cell *cell;

/* use the next prime, which is roughly double the size */
   for ( i = 0; i < nprimes; i++ ) {
      if ( ht->size < primes[i] ) {
         size = primes[i];
         break;
      }
   }

   if (size == 0)
     return 0;

   newtable = (struct hhh_Cell**)calloc( size, sizeof(struct hhh_Cell*) );
   if ( ! newtable )
      return 0;

   pthread_mutex_lock( &ht->lock );

/* copy elements into the new table (size has changed, and so does the index */
   for ( i = 0; i < ht->size; i++ ) {
      while ( (cell = ht->table[i]) != NULL ) {
         ht->table[i] = cell->next;
         index = ADDR_HASH( cell->key ) % size;
         cell->next = newtable[index];
         newtable[index] = cell;
      }
   }

/* done with old table, keep new; ht->lastfound, ht->nentries still same */
   free( ht->table );
   ht->table = newtable;
   ht->size  = size;

   pthread_mutex_unlock( &ht->lock );

   return 1;
}


/* insert pair (key, value); return 0 on failure */
int hhh_HashTable_insert( struct hhh_HashTable *ht, void *key, void *value )
{
   unsigned long index;
   struct hhh_Cell *cell;

   if ( maxload < (ht->nentries+1)/(double)ht->size )
      hhh_HashTable_expand( ht );  /* may fail */

   cell = hhh_Cell_new();
   if ( ! cell )
      return 0;

   cell->key = key;
   cell->value = value;

   pthread_mutex_lock( &ht->lock );

   index = ADDR_HASH( key ) % ht->size;
   cell->next = ht->table[index];
   ht->table[index] = cell;
   ht->nentries += 1;

   pthread_mutex_unlock( &ht->lock );

   return 1;
}


/* remove pair (key, value) from table; return 0 on failure */
int hhh_HashTable_remove(
   struct hhh_HashTable *ht, void *key, void (*cleanup)( void *ptr ) )
{
   struct hhh_Cell **pcell, *cell;

   pthread_mutex_lock( &ht->lock );

   pcell = &(ht->table[ ADDR_HASH( key ) % ht->size ]);
   cell  = *pcell;
   while ( cell != NULL ) {
      if ( key == cell->key ) {
         *pcell = cell->next;
         ht->nentries -= 1;
         pthread_mutex_unlock( &ht->lock );

         if ( cleanup )
            cleanup( cell->value );
         hhh_Cell_delete( cell );

         return 1;
      }
      pcell = &(cell->next);
      cell  = cell->next;
   }

   pthread_mutex_unlock( &ht->lock );

   return 0;
}


/* find value for pair; return NULL if not found */
void *hhh_HashTable_find( struct hhh_HashTable *ht, void *key )
{
/* this is rather common: e.g. memory allocation in a loop */
   if ( ht->lastfound && ht->lastfound->key == key )
      return ht->lastfound->value;

/* ordinary lookup */
   for ( ht->lastfound = ht->table[ ADDR_HASH( key ) % ht->size ];
         ht->lastfound && ht->lastfound->key != key;
         ht->lastfound = ht->lastfound->next )
   {}

/* success */
   if ( ht->lastfound )
      return ht->lastfound->value;

/* failure */
   return NULL;
}


/* merge one hash table into another */
int hhh_HashTable_merge( struct hhh_HashTable* into, struct hhh_HashTable* from )
{
   unsigned long i;
   struct hhh_Cell* cell;

   pthread_mutex_lock( &from->lock );

   for ( i = 0; i < from->size; i++ ) {
      cell = from->table[i];
      while ( cell != NULL ) {
         hhh_HashTable_insert( into, cell->key, cell->value );
         cell = cell->next;
      }
   }

   pthread_mutex_unlock( &from->lock );

   return 1;
}


/* delete all contents (all values are assumed malloc-ed) */
void hhh_HashTable_delete( struct hhh_HashTable* ht, void (*cleanup)( void *ptr ) )
{
   unsigned long i;
   struct hhh_Cell *cell, *tmp;

   if ( ! ht )
      return;

   for ( i = 0; i < ht->size; i++ ) {
      cell = ht->table[i];
      while ( cell != NULL ) {
         tmp = cell;
         cell = cell->next;
         if ( cleanup )
            cleanup( tmp->value );
         hhh_Cell_delete( tmp );
      }
   }

   pthread_mutex_destroy( &ht->lock );

   free( ht->table );
   free( ht );
}
