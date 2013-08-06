/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#if defined(linux)
#define _GNU_SOURCE 1
#else
#define __builtin_expect(x, y) (x)
#endif

#include <stdio.h>
#include <dlfcn.h>


/*- data ------------------------------------------------------------------- */
typedef void * (*Malloc_t) ( size_t size );
typedef void * (*Realloc_t) ( void *ptr, size_t size );
typedef void   (*Free_t) ( void *ptr );

static Malloc_t  gMalloc  = 0;
static Realloc_t gRealloc = 0;
static Free_t    gFree    = 0;

/* hook variables */
void (*hhh_malloc_hook) ( size_t size, const void *result ) = NULL;
#pragma weak hhh_malloc_hook
void (*hhh_realloc_hook) ( void *ptr, size_t size, const void *result ) = NULL;
#pragma weak hhh_realloc_hook
void  (*hhh_free_hook) ( void *ptr ) = NULL;
#pragma weak hhh_free_hook

/* to let the memory trackers know we're here */
int hhh_preloaded = 1;


/* _________________________________________________________________________ */
void* malloc( size_t size ) {
   void *result = 0;

   result = gMalloc( size );

   if ( __builtin_expect( (long)hhh_malloc_hook, 0 ) )
      hhh_malloc_hook( size, result );

   return result;
}

/* _________________________________________________________________________ */
void* realloc( void *ptr, size_t size ) {
   void *result = 0;

   result = gRealloc( ptr, size );

   if ( __builtin_expect( (long)hhh_realloc_hook, 0 ) )
      hhh_realloc_hook( ptr, size, result );

   return result;
}

/* _________________________________________________________________________ */
void free( void *ptr ) {
   if ( __builtin_expect( (long)hhh_free_hook, 0 ) )
      hhh_free_hook( ptr );

   return gFree( ptr );
}


/*========================================================================== */
/* startup and shutdown */
/*========================================================================== */
static void setup() __attribute__ ((constructor));
static void teardown() __attribute__ ((destructor));

/* _________________________________________________________________________ */
static void setup() {
   static int initialized = 0;
   int i;
   const char *libs[] = { "libc.so", "libc.so.6" };
   const int nlibs = sizeof(libs)/sizeof(libs[0]);

   if ( ! initialized ) {
      initialized = 1;

      gMalloc  = (Malloc_t)dlsym( RTLD_NEXT, "malloc" );
      gRealloc = (Realloc_t)dlsym( RTLD_NEXT, "realloc" );
      gFree    = (Free_t)dlsym( RTLD_NEXT, "free" );

      if ( ! ( gMalloc && gRealloc && gFree ) )
         fprintf( stderr, "Hephaestus warning: unable to locate glibc memory functions; hooks will not work!\n" );
   }
}

/* _________________________________________________________________________ */
static void teardown() {
   static int finalized = 0;

   if ( ! finalized ) {
      finalized = 1;

      gMalloc  = 0;
      gRealloc = 0;
      gFree    = 0;
   }
}
