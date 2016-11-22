/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/Utils.h"
#include "Hephaestus/MemoryTrace.h"
#include "Hephaestus/HashTable.h"
#if defined ( __i386 )
#include "Hephaestus/GenericBacktrace.h"
#endif

#include <execinfo.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <assert.h>


#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif


/*- data ------------------------------------------------------------------- */

/* forward declarations */
static void *hhh_MallocHook( size_t size, const void *caller );
static void *hhh_ReallocHook( void *ptr, size_t size, const void *caller );
static void hhh_FreeHook( void *ptr, const void *caller );

/* holders for original hooks */
static void *(*s_OldMallocHook)  ( size_t size, const void *caller );
static void *(*s_OldReallocHook) ( void *ptr, size_t size, const void *caller );
static void (*s_OldFreeHook)     ( void *ptr, const void *caller );

/* available flags */
#define IS_RUNNING          1
#define DOUBLE_DEL_CHECK    2

/* data holders and their management */
static struct hhh_HashTable *gTraceInfo  = 0;

/* module-level configuration flags */
static long gFlags = DOUBLE_DEL_CHECK;


/* _________________________________________________________________________ */
static void hhh_Hooks_save() {
/* store old hooks in buffer for reset later; prevent storing ourselves in
   case of a double-call by the user (e.g. from python) */
   if ( __malloc_hook != hhh_MallocHook )
      s_OldMallocHook  = __malloc_hook;
   if ( __realloc_hook != hhh_ReallocHook )
      s_OldReallocHook = __realloc_hook;
   if ( __free_hook != hhh_FreeHook )
      s_OldFreeHook    = __free_hook;
}

/* _________________________________________________________________________ */
static void hhh_Hooks_install() {
   __free_hook    = hhh_FreeHook;
   __realloc_hook = hhh_ReallocHook;
   __malloc_hook  = hhh_MallocHook;
}

/* _________________________________________________________________________ */
static void hhh_Hooks_uninstall() {
   __malloc_hook  = s_OldMallocHook;
   __realloc_hook = s_OldReallocHook;
   __free_hook    = s_OldFreeHook;
}

/* _________________________________________________________________________ */
static void hhh_Hooks_start() {
   gFlags |= IS_RUNNING;
}

/* _________________________________________________________________________ */
static void hhh_Hooks_stop() {
/* accept no more new info, but keep tracking old, through realloc and free */
   gFlags &= gFlags ^ IS_RUNNING;
}


/* _________________________________________________________________________ */
/* out-of-line to ensure symbol exists */
static void captureTrace( void *result ) {
   void *addresses[ hhh_TRACEDEPTH ];
   int tcount, jaddr;
   struct hhh_MemoryTrace *b;

   if ( ! ( gFlags & DOUBLE_DEL_CHECK ) )
      return;

#if defined ( __i386 )
   tcount = hhh_GenericBacktrace( addresses, hhh_TRACEDEPTH );
#elif defined( __x86_64__ )
/* FIXME: backtrace uses _Unwind_Backtrace, which tends to be slow */
   tcount = backtrace( addresses, hhh_TRACEDEPTH );
#else
   tcount = backtrace( addresses, hhh_TRACEDEPTH );
#endif

   assert( tcount <= hhh_TRACEDEPTH );

/* save full traceback info */
   b = hhh_MemoryTrace_new();
   if ( b == 0 ) {
      hhh_Hooks_stop();
      fprintf( stderr, "Hephaestus ERROR: MemoryTrace_new allocation failed ... tracing stopped\n" );
      return;
   }

   hhh_MemoryTrace_initialize( b, (unsigned long)-1 /* dummy */,
                               addresses + hhh_SYSTEMOFF,
                               tcount - hhh_SYSTEMOFF);

   hhh_HashTable_insert( gTraceInfo, result, b );
}

/* _________________________________________________________________________ */
static void *hhh_MallocHook( size_t size, const void *unused ) {
   void *result;

/* reset (system/NULL) hook to prevent recursion */
   hhh_Hooks_uninstall();

/* let malloc do its thing */
   result = malloc( size );

/* stop tracking this pointer if it was previously free()d */
   if ( gTraceInfo )
      hhh_HashTable_remove( gTraceInfo, result, &hhh_MemoryTrace_delete );

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();

   return result;
}

/* _________________________________________________________________________ */
static void *hhh_ReallocHook( void *ptr, size_t size, const void *unused ) {
   void *result;

/* reset (system/NULL) hook to prevent recursion */
   hhh_Hooks_uninstall();

/* let realloc do its thing */
   result = realloc( ptr, size );

/* stop tracking this pointer if it was previously free()d */
   if ( gTraceInfo )
      hhh_HashTable_remove( gTraceInfo, result, &hhh_MemoryTrace_delete );

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();

   return result;
}

