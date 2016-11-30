/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/Utils.h"
#include "Hephaestus/DebugInfo.h"
#include "Hephaestus/HashTable.h"

#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <string.h>

#ifdef HEPHAESTUS_USE_BFD
#define do_debug 1
#else
#define do_debug 0
#endif


/*- data ------------------------------------------------------------------- */
typedef char* (*Demangler_t) ( const char *__mangled_name, char *__output_buffer,
                               __SIZE_TYPE__ *__length, int *__status );
static Demangler_t gDemangler = 0;
static void *gCpplibHandle = 0;

static struct hhh_HashTable *gDemangleCache = 0;
static struct hhh_HashTable *gLinkCache     = 0;
static struct hhh_HashTable *gCtorCache     = 0;

static char *gDemangleBuf = 0;
static __SIZE_TYPE__ gDemangleBuflen = hhh_SYMBOL_BUFLEN;
static const char* unknown = "<unknown>";

/* _________________________________________________________________________ */
static const char* demangle( const char* s ) {
   int status = 0;
   char* result;

   if ( ! gDemangler )
      return s;

   result = gDemangler( s, gDemangleBuf, &gDemangleBuflen, &status );
   if ( ! result || status != 0 )
      return s;     /* failure usually b/c of C-linkage (i.e. no mangling) */

   if ( result != gDemangleBuf )
      gDemangleBuf = result;       /* realloc moved ptr */

   return gDemangleBuf;
}

/* _________________________________________________________________________ */
static struct link_map* getLinkMap( void *addr ) {
   struct link_map *lm;
   Dl_info info;

   lm = (struct link_map*)hhh_HashTable_find( gLinkCache, addr );
   if ( ! lm ) {
      if ( dladdr1( addr, &info, (void**)&lm, RTLD_DL_LINKMAP ) )
         hhh_HashTable_insert( gLinkCache, addr, lm );
      else
         lm = 0;
   }

   return lm;
}

/* _________________________________________________________________________ */
const char* hhh_getSymbol( void *addr ) {
   const char *cname;
   cname = hhh_getSymbolE( addr );
   if ( ! cname )
      return unknown;
   return cname;
}

/* _________________________________________________________________________ */
const char* hhh_getSymbolE( void *addr ) {
   char *value, *cname;
   const char *symbol, *name, *dbg = 0;
   int freesym = 0;
   Dl_info info;
   struct link_map *lm;
   size_t nlen = 0;

   value = (char*)hhh_HashTable_find( gDemangleCache, addr );
   if ( value )
      return value;

   if ( ! dladdr1( addr, &info, (void**)&lm, RTLD_DL_LINKMAP ) )
      return (const char*)NULL;

   /* help getLinkMap() fill its cache */
   /* hhh_HashTable_insert( gLinkCache, addr, lm ); */
 
   symbol = (info.dli_sname && info.dli_sname[0] != '\0') ? info.dli_sname : 0;

   if ( do_debug && lm && lm->l_name )
      dbg = hhh_getDebugInfo( addr, &info, lm, symbol == 0 ? &symbol : 0 );

   if ( symbol ) {
      name = demangle( symbol );
      nlen = strlen( name );

      if ( dbg ) {
         cname = (char*)malloc( nlen + strlen( dbg ) + 3 );
         strcpy( cname, name );
         strcpy( cname + nlen, " @" );
         strcpy( cname + nlen + 2, dbg );
      } else {
         cname = (char*)malloc( nlen + 1 );
         strcpy( cname, name );
      }

      hhh_HashTable_insert( gDemangleCache, addr, cname );
      return cname;
   }

/* one could return dli_fname, but it is mostly /lib/ld-linux.so, which realloc()s
   linker tables from time to time (is now filtered) */
   return (const char*)NULL;
}

