/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/src/exctrace/exctrace_collector.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Generate stack trace backs from a caught exception ---
 *        collector module.
 *
 * When preloaded, this module hooks into the __cxa_throw function
 * used by the abi to throw exceptions so that it records stack
 * back traces in static variables.  These can later be accessed
 * with @c CxxUtils::exctrace.
 */


#include <dlfcn.h>
#include <execinfo.h>
#include <cstdio>
#include <typeinfo>
#include <utility>
#include <algorithm>
#include "CxxUtils/checker_macros.h"

// Maximum stack depth.
static
const int bt_depth = 100;

// Static buffer used to save the backtrace.
static thread_local int exctrace_last_depth = 0;
static thread_local void* exctrace_last_trace[bt_depth];

// The real __cxa_throw function.
typedef void throwfn (void*, void*, void (*dest)(void*));
static throwfn* old_throw;

extern "C" {
  // Function to retrieve the last trace.
  // extern "C" because we want to find it with dlsym.
  int exctrace_get_last_trace (int max_depth, void* trace[])
  {
    int ncopy = std::min (exctrace_last_depth, max_depth);
    std::copy (exctrace_last_trace, exctrace_last_trace+ncopy, trace);
    return ncopy;
  }
}


// The __cxa_throw hook function.
// Record a backtrace, then chain to the real throw function.
extern "C" void __cxa_throw (void* thrown_exception,
                             void* tinfo,
                             void (*dest)(void*))
{
  exctrace_last_depth = backtrace (exctrace_last_trace, bt_depth);
  old_throw (thrown_exception, tinfo, dest);
  // not reached
  std::abort();
}


// Initialization: install the hook.
namespace CxxUtils {


struct ATLAS_NOT_THREAD_SAFE extrace_init
{
  extrace_init();
};


extrace_init::extrace_init()
{
  old_throw = (throwfn*)(long)dlsym (RTLD_NEXT, "__cxa_throw");
}


static extrace_init initer;


} // namespace CxxUtils
