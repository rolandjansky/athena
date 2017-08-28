/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/src/exctrace.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Generate stack trace backs from a caught exception.
 *
 * The backtrace for the last exception is saved as static data
 * in exctrace_last_depth and exctrace_last_trace, which are defined
 * in the collector module.  We use dlsym to find them, so that we can
 * fail gracefully if the collector isn't present.
 */


#include "CxxUtils/exctrace.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>
#include <dlfcn.h>


using std::strlen;
using std::sprintf;
using std::free;


// Helper to write literals
# define MYWRITELIT(fd,str) MYWRITE(fd,str,sizeof(str)-1)


namespace CxxUtils {


/**
 * @brief Print out information for the last exception.
 *
 * Prints the supplied exception, plus the backtrace from
 * the last exception, if available.
 *
 * @param e The exception to print.
 * @param fd The file descriptor to which to write.
 */
void exctrace ATLAS_NOT_THREAD_SAFE (const std::exception& e, IOFD fd /*= IOFD_INVALID*/)
{
  if (fd == IOFD_INVALID)
    fd = Athena::DebugAids::stacktraceFd();

  static bool init = false;
  static int* exctrace_last_depth = 0;
  static void** exctrace_last_trace = 0;

  if (!init) {
    init = true;
    exctrace_last_depth = (int*)dlsym (RTLD_DEFAULT, "exctrace_last_depth");
    exctrace_last_trace = (void**)dlsym (RTLD_DEFAULT, "exctrace_last_trace");
  }

  MYWRITELIT(fd, "Exception: ");
  MYWRITE(fd, e.what(), strlen (e.what()));

  if (exctrace_last_depth && exctrace_last_trace) {
    MYWRITELIT(fd, "\n");
    // Index 0 is __cxa_throw.  Skip it.
    for (int i = 1; i < *exctrace_last_depth; ++i) {
      unsigned long ip =
        reinterpret_cast<unsigned long> (exctrace_last_trace[i]);
      // A function that throws may have the call to __cxa_throw
      // as the last instruction in the function.  In that case, the IP
      // we see here will be one beyond the end of the function,
      // and we'll report the wrong function.  So move back the IP
      // slightly for the function that threw.
      if (i == 1) --ip;
      Athena::DebugAids::stacktraceLine (fd, ip);
    }
  }
  else
    MYWRITELIT(fd, " (no backtrace available).\n");
}


} // namespace CxxUtils