/* _________________________________________________________________________ */
int hhh_writeSymbols( FILE *out ) {
   unsigned long i, key;
   char endl = '\n';
   struct hhh_Cell *cell;

   for ( i = 0; i < gDemangleCache->size; i++ ) {
      cell = gDemangleCache->table[i];
      while ( cell != NULL ) {
         key = (unsigned long)cell->key;
         fwrite( &key, sizeof( unsigned long ), 1, out );
         fwrite( cell->value, strlen( (const char*)cell->value ), 1, out );
         fwrite( &endl, 1, 1, out );
         cell = cell->next;
      }
   }

   return 0;
}

/* _________________________________________________________________________ */
void* hhh_getConstructor( void *addr ) {
#if defined ( __i386 )
   void *ctor;
   struct link_map *lm;
   unsigned long symtab, got, address, off, off2;
   char *caddr;
   ElfW(Dyn) *dyn;
   size_t jj;

   ctor = hhh_HashTable_find( gCtorCache, addr );
   if ( ctor )
      return ctor;

   lm = getLinkMap( addr );
   if ( ! lm )
      return 0;

   for ( dyn = lm->l_ld; dyn->d_tag != DT_NULL; ++dyn ) {
      if ( dyn->d_tag == DT_PLTGOT )
         got = (unsigned long)dyn->d_un.d_val;
      else if ( dyn->d_tag == DT_SYMTAB )
         symtab = (unsigned long)dyn->d_un.d_val;
   }

   ctor = 0; caddr = (char*)addr;
   for ( jj = sizeof(void*); jj < 2*sizeof(void*); ++jj ) {
      if ( *(caddr+jj) == '\xe8' ) {
         off = 0;
         memcpy( &off, caddr+jj+1, sizeof(unsigned long) );
         address = (unsigned long)caddr+jj+off+0x5;

      /* sanity check: text section in between symbol and offset tables */
         if ( ! ( symtab < address && address < got ) )
            continue;

         if ( ((char*)address)[0] == '\xff' ) {
         /* indirection through linkage and offset tables required; note that
            if the function has not been called yet, this will not work */
            off2 = 0;
            memcpy( &off2, (char*)address+2, sizeof(void*) );
            ctor = (void*)*(ElfW(Addr)*)(got+off2);
         }
         else {
         /* no indirection, store jump address */
            ctor = (void*)address;
         }

         break;
      }
   }

   if ( ctor != 0 && hhh_getSymbol( ctor ) != unknown ) {
      hhh_HashTable_insert( gCtorCache, addr, ctor );
      return ctor;
   }
#endif

   return 0;
}

/* _________________________________________________________________________ */
int hhh_setLinkLate( void *addr ) {
   struct link_map *lm, *lm2;

/* close handle to cpplib if opened, to allow ourselves to be put after; this
   does mean that everything happening after this call will not be demangled;
   most (all?) relevant addresses should be in, though. */
   if ( gDemangler && gCpplibHandle ) {
      gDemangler = 0;
      dlclose( gCpplibHandle );
      gCpplibHandle = 0;
   }

/* get handle to lib to be put late (Hephaestus self, really) */
   lm = getLinkMap( addr );
   if ( ! lm )
      return 0;

   lm2 = lm;
   while ( lm2->l_next ) lm2 = lm2->l_next;

/* nothing to do if already at end of list */
   if ( lm == lm2 )
      return 1;

/* move the current link_map to the end */
   lm->l_prev->l_next = lm->l_next;
   lm->l_next->l_prev = lm->l_prev;

   lm2->l_next = lm;
   lm->l_prev = lm2;
   lm->l_next = 0;

   return 1;
}


/*========================================================================== */
/* addrtoline */
/*========================================================================== */

/******************************************************************
 * address -> line translation.
 * This works the same way as in SealDebug in CxxUtils; duplicated here
 * to avoid adding dependencies.
 */


#define HHH_LINE_MAX 1024


static
int addr_readline (FILE* f, char* buf, int buflen)
{
  int len = 0;
  while (len < buflen-1) {
    int c = fgetc(f);
    if (c < 0 || c == '\n') break;
    *buf++ = c;
    ++len;
  }
  *buf = '\0';
  return len;
}


