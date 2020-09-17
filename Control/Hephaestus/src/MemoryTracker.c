/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/CheckPoints.h"
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

/* holder to locate this shared library */
void hhh_dummy() {}

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
#define LEAK_CHECK          2
#define PROFILE             4
#define HIDEMEMADDR         8
#define QUICK              16
#define STL_CHECKS         32
#define FILTER_STL         64
#define FREESTAT          128

/* data holders and their management */
static struct hhh_HashTable *gTraceInfo  = 0;
static struct hhh_HashTable *gPushBacks = 0;
static struct hhh_NameSet {
   char **names;
   unsigned int *lens;
   unsigned int size;
   unsigned int capacity;
} gIgnoreNames, gIgnoreCallNames, gTraceNames;
static struct hhh_HashTable *gOldTracers[ hhh_MAX_CHECKPOINTS + 1 ];

/* for free statistics */
static struct hhh_FreeStatistics {
   double total;
   long untracked;
} gFreeStatistics;

/* for reporting */
struct hhh_LeakSummary {
   struct hhh_MemoryTrace* leak;
   unsigned int count;
};

/* module-level configuration flags */
static long gFlags = LEAK_CHECK | FILTER_STL;

/* handle to output streams/files used */
static FILE *gReportStream = 0;
static FILE *gProfileStream = 0;
static char *gSymbolFileName = 0;

/* default total depth; this totals 20 user-side + 3 system */
static int gLargeTraceDepth = 23;

/* accessor for gTraceInfo ------------------------------------------------- */
struct hhh_HashTable *hhh_getTraceInfo() {
   return gTraceInfo;
}


/* - helpers for dealing with sets of names -------------------------------- */
static void hhh_NameSet_construct( struct hhh_NameSet *nameset, unsigned int size ) {
   nameset->names = (char**)malloc( sizeof(char*)*size );
   nameset->lens = (unsigned int*)malloc( sizeof(unsigned int)*size );
   nameset->size = 0;
   nameset->capacity = size;
}

static void hhh_NameSet_destroy( struct hhh_NameSet *nameset ) {
   unsigned int i;

   for ( i = 0; i < nameset->size; i++ )
      free( nameset->names[i] );
   free( nameset->names );
   free( nameset->lens );
   nameset->names = 0; nameset->capacity = 0; nameset->size = 0;
   nameset->lens = 0;
}

static void hhh_NameSet_Add( struct hhh_NameSet *nameset, const char *name ) {
   char *name_cp;

   if ( nameset->size >= nameset->capacity ) {
      nameset->names =
         (char**)realloc( nameset->names, sizeof(char*)*nameset->capacity*2 );
      nameset->lens =
         (unsigned int*)realloc( nameset->lens, sizeof(unsigned int)*nameset->capacity*2 );
      nameset->capacity *= 2;
   }

   unsigned int len = strlen( name );
   name_cp = (char*)malloc( len + 1 );
   strcpy( name_cp, name );
   nameset->lens[ nameset->size ] = len;
   nameset->names[ nameset->size++ ] = name_cp;
}

static void hhh_NameSet_print( FILE* out, struct hhh_NameSet *nameset, const char* preamb ) {
   unsigned int i;

   if ( nameset->size && preamb )
      fprintf( out, "%s", preamb );

   for ( i = 0; i < nameset->size; ++i )
      fprintf( out, " \"%s\"", nameset->names[i] );
}


/* - helpers for reporting ------------------------------------------------- */
static int hhh_LeakSummary_compare1( const void *vls1, const void *vls2 ) {
   return hhh_MemoryTracePtr_compare(
      &((struct hhh_LeakSummary*)vls1)->leak, &((struct hhh_LeakSummary*)vls2)->leak );
}

static int hhh_LeakSummary_compare2( const void *vls1, const void *vls2 ) {
   struct hhh_LeakSummary *ls1, *ls2;

   ls1 = (struct hhh_LeakSummary*)vls1;
   ls2 = (struct hhh_LeakSummary*)vls2;

   if ( ls1->count == ls2->count )
      return 0;

   if ( ls1->count < ls2->count )
      return -1;

   return 1;
}


