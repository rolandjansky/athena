#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <execinfo.h>

#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "bfd.h"
#include <unistd.h>
// for demangling
#include <cxxabi.h>
// for following code in shared libraries
#include <link.h>

#include <malloc.h>
#include <mcheck.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <string>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void resolve(void *address, const char* library)
{
  bfd *ibfd(0);
  asymbol **symtab(0);
  long nsize;
  char **matching(0);
  asection *text(0);
  
  bfd_init();
  ibfd = bfd_openr(library, NULL);
  
  if (ibfd == NULL)
    {
      fprintf(stderr,"bfd_openr error\n");
      exit(1);
    }
  
  if (!bfd_check_format_matches(ibfd, bfd_object, &matching))
    {
      fprintf(stderr,"format_matches\n");
      exit(1);
    }
  
  nsize = bfd_get_symtab_upper_bound (ibfd);
  symtab = (asymbol **)malloc(nsize);
  /*nsyms =*/ bfd_canonicalize_symtab(ibfd, symtab);
  
  text = bfd_get_section_by_name(ibfd, ".text");
  
  long offset(0);
  if(text)
    offset = ((long)address) - text->vma;
  
  if (strstr (library, ".so") != 0)
    {
      unsigned long addr = (unsigned long)address;
      if (text)
	offset = addr - text->vma;
    }
  
  // printf("offset : %lx %lx %lx %ld\n",offset,text,text ? text->vma : 0, nsize);
  
  if (offset > 0)
    {
      const char *file;
      const char *func;
      unsigned line;
      
      bool first=true;
      char   *realname(0);
      int     status;
      
      bool found = bfd_find_nearest_line (ibfd, text, symtab, offset, &file, &func, &line);
      
      // printf("found : %d %p %s\n",found,file, file ? file : "none" );
      
      if ( found && file)
	{
	  do
	    {
	      // from http://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
	      if(func)
		realname = abi::__cxa_demangle(func, 0, 0, &status);
	      if ( realname )
		{
		  printf(" %s : %s (%s,%u)\n",first ? "in function" : "  included from", realname, file, line);
		}
	      else
		{
		  printf(" %s : %s (%s,%u)\n", first ? "in function" : "  included from", func ? func : "??:0", file, line);
		}
	      free(realname);
              realname = 0;
	      
	      found = bfd_find_inliner_info (ibfd, &file, &func, &line);
	      first=false;
	    }
	  while(found);
	}
    }
  printf(" in library : %s\n",library);
  bfd_close(ibfd);
}

void usage()
{
  printf("usage: resolveAtlasAddr2Line -f sharedLibrary -a relative_address\n");
  printf("       typical usage would be from stacktraces given in logfiles, where the\n");
  printf("       job was run on a release without debug symbols. The information is still\n");
  printf("       enough to deduce line numbers if the same release is still available\n");
  printf("       e.g. in the builds area where the debug symbols are installed.\n");
  printf("       Normally used only from within 'atlasAddress2Line --file <logfile>'\n");
}

int
main (int argc, char **argv)
{
  char *avalue = NULL;
  char *fvalue = NULL;
  int index;
  int c;
     
  opterr = 0;
     
  while ((c = getopt (argc, argv, "a:f:h")) != -1)
    switch (c)
      {
      case 'h':
	usage();
	exit(0);
	break;
      case 'f':
	fvalue = optarg;
	break;
      case 'a':
	avalue = optarg;
	break;
      case '?':
	if (optopt == 'a')
	  fprintf (stderr, "Option -a requires an argument.\n");
	else if (optopt == 'f')
	  fprintf (stderr, "Option -f requires an argument.\n");
	else if (isprint (optopt))
	  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	else
	  fprintf (stderr,
		   "Unknown option character `\\x%x'.\n",
		   optopt);
	return 1;
      default:
	abort ();
      }
     
  // printf ("avalue = %s, fvalue = %s\n", avalue, fvalue);
     
  for (index = optind; index < argc; index++)
    {
      printf ("Non-option argument %s\n", argv[index]);
      return 1;
    }
  if ( bool(avalue) && bool(fvalue) )
    {
      unsigned long l(0);
      sscanf(avalue,"%80lx",&l);
      resolve((void*)l,fvalue); 
    }
  
  return 0;
}