const char* hhh_addrToLine (void* addr_in, const char** symbol)
{
  unsigned long addr = (unsigned long) addr_in;
  unsigned long libaddr;
  unsigned long relative_address;
  size_t len;
  Dl_info info;
  if (dladdr (addr_in, &info) == 0 ||
      info.dli_fname == 0 ||
      info.dli_fname[0] == 0)
    return "";

  /* difference of two pointers */
  libaddr = (unsigned long) info.dli_fbase;
  relative_address = (addr >= libaddr) ? addr - libaddr : libaddr - addr;
  if (strstr (info.dli_fname, ".so") == 0)
    relative_address = addr;
  relative_address -= 1;

  /* did we find a valid entry? */
  len = strlen (info.dli_fname);
  if (len > 0 && len + 80 < HHH_LINE_MAX) {
    FILE* pf;
    static char line[HHH_LINE_MAX];

    if (getenv ("LD_PRELOAD"))
      unsetenv ("LD_PRELOAD");

    if (access ("/usr/bin/eu-addr2line", F_OK) == 0)
    {
      snprintf (line, HHH_LINE_MAX, "/usr/bin/eu-addr2line -f -e %s %p | /usr/bin/c++filt ",
                info.dli_fname,
                (void*)relative_address); 
    }
    else
    {
      snprintf (line, HHH_LINE_MAX, "/usr/bin/addr2line -f -C -e %s %p",
                info.dli_fname,
                (void*)relative_address);
    }

    pf = popen (line, "r");
    if (pf) {
      int length = 1;
      static char dembuf[ HHH_LINE_MAX ];
      size_t demlen = addr_readline (pf, dembuf, sizeof(dembuf));
      if (symbol)
        *symbol = dembuf;
      line[0] = ' ';
      length = addr_readline (pf, line+1, sizeof(line)-1);
      if (length >= 0) ++length;
      pclose (pf);
      if (length < 0 || line[1] == '?') {
        line[1] = '\0';
        length = 0;
      }

      line[length] = '\0';
      return line;
    }
  }
  return "";
}



/*========================================================================== */
/* startup and shutdown */
/*========================================================================== */
static void setup() __attribute__ ((constructor));

/* _________________________________________________________________________ */
static void hhh_utils_setup() {
   static int initialized = 0;
   int i;
   const char *libs[] = { "libstdc++.so", "libstdc++.so.6", "libstdc++.so.5" };
   const int nlibs = sizeof(libs)/sizeof(libs[0]);

   if ( ! initialized ) {
      initialized = 1;

      gDemangleCache = hhh_HashTable_new( hhh_HASHTABLE_MINSIZE );
      gLinkCache     = hhh_HashTable_new( hhh_HASHTABLE_MINSIZE );
      gCtorCache     = hhh_HashTable_new( hhh_HASHTABLE_MINSIZE );

      for ( i = 0; i < nlibs; ++i ) {
         gCpplibHandle = dlopen( libs[i], RTLD_LAZY | RTLD_LOCAL );
         if ( gCpplibHandle )
            break;
      }

      if ( gCpplibHandle )
         gDemangler = (Demangler_t)dlsym( gCpplibHandle, "__cxa_demangle" );
      else
         printf( "Hephaestus warning: unable to locate demangler; filter will not work!\n" );

      gDemangleBuf = (char*)malloc( hhh_SYMBOL_BUFLEN );
   }
}

static void setup() {
   hhh_utils_setup();
}

/* _________________________________________________________________________ */
void hhh_utils_cleanup() {
   static int finalized = 0;

   if ( ! finalized ) {
      finalized = 1;

      if ( gDemangler && gCpplibHandle ) {
         gDemangler = 0;
         dlclose( gCpplibHandle );
         gCpplibHandle = 0;
      }

      free( gDemangleBuf );

      hhh_HashTable_delete( gCtorCache,         0 ); gCtorCache = 0;
      hhh_HashTable_delete( gLinkCache,         0 ); gLinkCache = 0;
      hhh_HashTable_delete( gDemangleCache, &free ); gDemangleCache = 0;
   }
}