/* _________________________________________________________________________ */
static void hhh_report( void ) {
   unsigned int i, iuniq, iname, nuniq, nreports;
   struct hhh_MemoryTrace *mt1, *mt2;
   struct hhh_LeakSummary *uniq = 0, *summary;
   struct hhh_Cell *cell;
   int ignore;
   const char *first;
   char *pos, *sub;
   long reportedLeakSize = 0, ignoredLeakSize = 0;

/* use stdout if no outstream given */
   if ( ! gReportStream )
      gReportStream = stdout;

/* report on detected leaks */
   if ( gFlags & LEAK_CHECK ) {
      fprintf( gReportStream, "\nList of unique leaks" );
      hhh_NameSet_print( gReportStream, &gTraceNames, " in " );
      fprintf( gReportStream, ":\n" );
   }

   if ( ( gFlags & LEAK_CHECK ) && gTraceInfo->nentries == 0 ) {
      fprintf( gReportStream, "no leaks detected\n" );

   } else if ( gFlags & LEAK_CHECK ) {

   /* report actual leaks with traceback only once */
      uniq = (struct hhh_LeakSummary*)malloc( sizeof(struct hhh_LeakSummary) * gTraceInfo->nentries );
      iuniq = 0;
      for ( i = 0; i < gTraceInfo->size; i++ ) {
         cell = gTraceInfo->table[i];
         while ( cell ) {
            uniq[iuniq++].leak = (struct hhh_MemoryTrace*)cell->value;
            cell = cell->next;
         }
      }

      qsort( uniq, gTraceInfo->nentries, sizeof(struct hhh_LeakSummary), hhh_LeakSummary_compare1 );
      
      nuniq = 0;
      uniq[ 0 ].count = 1;
      for ( iuniq = 1; iuniq < gTraceInfo->nentries; ++iuniq ) {
         mt1 = uniq[nuniq].leak;
         mt2 = uniq[iuniq].leak;
         if ( hhh_MemoryTracePtr_compare( &mt1, &mt2 ) == 0 && mt1->size == mt2->size ) {
            uniq[ nuniq ].count += 1;
         } else {
            uniq[ ++nuniq ].leak = mt2;
            uniq[ nuniq ].count = 1;         /* first occurence of this leak */
         }         
      }
      nuniq += 1;                            /* counter started at zero, so total is +1 */

   /* sort for importance */
      qsort( uniq, nuniq, sizeof(struct hhh_LeakSummary), hhh_LeakSummary_compare2 );

   /* print report on stdout */
      long reportedLeakSize = 0;
      long ignoredLeakSize = 0;
      nreports = 0;
      for ( iuniq = 0; iuniq < nuniq; ++iuniq ) {
         summary = &uniq[ iuniq ];

         first = hhh_getSymbol( hhh_MT_ORIGINATOR( *summary->leak ) );
         pos = strstr( first, "(" );

       /* determine whether to ignore this report; since the match is based on a search,
          function arguments (if any) are stripped so that they don't accidently match */
         ignore = 0;
         if ( pos ) {
           sub = (char*)malloc( pos-first + 1 );
            strncpy( sub, first, pos-first );
            sub[ pos-first ] = '\0';
         } else {
            sub = (char*)first;
         }

         for ( iname = 0; iname < gIgnoreNames.size; ++iname ) {            
            if ( strstr( sub, gIgnoreNames.names[iname] ) ) {
               ignore = 1;
               break;
            }
         }

         {
           StackElement* elt;
           for (elt = STACK_HANDLE_ELEMENT (summary->leak->handle);
                !STACK_ELEMENT_IS_ROOT(elt);
                elt = STACK_ELEMENT_PARENT (elt))
           {
             const char* s = hhh_getSymbol( *elt );
             int iname;
             for ( iname = 0; iname < gIgnoreCallNames.size; ++iname ) {
               if ( strncmp (s, gIgnoreCallNames.names[iname], gIgnoreCallNames.lens[iname]) == 0 )
               {
                 ignore = 1;
                 break;
               }
             }
           }
         }

         if ( pos )
            free( sub );
         sub = 0;

       /* report the leak, or add its size to the total ignored, as appropriate */
      /*         long leakSize = nOcc[iuniq]*((struct hhh_MemoryTrace*)uniq[iuniq])->size;*/
         long leakSize = summary->count * summary->leak->size;
         if ( ! ignore ) {
            nreports += 1;
            fprintf( gReportStream, "%u leak%s (total %ld bytes), originating in:\n",
                     summary->count, (summary->count == 1 ? "" : "s"),
                     leakSize );
            
            hhh_MemoryTrace_print( gReportStream, summary->leak, gFlags & HIDEMEMADDR );
            fprintf( gReportStream, "\n" );

            reportedLeakSize += leakSize;
         }
         else
            ignoredLeakSize += leakSize;
      }

   /* report summary */
      fprintf( gReportStream, "Leak summary" );
      hhh_NameSet_print( gReportStream, &gTraceNames, " for " );
      fprintf( gReportStream, ":\n" );

      fprintf( gReportStream, "   detected %u unique leak%s\n",
               nuniq, (nuniq == 1 ? "" : "s" ) );
            
      fprintf( gReportStream, "   detected %d non-unique leak%s (total %ld bytes reported)\n",
               gTraceInfo->nentries, ( gTraceInfo->nentries == 1 ? "" : "s" ),
               reportedLeakSize );

      if ( (nuniq - nreports) != 0 )
         fprintf( gReportStream,
                  "   ignored %u unique leak%s using %u filter%s (total %ld bytes ignored)\n",
                  nuniq - nreports, ( nuniq - nreports == 1 ? "" : "s" ),
                  gIgnoreNames.size, ( gIgnoreNames.size == 1 ? "" : "s"),
                  ignoredLeakSize);
   }

   free( uniq ); uniq = 0;
   
/* report on push_backs */
   if ( ( gFlags & STL_CHECKS ) && gPushBacks->nentries != 0 ) {
      fprintf( gReportStream, "\nTotal number of allocating push_backs per callee:\n" );
      for ( i = 0; i < gPushBacks->size; i++ ) {
         cell = gPushBacks->table[i];
         while ( cell ) {
            fprintf( gReportStream, "%8ld %s\n", *((long*)cell->value), hhh_getSymbol( cell->key ) );
            cell = cell->next;
         }
      }
   }

   fflush( gReportStream );
}

