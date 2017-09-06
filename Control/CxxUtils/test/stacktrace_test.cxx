/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/test/stacktrace_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test stack trace printing.
 */


#undef NDEBUG

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "CxxUtils/SealDebug.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifndef __APPLE__
#   include <malloc.h>
#endif // not __APPLE__
#include <dlfcn.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>


char* snip (char* buf, char* p, char fill = '\0')
{
  if (p > buf) {
    if (fill)
      *buf++ = fill;
    char* q = buf;
    while (*p)
      *q++ = *p++;
    *q = 0;
  }
  return buf;
}


void filter (char* buf)
{
  char* sl = 0;
  while (*buf) {
    if (buf[0] == '0' && buf[1] == 'x') {
      buf += 2;
      char* p = buf;
      while (isxdigit (*p))
        ++p;
      buf = snip (buf, p, 'X');
    }

    else if (buf[0] == '/') {
      if (sl)
        buf = snip (sl, buf);
      sl = buf;
      ++buf;
    }

    else if (buf[0] == ' ') {
      ++buf;
      sl = 0;
    }

    else if (buf[0] == '.' && buf[1] == '.') {
      buf = snip (buf, buf+2);
    }

    // Get rid of file/line number.
    // Unfortunate, but we don't get these for opt builds.
    else if (buf[0] == ':' && sl) {
      ++buf;
      while (isdigit (*buf))
        ++buf;
      if (*buf == ' ')
        ++buf;
      buf = snip (sl, buf);
      sl = 0;
    }

    else
      ++buf;
  }
}


void dumptrace (FILE* fp)
{
  fseek (fp, 0, SEEK_SET);
  char buf[65536];
  while (fgets (buf, sizeof (buf), fp)) {
    if (strstr (buf, "libasan") != nullptr)
      continue;
    filter (buf);
    fputs (buf, stdout);
  }
}


bool initialized = false;
bool initializing = false;
bool finishing = false;
bool nomalloc = false;

void *(*old_malloc)(size_t size);
void  (*old_free)(void* ptr);
void *(*old_realloc)(void* ptr, size_t size);

void initpointers()
{
  initializing = true;
  old_malloc = (void* (*)(size_t)) (unsigned long) dlsym(RTLD_NEXT, "__libc_malloc");
  old_free = (void (*)(void*)) (unsigned long) dlsym(RTLD_NEXT, "__libc_free");
  old_realloc = (void* (*)(void*, size_t)) (unsigned long) dlsym(RTLD_NEXT, "__libc_realloc");
  initialized = true;
  initializing = false;
}

void* do_malloc (size_t size)
{
  /* It's possible to get recursion here, since dlsym() can trigger
   * memory allocation. To deal with this, we flag the initialization
   * condition specially, then use the special knowledge that it's
   * OK for malloc to fail during initialization (libc degrades
   * gracefully), so we just return NULL from malloc(), realloc().
   *
   * This trick is borrowed from from libc's memusage.
   */
  if (!initialized) {
    if (initializing)
      return NULL;
    initpointers();
  }

  if (nomalloc) {
    printf ("malloc called\n");
    std::abort();
  }
  return (*old_malloc) (size);
}

void* malloc (size_t size)
#ifndef __APPLE__
throw()
#endif
{
  return do_malloc (size);
}


void* __libc_malloc (size_t size)
{
  return do_malloc (size);
}


void do_free (void* ptr)
{
  if (!initialized) {
    if (initializing)
      return;
    initpointers();
  }
  if (finishing)
    return;

  (*old_free) (ptr);
}


void free (void* ptr)
#ifndef __APPLE__
throw()
#endif
{
  return do_free (ptr);
}


void __libc_free (void* ptr)
{
  return do_free (ptr);
}


void* do_realloc (void* ptr, size_t size)
{
  if (!initialized) {
    if (initializing)
      return NULL;
    initpointers();
  }

  if (nomalloc) {
    printf ("realloc called\n");
    std::abort();
  }
  return (*old_realloc) (ptr, size);
}


void* realloc (void* ptr, size_t size)
#ifndef __APPLE__
throw()
#endif
{
  return do_realloc (ptr, size);
}


void* __libc_realloc (void* ptr, size_t size)
{
  return do_realloc (ptr, size);
}



// Used to check that we don't call malloc during the stack trace.
void sethooks()
{
  nomalloc = true;
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__ ((noinline))
#endif
void resethooks() 
{
  nomalloc = false;
}


#if defined(__GNUC__) || defined(__clang__)
__attribute__ ((noinline))
#endif
void fromhere() 
{
  char pat[] = "stacktrace_testXXXXXX";
  umask (0600);
  int fd = mkstemp (pat);
  if (fd < 0) std::abort();
  FILE* fp = fdopen (fd, "w+");
  sethooks();
  Athena::DebugAids::stacktrace (fd);
  resethooks();
  dumptrace (fp);
  fclose (fp);
}


int main()
{
  initpointers();
  Athena::DebugAids::setStackTraceAddr2Line ("/usr/bin/addr2line");
  fromhere();
  finishing = true;
  return 0;
}
