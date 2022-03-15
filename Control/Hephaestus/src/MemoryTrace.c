/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/MemoryTrace.h"
#include "Hephaestus/Utils.h"

#include <pthread.h>
#include <dlfcn.h>


/* _________________________________________________________________________ */
#if 0
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
#endif


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
      true_size = sizeof( struct hhh_MemoryTrace );

      const long nobj = sysconf( _SC_PAGESIZE ) / true_size;
      gPool = (char*)malloc( nobj * true_size );

      if ( gPool == NULL ) {
         fprintf( stderr, "Hephaestus ERROR: memory allocation failed in MemoryTrace_new\n" );
         pthread_mutex_unlock( &gPoolLock );
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
void hhh_MemoryTrace_initialize( struct hhh_MemoryTrace *mt, long size,
                                 StackElement* addresses, long depth)
{
/* initialize a newly created MemoryTrace */
   int itrace;

   mt->size = size;
   mt->handle = hhh_stackstash_store (addresses, depth);
}

/* _________________________________________________________________________ */
const char* hhh_MemoryTrace_getType( struct hhh_MemoryTrace *mt )
{
   return 0;
#if 0
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
#endif
}

/* _________________________________________________________________________ */
void hhh_MemoryTrace_print( FILE* f, struct hhh_MemoryTrace *mt, int hideMemAddr )
{
/* report on traceback */
   const char *tp, *sym;
   char *h;
   int i;

/* report culprit */
   StackElement* elt = STACK_HANDLE_ELEMENT (mt->handle);
   sym = (const char*)hhh_getSymbol( *elt );
   if ( (h = strstr( sym, " @" )) ) *h = '\0';
   if ( mt->size != (unsigned long)-1 )
      fprintf( f, "%s (%lu bytes)\n", hhh_getSymbol( *elt ), mt->size );
   else
      fprintf( f, "%s\n", hhh_getSymbol( *elt ) );
   if ( h ) *h = ' ';

/* try to report detailed type information */
   tp = hhh_MemoryTrace_getType( mt );
   if ( tp )
      fprintf( f, "    => guess of object type: %s\n", tp );

/* report (partial) stack trace */
   while (!STACK_ELEMENT_IS_ROOT(elt)) {
      if (hideMemAddr)
        fprintf( f, "   %s %s\n", hhh_getSymbol( *elt ),
                 hhh_addrToLine(*elt, NULL));
      else
        fprintf( f, "   %p %s %s\n", *elt, hhh_getSymbol( *elt ),
                 hhh_addrToLine(*elt, NULL));
      elt = STACK_ELEMENT_PARENT (elt);
   }
}

/* _________________________________________________________________________ */
int hhh_MemoryTracePtr_compare( const void *pmt1, const void *pmt2 )
{
/* comparison function for memory traces, returns like strcmp */
   struct hhh_MemoryTrace *mt1, *mt2;

   mt1 = *((struct hhh_MemoryTrace**)pmt1);
   mt2 = *((struct hhh_MemoryTrace**)pmt2);

   if ( mt1->size < mt2->size )
      return 1;

   if ( mt1->size > mt2->size )
      return -1;

   if ( mt1->handle < mt2->handle )
      return 1;

   if ( mt1->handle > mt2->handle )
      return -1;

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

#if PY_MAJOR_VERSION >= 3
   static struct PyModuleDef moduledef = {
     PyModuleDef_HEAD_INIT,
     "MemoryTrace",     /* m_name */
     "Memory trace",  /* m_doc */
     -1,                  /* m_size */
     gMemoryTraceMethods,    /* m_methods */
     NULL,                /* m_reload */
     NULL,                /* m_traverse */
     NULL,                /* m_clear */
     NULL,                /* m_free */
   };
   memtrace = PyModule_Create (&moduledef);
#else   
   memtrace = Py_InitModule( (char*)"MemoryTrace", gMemoryTraceMethods );
#endif
   return memtrace;
}