/* _________________________________________________________________________ */
static void trace( const char *name ) {
   int iname;
   char *name_cp;

/* reset the set of names if name is the empty string (which matches all, so the
   more restrictive filters, if any, are no longer needed) */
   if ( ! name || strlen(name) == 0 ) {
      for ( iname = 0; iname < gTraceNames.size; iname++ )
         free( gTraceNames.names[iname] );
      gTraceNames.size = 0;
      return;
   }

   hhh_NameSet_Add( &gTraceNames, name );
}

/* _________________________________________________________________________ */
static void ignore( const char *name ) {
   hhh_NameSet_Add( &gIgnoreNames, name );
}

/* _________________________________________________________________________ */
static void ignoreCall( const char *name ) {
   hhh_NameSet_Add( &gIgnoreCallNames, name );
}

/* _________________________________________________________________________ */
static long configure( long flags ) {
   if ( flags )
      gFlags = flags | ( gFlags & IS_RUNNING );
   return gFlags;
}

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
void hhh_Hooks_start() {
   gFlags |= IS_RUNNING;
}

/* _________________________________________________________________________ */
void hhh_Hooks_stop() {
/* accept no more new info, but keep tracking old, through realloc and free */
   gFlags &= gFlags ^ IS_RUNNING;
}

/* _________________________________________________________________________ */
static void hhh_Hooks_atexit() {
   unsigned long i;
   int itrace;
   struct hhh_Cell *cell;
   struct hhh_MemoryTrace *bt;

/* reset (system/NULL) hook to prevent tracking */
   hhh_Hooks_uninstall();

/* force resolving all addresses; results are cached */
   if ( gFlags & QUICK ) {
   /* most addresses have not been resolved yet, do so now */
      for ( i = 0; i < gTraceInfo->size; i++ ) {
         cell = gTraceInfo->table[i];
         while ( cell != NULL ) {
            if ( cell->value != NULL ) {
               StackElement* elt;
               bt = (struct hhh_MemoryTrace*)cell->value;
               elt = STACK_HANDLE_ELEMENT (bt->handle);
               while (!STACK_ELEMENT_IS_ROOT (elt)) {
                  hhh_getSymbol( *elt );
                  elt = STACK_ELEMENT_PARENT (elt);
               }
            }

            cell = cell->next;
         }
      }
   }

/* heuristic to be unloaded as late as possible */
   hhh_setLinkLate( (void*)&hhh_dummy );

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();
}


