///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MemStatsHooks.h 
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_PERFMONMEMSTATSHOOKS_H 
#define PERFMONEVENT_PERFMONMEMSTATSHOOKS_H 1

#include <stddef.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <malloc.h>
#endif

/** @brief a simple @c malloc wrapper that keeps track of the amount of memory 
 *         allocated on the heap.
 */

// forward declarations

namespace PerfMon {

/** @brief placeholder for the stats
 */
struct MemStats
{
  /// flag disabling or enabling the global malloc hooks
  static bool m_enabled;

  /// number of bytes allocated so far
  static unsigned long long m_nbytes;

  /// number of times malloc has been called so far
  static unsigned long long m_nmallocs;

  /// number of times free has been called so far
  static unsigned long long m_nfrees;

  /// switch to enable or disable the global malloc hooks
  /// @returns the old value of the flag
  static bool enable(const bool flag)
  {
    bool old_value = m_enabled;
    m_enabled = flag;
    return old_value;
  }

  /// install our hooks
  static void installHooks();

  /// uninstall our hooks
  static void uninstallHooks();

  /// save current hooks
  static void saveHooks();

  /// return the current flag value
  static bool enabled() { return m_enabled; }
  /// return the number of bytes allocated so far
  static unsigned long long nbytes() { return m_nbytes; }
  /// return the number of times malloc has been called so far
  static unsigned long long nmallocs() { return m_nmallocs; }
  /// return the number of times free has been called so far
  static unsigned long long nfrees() { return m_nfrees; }
  /// reset the number of so far allocated bytes to 0
  static void reset() 
  { 
    m_nbytes =  0;
    m_nmallocs= 0;
    m_nfrees =  0;
  }

  /// initialize library
  static void start();
  /// finalize library
  static void stop();
};

} //> namespace PerfMon

#endif// PERFMONEVENT_PERFMONMEMSTATSHOOKS_H 