/* _________________________________________________________________________ */
#define hhh_DEEPTRACEDEPTH 2*hhh_TRACEDEPTH
static void hhh_FreeHook( void *ptr, const void *unused ) {
   struct hhh_MemoryTrace *ti = 0;
   int tcount = 0, i = 0;
   char** symbols;
   void* addresses[ hhh_DEEPTRACEDEPTH ];

/* reset (system/NULL) hook to prevent recursion */
   hhh_Hooks_uninstall();

/* track this free, if called on a non-NULL pointer */
   if ( ptr != NULL && gTraceInfo ) {
      ti = (struct hhh_MemoryTrace*)hhh_HashTable_find( gTraceInfo, ptr );

      if ( ti != NULL ) {
         fprintf( stderr, "*** Hephaestus ... double free detected (%p), original free:\n", ptr );
         hhh_MemoryTrace_print( stderr, ti, 0 );
         fprintf( stderr, "\n" );

         fprintf( stderr, "*** Hephaestus ... double free detected (%p), current free:\n", ptr );

#if defined ( __i386 )
         tcount = hhh_GenericBacktrace( addresses, hhh_DEEPTRACEDEPTH );
#elif defined( __x86_64__ )
      /* FIXME: backtrace uses _Unwind_Backtrace, which tends to be slow */
         tcount = backtrace( addresses, hhh_DEEPTRACEDEPTH );
#else
         tcount = backtrace( addresses, hhh_DEEPTRACEDEPTH );
#endif

         for ( i = hhh_SYSTEMOFF; i < tcount; i++ )
            fprintf( stderr, "   %p %s\n", addresses[i], hhh_getSymbol( addresses[i] ) );

         hhh_HashTable_remove( gTraceInfo, ptr, *&hhh_MemoryTrace_delete );
      }

      if ( gFlags & IS_RUNNING )
         captureTrace( ptr );

   }

/* normal, actual free */
   free( ptr );

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();
}


/*========================================================================== */
/* startup and shutdown */
/*========================================================================== */
static void setup() __attribute__ ((constructor));
static void teardown() __attribute__ ((destructor));

/* _________________________________________________________________________ */
static void setup() {
   s_OldMallocHook = 0;
   s_OldReallocHook = 0;
   s_OldFreeHook = 0;

   gTraceInfo   = hhh_HashTable_new( hhh_HASHTABLE_MINSIZE );
}

/* _________________________________________________________________________ */
static void teardown() {
   static int finalized = 0;

   if ( ! finalized ) {
      finalized = 1;

   /* reset (system/NULL) as we're done now */
      hhh_Hooks_uninstall();

      hhh_HashTable_delete( gTraceInfo, &hhh_MemoryTrace_delete ); gTraceInfo = 0;
   }
}


/*========================================================================== */
/* python interface module */
/*========================================================================== */
#define CPPTOPYTHON( fname )                                                  \
static PyObject* hep_##fname( PyObject* unused, PyObject* args ) {            \
   if ( ! PyArg_ParseTuple( args, (char*)":"#fname ) )                        \
      return 0;                                                               \
                                                                              \
   hhh_Hooks_##fname();                                                       \
                                                                              \
   Py_INCREF( Py_None );                                                      \
   return Py_None;                                                            \
}

CPPTOPYTHON( save )
CPPTOPYTHON( install )
CPPTOPYTHON( start )
CPPTOPYTHON( stop )
CPPTOPYTHON( uninstall )


/* _________________________________________________________________________ */
static PyMethodDef gDeleteCheckerMethods[] = {
   { (char*)"save",      (PyCFunction)hep_save,      METH_VARARGS, (char*)"save pre-existing hooks" },
   { (char*)"install",   (PyCFunction)hep_install,   METH_VARARGS, (char*)"install checker" },
   { (char*)"start",     (PyCFunction)hep_start,     METH_VARARGS, (char*)"start checking for double deletes" },
   { (char*)"stop",      (PyCFunction)hep_stop,      METH_VARARGS, (char*)"stop checking for double deletes" },
   { (char*)"uninstall", (PyCFunction)hep_uninstall, METH_VARARGS, (char*)"uninstall checker" },

   { NULL, NULL, 0, NULL }
};

/* _________________________________________________________________________ */
PyObject* initDoubleDeleteChecker() {
   PyObject* ddcheck;

   ddcheck = Py_InitModule( (char*)"DeleteChecker", gDeleteCheckerMethods );
   return ddcheck;
}