/* out-of-line to ensure symbol exists */
static void captureTrace( void *result, long size, int isrealloc ) {
   static void *addr_malloc  = NULL;
   static void *addr_new     = NULL;
   static void *addr_new_arr = NULL;
   static void *addr_dl_open = NULL;

   const int traceDepth = 2048;
   void* addresses[traceDepth];
   void *current;
   int tcount, responsible, mallocoff, i, j, k, iname, accept;
   const char *name;
   struct hhh_MemoryTrace *b;
   double *total;
   long *pbcount, ifilter;

#if defined ( __i386 )
   tcount = hhh_GenericBacktrace( addresses, traceDepth );
#elif defined( __x86_64__ )
/* FIXME: backtrace uses _Unwind_Backtrace, which tends to be slow */
   tcount = backtrace( addresses, traceDepth );
#else
   tcount = backtrace( addresses, traceDepth );
#endif

   assert( tcount <= traceDepth );

/* tcount will (in ATLAS) almost always be larger than hhh_TRACEDEPTH (max size
   of stack traces kept in memory). The filter will work up to gLargeTraceDepth
   even as only up to hhh_BACKTRACE_SIZE (hhh_TRACEDEPTH - hhh_SYSTEMOFF) is kept. */

   responsible = mallocoff = 0;
   for ( i = 1 /* can at least skip the hook */; i < tcount; i++ ) {
   /* need to find the actual function that allocates the memory */
      current = addresses[i];

   /* if located, quick-treat the allocating and special cases */
      if ( current == addr_malloc )
      {
         responsible = i+1 < tcount ? i+1 : i;
         mallocoff = i+1;
         continue;
      }

      if ( current == addr_new || current == addr_new_arr ) {
         responsible = i+1 < tcount ? i+1 : i;
         continue;
      }

      if ( current == addr_dl_open ) /* off-loaded _after_ Hephaestus, so can't trace */
         break;

      name = hhh_getSymbolE( current );
      if ( ! name ) {
         if ( isrealloc && ( ! responsible || responsible == i ) ) {
            break;     /* all too likely dld.so reallocating linker tables */
         }

         continue; /* part of the overall stack, can't filter */
      }

   /* set of special cases for which addresses are saved (STL, new, new[]);
      the rationale of filtering STL is that leaks from it always occur as
      part of another leak (e.g. std::string that is a data member of a
      leaksed object) and the duplicate report only confuses */
      if ( (gFlags & FILTER_STL) &&
           (strncmp( name, "std::", 5 ) == 0 || strncmp( name, "__gnu_cxx", 9 ) == 0) ) {
         break;                    /* filter STL allocations (see above) */

      } else if ( ! addr_malloc && (strstr( name, "malloc" ) == name || strstr( name, "__libc_malloc" ) == name) ) {
         addr_malloc  = current;
         responsible = i+1 < tcount ? i+1 : i;
         mallocoff = i+1;

      } else if ( ! addr_new && strstr( name, "operator new" ) == name ) {
         addr_new     = current;
         responsible = i+1 < tcount ? i+1 : i;

      } else if ( ! addr_new_arr && strstr( name, "operator new[]" ) == name ) {
         addr_new_arr = current;
         responsible = i+1 < tcount ? i+1 : i;

      } else if ( ! addr_dl_open && strstr( name, "_dl_open" ) == name ) {
         addr_dl_open = current;
         break;

      } else if ( ( gFlags & STL_CHECKS ) && (       /* assumes that push_back does not match filter */
           ( strstr( name, "push_back" ) && i+1 < tcount ) ||       /* not exact b/c of template arg */
           ( strstr( name, "_M_insert_aux" ) && i+1 < tcount ) ) ) { /* in case push_back is inlined */
         pbcount = (long*)hhh_HashTable_find( gPushBacks, addresses[i+1] );
         if ( ! pbcount ) {
            pbcount = (long*)malloc( sizeof( long ) );
            *pbcount = 1;
            hhh_HashTable_insert( gPushBacks, addresses[i+1], (void*)pbcount );
         } else {
            *pbcount += 1;
         }

   /* normal case of matching the function name to the filter */
      } else {

         if ( ! responsible )
            responsible = hhh_SYSTEMOFF;  /* e.g. if malloc/realloc called directly */

         if ( gTraceNames.size != 0 ) {
         // filter traces as requested
            accept = 0;
            for ( iname = 0; iname < gTraceNames.size; ++iname ) {
               if ( strstr( name, gTraceNames.names[iname] ) ) {
                  accept = 1;
                  ifilter = i-mallocoff;
                  break;
               }
            }

         } else {
         // always accept all traces
            accept = 1;
            ifilter = hhh_gBacktraceSize;
         }

         if ( ! accept )
            continue;

         if ( gFlags & LEAK_CHECK ) {
         /* force lookup of symbols, such that they are cached */
            if ( ! ( gFlags & QUICK ) && i+1 < tcount ) {
              for ( j = i+1; j < tcount; j++ ) {
                hhh_getSymbol( addresses[j] );
              }
            }

         /* save full traceback info */
            b = hhh_MemoryTrace_new();
            if ( b == 0 ) {
               hhh_Hooks_stop();
               fprintf( stderr, "Hephaestus ERROR: MemoryTrace_new allocation failed ... tracing stopped\n" );
               return;
            }

            hhh_MemoryTrace_initialize( b, size,
                                        addresses + responsible,
                                        tcount - responsible );

            hhh_HashTable_insert( gTraceInfo, result, b );
         }

         if ( gFlags & PROFILE ) {
            if ( ! gProfileStream ) {
               (void)remove( "hephaestus.prof" );
               gProfileStream = popen( "gzip -f >> hephaestus.prof", "w" );
            }

            fwrite( &size, sizeof( long ), 1, gProfileStream );
            fwrite( &ifilter, sizeof( long ), 1, gProfileStream );
            fwrite( &addresses[ mallocoff ], sizeof(void*), ifilter, gProfileStream );
         }

         break;
      }

   }
}

/* _________________________________________________________________________ */
static void *hhh_MallocHook( size_t size, const void *unused ) {
   void *result;

/* reset (system/NULL) hook to prevent recursion */
   hhh_Hooks_uninstall();

/* let malloc do its thing */
   result = malloc( size );

/* track, if stack passes filter (the predicate is to prevent rogue calls) */
   if ( gFlags & IS_RUNNING )
      captureTrace( result, size, 0 );

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();

   return result;
}

/* _________________________________________________________________________ */
static void *hhh_ReallocHook( void *ptr, size_t size, const void *unused ) {
   void *result;
   struct hhh_HashTable *tf = 0;
   int i = 0;

/* reset (system/NULL) hook to prevent recursion */
   hhh_Hooks_uninstall();

/* let realloc do its thing */
   result = realloc( ptr, size );

/* memory size may have changed or perhaps memory was freed w/ tracking off */
   if ( ! hhh_HashTable_remove( gTraceInfo, ptr, &hhh_MemoryTrace_delete ) ) {

   /* not in current tracer ... search in old blocks */
      while ( ( tf = gOldTracers[ i++ ] ) ) {
         if ( hhh_HashTable_remove( tf, ptr, &hhh_MemoryTrace_delete ) )
            break;
      }
   }

/* track (or re-track if ptr == result), if stack passes filter */
   if ( gFlags & IS_RUNNING )
      captureTrace( result, size, 1 );

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();

   return result;
}

