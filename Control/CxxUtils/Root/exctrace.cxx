/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "CxxUtils/checker_macros.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <execinfo.h>
#include <unistd.h>
#include <dlfcn.h>


using std::strlen;
using std::sprintf;
using std::free;


// Helper to write literals
# define MYWRITELIT(fd,str) MYWRITE(fd,str,sizeof(str)-1)


namespace {
bool stacktraceLine ATLAS_NOT_THREAD_SAFE (IOFD fd, unsigned long addr)
{
  Athena::DebugAids::stacktraceLine (fd, addr);
  return false;
}
}


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
void exctrace (const std::exception& e, IOFD fd /*= IOFD_INVALID*/)
{
  if (fd == IOFD_INVALID)
    fd = Athena::DebugAids::stacktraceFd();

  typedef int (*get_last_trace_fn) (int max_depth, void* trace[]);
  get_last_trace_fn get_last_trace = (get_last_trace_fn) dlsym (RTLD_DEFAULT, "exctrace_get_last_trace");
  

  MYWRITELIT(fd, "Exception: ");
  MYWRITE(fd, e.what(), strlen (e.what()));

  if (get_last_trace) {
    void* trace[100];
    int depth = get_last_trace (std::end(trace)-std::begin(trace), trace);

    MYWRITELIT(fd, "\n");
    // Index 0 is __cxa_throw.  Skip it.
    for (int i = 1; i < depth; ++i) {
      unsigned long ip =
        reinterpret_cast<unsigned long> (trace[i]);
      // A function that throws may have the call to __cxa_throw
      // as the last instruction in the function.  In that case, the IP
      // we see here will be one beyond the end of the function,
      // and we'll report the wrong function.  So move back the IP
      // slightly for the function that threw.
      if (i == 1) --ip;

      // It's true that stacktraceLine is not really thread-safe.
      // However, if we're here, things are going south fast anyway,
      // so we'll just cross our fingers and try to shovel out as much
      // information as we can.
      [[maybe_unused]]
      bool dum ATLAS_THREAD_SAFE = stacktraceLine (fd, ip);
    }
  }
  else
    MYWRITELIT(fd, " (no backtrace available).\n");
}


} // namespace CxxUtils
