/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/MemoryTrace.h"
#include "Hephaestus/Utils.h"

#include <pthread.h>


/* _________________________________________________________________________ */
static char *strrstr( const char *s1, const char *s2 ) {
/* Kent Irwin, irwin@leland.stanford.edu */
   const char *sc2, *psc1, *ps1;

   if ( *s2 == '\0' )
      return (char*)s1;
		
   ps1 = s1 + strlen( s1 );
	
   while ( ps1 != s1 ) {
      --ps1;
      for ( psc1 = ps1, sc2 = s2; ; )
         if ( *(psc1++) != *(sc2++) )
            break;
         else if ( *sc2 == '\0' )
            return (char*)ps1;
   }

   return (char*)NULL;
}


/* - use an arena for alloction of traces ---------------------------------- */
unsigned long hhh_gBacktraceSize = hhh_BACKTRACE_SIZE;
static char *gPool = 0;
static int gPoolInUse = 0;
static pthread_mutex_t gPoolLock = PTHREAD_MUTEX_INITIALIZER;

struct hhh_MemoryTrace *hhh_MemoryTrace_new() {
/* allocate objects in a single memory block, admin w/ linked list */
   int i = 0;
   unsigned long true_size = 0;
   struct hhh_MemoryTrace *fresh = 0;

   pthread_mutex_lock( &gPoolLock );

   if ( gPool == NULL ) {
      gPoolInUse = 1;             /* indicate to user that trace size can no longer be modified */
      true_size = sizeof( struct hhh_MemoryTrace ) + hhh_gBacktraceSize*sizeof(void*);

      const long nobj = sysconf( _SC_PAGESIZE ) / true_size;
      gPool = (char*)malloc( nobj * true_size );

      if ( gPool == NULL ) {
         fprintf( stderr, "Hephaestus ERROR: memory allocation failed in MemoryTrace_new\n" );
         return NULL;
      }

      for ( i = 0; i < nobj-1; i++ ) {
	 ((struct hhh_MemoryTrace*)(gPool + i*true_size))->size = (unsigned long)(gPool + (i+1)*true_size);
      }
      ((struct hhh_MemoryTrace*)(gPool + (nobj-1)*true_size))->size = 0;
   }

   fresh = (struct hhh_MemoryTrace*)gPool;
   gPool = (char*)fresh->size;

   pthread_mutex_unlock( &gPoolLock );

   return fresh;
}

void hhh_MemoryTrace_delete( void *trace ) {
/* recycle the object memory, by putting it back into the list */
   if ( trace == 0 )
      return;

   pthread_mutex_lock( &gPoolLock );

   ((struct hhh_MemoryTrace*)trace)->size = (unsigned long)gPool;
   gPool = (char*)trace;

   pthread_mutex_unlock( &gPoolLock );
}


/* _________________________________________________________________________ */
void hhh_MemoryTrace_initialize( struct hhh_MemoryTrace *mt, long size )
{
/* initialize a newly created MemoryTrace */
   int itrace;

   mt->size = size;

   for ( itrace = 0; itrace < hhh_gBacktraceSize; ++itrace )
      ((void**)&mt->trace)[ itrace ] = 0;
}

/* _________________________________________________________________________ */
const char* hhh_MemoryTrace_getType( struct hhh_MemoryTrace *mt )
{
   return 0;

/* get the type that was newed, if available */
   int i;
   void *ctor;
   char *s1, *s2 = 0;

   ctor = hhh_getConstructor( hhh_MT_ORIGINATOR( *mt ) );
   s1 = (char*)(ctor ? (void*)hhh_getSymbol( ctor ) : ctor);
   if ( s1 ) {
      s2 = (char*)malloc( strlen( s1 ) + 1 );
      strcpy( s2, s1 );

      if ( (s1 = strrstr( s2, "::" )) ) *s1 = '\0';
   }

   return s2;
}

/* _________________________________________________________________________ */
void hhh_MemoryTrace_print( FILE* f, struct hhh_MemoryTrace *mt, int hideMemAddr )
{
/* report on traceback */
   const char *tp, *sym;
   char *h;
   int i;

/* report culprit */
   sym = (const char*)hhh_getSymbol( ((void**)&mt->trace)[0] );
   if ( (h = strstr( sym, " @" )) ) *h = '\0';
   if ( mt->size != (unsigned long)-1 )
      fprintf( f, "%s (%ld bytes)\n", hhh_getSymbol( ((void**)&mt->trace)[0] ), mt->size );
   else
      fprintf( f, "%s\n", hhh_getSymbol( ((void**)&mt->trace)[0] ) );
   if ( h ) *h = ' ';

/* try to report detailed type information */
   tp = hhh_MemoryTrace_getType( mt );
   if ( tp )
      fprintf( f, "    => guess of object type: %s\n", tp );

/* report (partial) stack trace */
   for ( i = 0; i < hhh_gBacktraceSize; i++ ) {
      if ( ! ((void**)&mt->trace)[i] )
         break;
      if (hideMemAddr)
         fprintf( f, "   %s\n", hhh_getSymbol( ((void**)&mt->trace)[i] ) );
      else
         fprintf( f, "   %p %s\n", ((void**)&mt->trace)[i], hhh_getSymbol( ((void**)&mt->trace)[i] ) );
   }
}

/* _________________________________________________________________________ */
int hhh_MemoryTracePtr_compare( const void *pmt1, const void *pmt2 )
{
/* comparison function for memory traces, returns like strcmp */
   double sum1, sum2;
   int i;
   struct hhh_MemoryTrace *mt1, *mt2;

   mt1 = *((struct hhh_MemoryTrace**)pmt1);
   mt2 = *((struct hhh_MemoryTrace**)pmt2);

   if ( mt1->size < mt2->size )
      return 1;

   if ( mt1->size > mt2->size )
      return -1;

   sum1 = sum2 = 0.;
   for ( i = 0; i < hhh_gBacktraceSize; ++i ) {
      sum1 += (unsigned long)((void**)&mt1->trace)[i];
      sum2 += (unsigned long)((void**)&mt2->trace)[i];
   }

   if ( sum1 == sum2 )
      return 0;

   if ( sum1 > sum2 )
      return 1;

   if ( sum1 < sum2 )
      return -1;

/* just to make clang happy; all cases are already covered above */
   return 0;
}


/*========================================================================== */
/* python interface module */
/*========================================================================== */
static PyObject* hep_size( PyObject* unused, PyObject* args ) {
   long new_size = 0, old_size = 0;

   if ( ! PyArg_ParseTuple( args, (char*)"|l:size", &new_size ) )
      return 0;

   pthread_mutex_lock( &gPoolLock );

   if ( gPoolInUse ) {
      PyErr_SetString( PyExc_RuntimeError, "MemoryTraces have been allocated; too late to change size" );
      pthread_mutex_unlock( &gPoolLock );
      return 0;
   }

/* do not adjust for system offset, as that is probably better to understand for the user */
   old_size = hhh_gBacktraceSize;
   hhh_gBacktraceSize = new_size;

   pthread_mutex_unlock( &gPoolLock );

   return PyLong_FromLong( old_size );
}

/* _________________________________________________________________________ */
static PyMethodDef gMemoryTraceMethods[] = {
   { (char*)"size",      (PyCFunction)hep_size,      METH_VARARGS, (char*)"set size of stored memory traces" },

   { NULL, NULL, 0, NULL }
};

/* _________________________________________________________________________ */
PyObject* initMemoryTrace() {
   PyObject *memtrace;

   memtrace = Py_InitModule( (char*)"MemoryTrace", gMemoryTraceMethods );
   return memtrace;
}