/* _________________________________________________________________________ */
static void hhh_FreeHook( void *ptr, const void *unused ) {
   struct hhh_HashTable *tf = 0;
   struct hhh_MemoryTrace *mt = 0;
   int i = 0;

/* reset (system/NULL) hook to prevent recursion */
   hhh_Hooks_uninstall();

/* collect some statistics if requested; the extra find is somewhat unfortunate
   but can't be helped for now */
   if ( gFlags & FREESTAT ) {
      mt = (struct hhh_MemoryTrace*)hhh_HashTable_find( gTraceInfo, ptr );

      if ( mt ) {
         gFreeStatistics.total += mt->size;
      } else {
         gFreeStatistics.untracked += 1;
      }

   }

/* cleanup, both in our info (if traced), as well as actual memory free */
   if ( ! hhh_HashTable_remove( gTraceInfo, ptr, &hhh_MemoryTrace_delete ) ) {

   /* not in current tracer ... search in old blocks */
      while ( ( tf = gOldTracers[ i++ ] ) ) {
         if ( hhh_HashTable_remove( tf, ptr, &hhh_MemoryTrace_delete ) )
            break;
      }
   }
   free( ptr );

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();
}


/*========================================================================== */
/* checkpoints interface */
/*========================================================================== */
int hhh_setCheckPoint( void ) {
/* set a new check point, which will become current */
   int i, result = 0;

/* reset (system/NULL) hook to prevent tracking */
   hhh_Hooks_uninstall();

/* move current trace data points */
   for ( i = 0; i < hhh_MAX_CHECKPOINTS; i++ ) {
      if ( gOldTracers[ i ] == 0 ) {
         gOldTracers[ i ] = gTraceInfo;
         break;
      }
   }

   if ( i < hhh_MAX_CHECKPOINTS ) {
   /* create new, current, tracking point */
      gTraceInfo   = hhh_HashTable_new( hhh_HASHTABLE_MINSIZE );
      result = 1;
   } else {
   /* failure: too many check points */
      result = 0;
   }

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();

   return result;
}

/* _________________________________________________________________________ */
int hhh_clearCheckPoint( CheckPointPos_t offset ) {
/* consolidate the check point with the general memory tracking */
   int i = -1, result = 0;

/* reset (system/NULL) hook to prevent tracking */
   hhh_Hooks_uninstall();

/* find last tracer */
   while ( gOldTracers[ ++i ] );
   i -= 1;

   if ( offset == 0 && 0 <= i ) {
   /* destroy the current tracer, and assign/reset to the global one */
      if ( 0 < i )
         hhh_HashTable_merge( gOldTracers[ 0 ], gTraceInfo );

      hhh_HashTable_delete( gTraceInfo, 0 ); /* no delete of values! */
      gTraceInfo = gOldTracers[ i ];
      gOldTracers[ i ] = 0;

      result = 1;

   } else if ( 0 < i+2-offset ) {
   /* locate and destroy the old tracer, close the gap if any */
      i -= offset-2;
      hhh_HashTable_merge( gOldTracers[ 0 ], gOldTracers[ i ] );

      hhh_HashTable_delete( gOldTracers[ i ], 0 );
      for ( ; i < hhh_MAX_CHECKPOINTS; ++i ) {
         if ( ! ( gOldTracers[ i ] = gOldTracers[ i+1 ] ) )
            break;
      }

      result = 1;
   }

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();

   return result;
}

/* _________________________________________________________________________ */
int hhh_check( CheckPointPos_t offset ) {
/* print out statistics for the given check point */
   struct hhh_HashTable *tf = 0;
   int i;

/* reset (system/NULL) hook to prevent tracking */
   hhh_Hooks_uninstall();

   if ( offset == 0 ) {
   /* report on current checkpoint */
      hhh_report();
   } else {
   /* locate the requested tracer and report */
      i = -1;
      while ( gOldTracers[ ++i ] );
      i -= offset-1;

      if ( i >= 1 && gOldTracers[ i ] ) {
      /* TODO: change hhh_report interface */
         tf = gTraceInfo;
         gTraceInfo = gOldTracers[ i ];
         hhh_report();
         gTraceInfo = tf;
      }
   }

/* store current hooks */
   hhh_Hooks_save();

/* reset tracker hook */
   hhh_Hooks_install();

   return 1;
}

/* _________________________________________________________________________ */
int hhh_depth( int depth ) {
   if ( depth < hhh_TRACEDEPTH ) {
      printf( "Hephaestus WARNING: depth requested (%d) less than minimum (%d) ... minimum chosen\n", depth, hhh_TRACEDEPTH );
      gLargeTraceDepth = hhh_TRACEDEPTH;
      return 0;
   }

   gLargeTraceDepth = depth + hhh_SYSTEMOFF;

   return 1;
}


