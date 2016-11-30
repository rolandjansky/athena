/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/DebugInfo.h"
#include "Hephaestus/HashTable.h"

#ifdef HEPHAESTUS_USE_BFD

#include <bfd.h>
#include <link.h>
#include <string.h>

/* Retrieving debug info follows addr2line.c from GNU binutils. */


/*- data ------------------------------------------------------------------- */
typedef struct {
   bfd_vma pc;

   asymbol **syms;
   const char *filename;
   const char *functionname;
   unsigned int line;
} hhh_DebugInfo;

static char *gFileLineBuf = 0;
static char *gSymbolBuf = 0;


/* _ helpers _______________________________________________________________ */
static int slurp_symtab( bfd *abfd, hhh_DebugInfo *dbi ) {
   long symcount;
   unsigned int size;

   if ( (bfd_get_file_flags( abfd ) & HAS_SYMS) == 0 )
      return 0;

   symcount = bfd_read_minisymbols( abfd, FALSE, (void*)&dbi->syms, &size );
   if ( symcount == 0 )   /* try dynamic instead */
      symcount = bfd_read_minisymbols( abfd, TRUE, (void*)&dbi->syms, &size );

   if ( symcount < 0 )
      return 0;

   return 1;
}


static int find_address_in_section(
      bfd *abfd, asection *section, hhh_DebugInfo* dbi ) {
   bfd_vma vma;
   bfd_size_type size;
   int found = 0;

/* debug info sections do not have flags set for allocating space on load */
   if ( (bfd_get_section_flags( abfd, section ) & SEC_ALLOC) == 0 )
      return found;

   vma = bfd_get_section_vma( abfd, section );
   if ( dbi->pc < vma )
      return found ;

   size = bfd_get_section_size( section );
   if ( dbi->pc >= vma + size )
      return found ;

   found = (int)bfd_find_nearest_line( abfd, section,
      dbi->syms, dbi->pc - vma, &dbi->filename, &dbi->functionname, &dbi->line );

   return found;
}

/* _________________________________________________________________________ */
const char* hhh_getDebugInfo(
      void *addr, Dl_info *info, struct link_map* lm, const char **symbol ) {
   static int first = 1;
   char *h;
   const char *result = 0;
   int found = 0;
   asection *p;
   bfd *abfd;
   bfd_vma base;
   hhh_DebugInfo dbi = { 0, NULL, NULL, NULL, 0 };

   if ( info && lm && lm->l_name ) {
      abfd = bfd_openr( lm->l_name, 0 );

      if ( ! abfd )
         return result;

   /* need to call this for some reason or another ... */
      if ( ! bfd_check_format_matches( abfd, bfd_object, 0 ) ) {
         bfd_close( abfd );
         return result;
      }

   /* read in a minitable of symbols */
      if ( slurp_symtab( abfd, &dbi ) ) {

      /* calculate the address within the relocated library */
         base = (bfd_vma)info->dli_fbase;
         dbi.pc = (bfd_vma)addr >= base ? (bfd_vma)addr - base : base - (bfd_vma)addr;

      /* loop over all sections, (hopefully) including the debug one, to find
         a close match to addr, results (if any) are stored in dbi */
         for ( p = abfd->sections; p != NULL; p = p->next ) {
            if ( (found = find_address_in_section( abfd, p, &dbi )) )
               break;
         }

         if ( found ) {

         /* store results, always filename if available */
            if ( dbi.filename && *dbi.filename != '\0' ) {
               snprintf( gFileLineBuf, hhh_SYMBOL_BUFLEN-1, "%s:%u", dbi.filename, dbi.line );
               result = gFileLineBuf;
            }

         /* function name (local symbol) is requested under some circumstances */
            if ( symbol && dbi.functionname && *dbi.functionname != '\0' ) {
               snprintf( gSymbolBuf, hhh_SYMBOL_BUFLEN-1, "%s", dbi.functionname );
               *symbol = gSymbolBuf;
            }

         /* further drill down to locate line number */
         /* do {
               if ( dbi.filename ) {
                  h = strrchr( dbi.filename, '/' );
                  if ( h != NULL )
                     dbi.filename = h + 1;
               }
      
               found = bfd_find_inliner_info( abfd, &dbi.filename, &dbi.functionname, &dbi.line );
            } while ( found ); */

         }

      /* cleanup symbols ... should cache? */
         free( dbi.syms );
      }
   }

/* cleanup bfd ... should cache? */
   bfd_close( abfd );

   return result;        /* returns 0 on failure */
}


/*========================================================================== */
/* startup and shutdown */
/*========================================================================== */
static void setup() __attribute__ ((constructor));
static void cleanup() __attribute__ ((destructor));

/* _________________________________________________________________________ */
static void setup() {
   static int initialized = 0;

   if ( ! initialized ) {
      initialized = 1;

      bfd_init();
#if defined ( __i386 )
      bfd_set_default_target( "elf32-i386" );
#endif

      gFileLineBuf = (char*)malloc( hhh_SYMBOL_BUFLEN );
      gSymbolBuf   = (char*)malloc( hhh_SYMBOL_BUFLEN );
   }
}

/* _________________________________________________________________________ */
static void cleanup() {
   static int finalized = 0;

   if ( ! finalized ) {
      finalized = 1;

      free( gSymbolBuf );
      free( gFileLineBuf );
   }
}

#else /* ! HEPHAESTUS_USE_BFD */

/* _________________________________________________________________________ */
const char* hhh_getDebugInfo(
      void *addr, Dl_info *info, struct link_map* lm, const char **symbol ) {
   return "";
}

#endif /* HEPHAESTUS_USE_BFD */