/*========================================================================== */
/* startup and shutdown */
/*========================================================================== */
static void setup() __attribute__ ((constructor));
static void teardown() __attribute__ ((destructor));

/* _________________________________________________________________________ */
static void setup() {
   void *trace[1];

   s_OldMallocHook = 0;
   s_OldReallocHook = 0;
   s_OldFreeHook = 0;

   gTraceInfo   = hhh_HashTable_new( hhh_HASHTABLE_MINSIZE );
   gPushBacks   = hhh_HashTable_new( hhh_HASHTABLE_MINSIZE );
   hhh_NameSet_construct( &gTraceNames, 2 );
   hhh_NameSet_construct( &gIgnoreNames, 10 );
   hhh_NameSet_construct( &gIgnoreCallNames, 10 );

   gOldTracers[ hhh_MAX_CHECKPOINTS ] = 0;

   gFreeStatistics.total     = 0.;
   gFreeStatistics.untracked = 0;

/* get a one-time malloc from backtrace done and over with */
   backtrace( trace, 1 );
}

/* _________________________________________________________________________ */
static void teardown() {
   static int finalized = 0;
   unsigned int iname;
   FILE *fsymbols;
   char *cmd;

   if ( ! finalized ) {
      finalized = 1;

   /* reset (system/NULL) as we're done now */
      hhh_Hooks_uninstall();

      hhh_report();

   /* save all names into the output file */
      if ( gFlags & PROFILE ) {

         if ( gSymbolFileName ) {
            (void)remove( gSymbolFileName );
            cmd = (char*)malloc( 16+strlen( gSymbolFileName )+1 );
            sprintf( cmd, "gzip -f >> %s.symb", gSymbolFileName );
            fsymbols = popen( cmd, "w" );
            free( cmd );
            free( gSymbolFileName ); gSymbolFileName = 0;
         } else {
            (void)remove( "hephaestus.symb" );
            fsymbols  = popen( "gzip -f >> hephaestus.symb", "w" );
         }

         hhh_writeSymbols( fsymbols );
         pclose( fsymbols );
      }

   /* done with data stream */
      if ( gProfileStream )
         pclose( gProfileStream );

      hhh_NameSet_destroy( &gIgnoreNames );
      hhh_NameSet_destroy( &gIgnoreCallNames );
      hhh_NameSet_destroy( &gTraceNames );

      hhh_HashTable_delete( gPushBacks,  &free ); gPushBacks = 0;
   /* TODO: cleanup contents of old tracers if not done by user */
      hhh_HashTable_delete( gTraceInfo, &hhh_MemoryTrace_delete ); gTraceInfo = 0;

   /* done with outstream */
      if ( gReportStream && gReportStream != stdout ) {
         fclose( gReportStream );
         gReportStream = 0;
      }
   }

   hhh_utils_cleanup();
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
CPPTOPYTHON( atexit )

/* _________________________________________________________________________ */
#define CPPTOPYTHONWITHSTRING( fname )                                        \
static PyObject* hep_##fname( PyObject* unused, PyObject* args ) {            \
   const char *name;                                                          \
   if ( ! PyArg_ParseTuple( args, (char*)"s:"#fname, &name ) )                \
      return 0;                                                               \
                                                                              \
   fname( name );                                                             \
                                                                              \
   Py_INCREF( Py_None );                                                      \
   return Py_None;                                                            \
}

CPPTOPYTHONWITHSTRING( trace )
CPPTOPYTHONWITHSTRING( ignore )
CPPTOPYTHONWITHSTRING( ignoreCall )

/* _________________________________________________________________________ */
#define CPPTOPYTHONWITHINT( fname )                                           \
static PyObject* hep_##fname( PyObject* unused, PyObject* args ) {            \
   int i, res;                                                                \
   if ( ! PyArg_ParseTuple( args, "i", &i ) )                                 \
      return 0;                                                               \
                                                                              \
   res = hhh_##fname( i );                                                    \
   return Py_BuildValue( "i", res );                                          \
}

CPPTOPYTHONWITHINT( check )
CPPTOPYTHONWITHINT( clearCheckPoint )
CPPTOPYTHONWITHINT( depth )

/* _________________________________________________________________________ */
static PyObject* hep_setCheckPoint( PyObject* unused, PyObject* args ) {
  int res = hhh_setCheckPoint();
  return Py_BuildValue( "i", res );
}

/* _________________________________________________________________________ */
static PyObject* hep_configure( PyObject* unused, PyObject* args ) {
   long flags = 0;
   if ( ! PyArg_ParseTuple( args, (char*)"|l:configure", &flags ) )
      return 0;

   flags = configure( flags );

   return PyLong_FromLong( flags );
}

/* _________________________________________________________________________ */
#if PY_MAJOR_VERSION < 3
static PyObject* hep_outstream( PyObject* unused, PyObject* args ) {
   FILE *fp = 0;
   int fd = -1;
   PyObject* pyfile;

   if ( ! PyArg_ParseTuple( args, "O!", &PyFile_Type, &pyfile ) )
      return 0;

   fp = PyFile_AsFile( pyfile );
   if ( ! fp )
      return 0;

   if ( gReportStream && gReportStream != stdout )
      fclose( gReportStream );

   fd = dup( fileno( fp ) );
   if (fd < 0)
     return 0;

   gReportStream = fdopen( fd, "w" );

   Py_INCREF( Py_None );
   return Py_None;
}
#endif

/* _________________________________________________________________________ */
static PyObject* hep_profname( PyObject* unused, PyObject* args ) {
   char *cmd = 0, *filename = 0;
   const char *name = 0;

   if ( ! PyArg_ParseTuple( args, "s", &name ) )
      return 0;

   if ( gProfileStream )
      pclose( gProfileStream );

// remove file if it already exists
   filename = (char*)malloc( 5+strlen(name)+1 );
   sprintf( filename, "%s.prof", name );
   (void)remove( filename );
   free( filename );

// open a pipe to gzip onto the new file
   cmd = (char*)malloc( 16+strlen( name )+1 );
   sprintf( cmd, "gzip -f >> %s.prof", name );
   gProfileStream = popen( cmd, "w" );
   free( cmd );

   Py_INCREF( Py_None );
   return Py_None;
}

/* _________________________________________________________________________ */
static PyObject* hep_symbname( PyObject* unused, PyObject* args ) {
   const char* name = 0;
   if ( ! PyArg_ParseTuple( args, "s", &name ) )
      return 0;

   if ( gSymbolFileName )
      free( gSymbolFileName );

   gSymbolFileName = (char*)malloc( strlen( name )+1 );
   strcpy( gSymbolFileName, name );

   Py_INCREF( Py_None );
   return Py_None;
}

/* _________________________________________________________________________ */
static PyObject* hep_freestat_start() {
   gFlags |= FREESTAT;
   return PyLong_FromLong( gFlags );
}

/* _________________________________________________________________________ */
static PyObject* hep_freestat_stop() {
   gFlags &= gFlags ^ FREESTAT;
   return PyLong_FromLong( gFlags );
}

/* _________________________________________________________________________ */
static PyObject* hep_freestat_reset() {
   gFreeStatistics.total     = 0.;
   gFreeStatistics.untracked = 0;

   Py_INCREF( Py_None );
   return Py_None;
}

/* _________________________________________________________________________ */
static PyObject* hep_freestat_stat() {
   PyObject *stat = 0;

   stat = PyTuple_New( 2 );
   PyTuple_SET_ITEM( stat, 0, PyFloat_FromDouble( gFreeStatistics.total ) );
   PyTuple_SET_ITEM( stat, 1, PyLong_FromLong( gFreeStatistics.untracked ) );

   return stat;
}

/* _________________________________________________________________________ */
static PyMethodDef gMemoryTrackerMethods[] = {
   { (char*)"save",      (PyCFunction)hep_save,      METH_VARARGS, (char*)"save pre-existing hooks" },
   { (char*)"install",   (PyCFunction)hep_install,   METH_VARARGS, (char*)"install tracking" },
   { (char*)"start",     (PyCFunction)hep_start,     METH_VARARGS, (char*)"start tracking" },
   { (char*)"stop",      (PyCFunction)hep_stop,      METH_VARARGS, (char*)"stop tracking" },
   { (char*)"uninstall", (PyCFunction)hep_uninstall, METH_VARARGS, (char*)"uninstall tracking" },
   { (char*)"atexit",    (PyCFunction)hep_atexit,    METH_VARARGS, (char*)"atexit handler" },
   { (char*)"trace",     (PyCFunction)hep_trace,     METH_VARARGS, (char*)"trace given filter" },
   { (char*)"ignore",    (PyCFunction)hep_ignore,    METH_VARARGS, (char*)"ignore in report" },
   { (char*)"ignoreCall",(PyCFunction)hep_ignoreCall,METH_VARARGS, (char*)"ignore in report anywhere in trace" },
   { (char*)"configure", (PyCFunction)hep_configure, METH_VARARGS, (char*)"set configuration flags" },
#if PY_MAJOR_VERSION < 3
   { (char*)"outstream", (PyCFunction)hep_outstream, METH_VARARGS, (char*)"set new outstream" },
#endif
   { (char*)"depth",     (PyCFunction)hep_depth,     METH_VARARGS, (char*)"set large trace depth" },
   { (char*)"_profname", (PyCFunction)hep_profname,  METH_VARARGS, (char*)"set new profile file name" },
   { (char*)"_symbname", (PyCFunction)hep_symbname,  METH_VARARGS, (char*)"set new symbol file name" },

   { NULL, NULL, 0, NULL }
};

/* _________________________________________________________________________ */
static PyMethodDef gCheckPointsMethods[] = {
   { (char*)"setCheckPoint",   (PyCFunction)hep_setCheckPoint,   METH_VARARGS,
        (char*)"set a new check point, which will become current (used for tracking run-away caches)" },
   { (char*)"clearCheckPoint", (PyCFunction)hep_clearCheckPoint, METH_VARARGS,
        (char*)"consolidate the check point with the general memory tracking (used for tracking run-away caches)" },
   { (char*)"check",           (PyCFunction)hep_check,           METH_VARARGS,
        (char*)"print out statistics for the given check point (used for tracking run-away caches)" },

   { NULL, NULL, 0, NULL }
};

/* _________________________________________________________________________ */
static PyMethodDef gFreeStatisticsMethods[] = {
   { (char*)"start",  (PyCFunction)hep_freestat_start,  METH_NOARGS, (char*)"start collecting statistics" },
   { (char*)"stop",   (PyCFunction)hep_freestat_stop,   METH_NOARGS, (char*)"stop collecting statistics" },
   { (char*)"reset",  (PyCFunction)hep_freestat_reset,  METH_NOARGS, (char*)"reset statistics collection" },
   { (char*)"stat",   (PyCFunction)hep_freestat_stat,   METH_NOARGS, (char*)"get collected statistics" },

   { NULL, NULL, 0, NULL }
};

/* _________________________________________________________________________ */
PyObject* initMemoryTrace();
PyObject* initDoubleDeleteChecker();

#if PY_MAJOR_VERSION >= 3
PyObject *
PyInit_MemoryTracker(void)
#else
void initMemoryTracker()
#endif
{
   PyObject *memtrack;
   PyObject *chkpoints, *freestat;
   PyObject *ddcheck, *memtrace;

#if PY_MAJOR_VERSION >= 3
   static struct PyModuleDef moduledefMemoryTracker = {
     PyModuleDef_HEAD_INIT,
     "MemoryTracker",     /* m_name */
     "Memory tracker",  /* m_doc */
     -1,                  /* m_size */
     gMemoryTrackerMethods,    /* m_methods */
     NULL,                /* m_reload */
     NULL,                /* m_traverse */
     NULL,                /* m_clear */
     NULL,                /* m_free */
   };
   memtrack = PyModule_Create (&moduledefMemoryTracker);
#else   
   memtrack = Py_InitModule( (char*)"MemoryTracker", gMemoryTrackerMethods );
#endif

/* configuration flags */
   PyModule_AddObject( memtrack, (char*)"LEAK_CHECK", PyLong_FromLong( LEAK_CHECK ) );
   PyModule_AddObject( memtrack, (char*)"PROFILE",    PyLong_FromLong( PROFILE ) );
   PyModule_AddObject( memtrack, (char*)"HIDEMEMADDR",PyLong_FromLong( HIDEMEMADDR ) );
   PyModule_AddObject( memtrack, (char*)"QUICK",      PyLong_FromLong( QUICK ) );
   PyModule_AddObject( memtrack, (char*)"STL_CHECKS", PyLong_FromLong( STL_CHECKS ) );
   PyModule_AddObject( memtrack, (char*)"FILTER_STL", PyLong_FromLong( FILTER_STL ) );
   PyModule_AddObject( memtrack, (char*)"FREESTAT",   PyLong_FromLong( FREESTAT ) );

/* checkpoints functionality for event-by-event leak checking */
#if PY_MAJOR_VERSION >= 3
   static struct PyModuleDef moduledefCheckPoints = {
     PyModuleDef_HEAD_INIT,
     "CheckPoints",     /* m_name */
     "Check points",  /* m_doc */
     -1,                  /* m_size */
     gCheckPointsMethods,    /* m_methods */
     NULL,                /* m_reload */
     NULL,                /* m_traverse */
     NULL,                /* m_clear */
     NULL,                /* m_free */
   };
   chkpoints = PyModule_Create (&moduledefCheckPoints);
#else   
   chkpoints = Py_InitModule( (char*)"CheckPoints", gCheckPointsMethods );
#endif
   PyModule_AddObject( memtrack, (char*)"CheckPoints", chkpoints );

/* steering of colleciton of free() statistics */
#if PY_MAJOR_VERSION >= 3
   static struct PyModuleDef moduledefFreeStatistics = {
     PyModuleDef_HEAD_INIT,
     "FreeStatistics",     /* m_name */
     "Free statistics",  /* m_doc */
     -1,                  /* m_size */
     gFreeStatisticsMethods,    /* m_methods */
     NULL,                /* m_reload */
     NULL,                /* m_traverse */
     NULL,                /* m_clear */
     NULL,                /* m_free */
   };
   freestat = PyModule_Create (&moduledefFreeStatistics);
#else   
   freestat = Py_InitModule( (char*)"FreeStatistics", gFreeStatisticsMethods );
#endif
   PyModule_AddObject( memtrack, (char*)"FreeStatistics", freestat );

/* access to used size of memory traces */
   memtrace = initMemoryTrace();
   PyModule_AddObject( memtrack, (char*)"MemoryTrace", memtrace );

/* double delete checker functionality */
   ddcheck = initDoubleDeleteChecker();
   PyModule_AddObject( memtrack, (char*)"DeleteChecker", ddcheck );

#if PY_MAJOR_VERSION >= 3
   return memtrack;
#endif
}
