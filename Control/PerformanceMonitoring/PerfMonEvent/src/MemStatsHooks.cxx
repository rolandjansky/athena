///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MemStatsHooks.cxx 
// Implementation file for PerfMon memory snooping
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// WARNING: This specifically disables warnings in 21.2 that we do not
// intend to fix anymore.  It should not be swept to any other
// branches.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// PerfMonEvent includes
#include "PerfMonEvent/MemStatsHooks.h"

// std includes
#include <pthread.h>
#include <stdexcept>

// mutex for the hooks
static pthread_mutex_t pmon_mem_lock = PTHREAD_MUTEX_INITIALIZER; 

// holders for original hooks
static void* (*orig_malloc) (size_t size, const void* caller);
static void* (*orig_realloc)(void* ptr, size_t size, const void* caller);
static void  (*orig_free)   (void* ptr, const void* caller);

// perfmon memory monitoring hooks
static void* pmon_mem_malloc (size_t size, const void* caller);
static void* pmon_mem_realloc(void* ptr, size_t size, const void* caller);
static void  pmon_mem_free   (void* ptr, const void* caller);

////////////////////////////////////////////////////////////////////////////////
bool PerfMon::MemStats::m_enabled = false;
unsigned long long PerfMon::MemStats::m_nbytes   = 0;
unsigned long long PerfMon::MemStats::m_nmallocs = 0;
unsigned long long PerfMon::MemStats::m_nfrees   = 0;

#include <limits>
#include <iostream>

#ifdef __APPLE__

// Dummy code for time being
void PerfMon::MemStats::start(){}
void PerfMon::MemStats::stop(){}
void PerfMon::MemStats::installHooks(){}
void PerfMon::MemStats::uninstallHooks(){}
void PerfMon::MemStats::saveHooks(){}
static void* pmon_mem_malloc(size_t size, const void* /*caller*/)
{ return malloc(size); }
static void* pmon_mem_realloc(void* ptr, size_t size, const void* /*caller*/)
{ return realloc(ptr, size); }
static void  pmon_mem_free( void* ptr, const void* /*caller*/ )
{ free(ptr); }

#else /* __linux__ */


void PerfMon::MemStats::start()
{
  if (0 != pthread_mutex_init(&pmon_mem_lock, NULL)) {
    throw std::runtime_error("perfmon: could not initialize pthread mutex");
  }
  pthread_mutex_lock(&pmon_mem_lock);
  saveHooks();
  if (PerfMon::MemStats::m_enabled) {
    installHooks();
  } else {
    // installHooks unlocks the mutex...
    pthread_mutex_unlock(&pmon_mem_lock);
  }
}
 
void PerfMon::MemStats::stop()
{
  // we actually don't want to ever un-install the hooks as
  // this might break daisy chains of hooks: if our hooks were installed first
  // uninstalling them will put the NULL ones back in, and won't give a chance
  // for later-registered hooks to do their properly afterwards.

  // if (PerfMon::MemStats::m_enabled) {
  //   uninstallHooks();
  // }
  if (0 != pthread_mutex_destroy(&pmon_mem_lock)) {
    throw std::runtime_error("perfmon: could not destroy pthread mutex");
  }
}

void PerfMon::MemStats::installHooks()
{
  __free_hook    = pmon_mem_free;
  __realloc_hook = pmon_mem_realloc;
  __malloc_hook  = pmon_mem_malloc;
  pthread_mutex_unlock(&pmon_mem_lock);
}

void PerfMon::MemStats::uninstallHooks()
{
  pthread_mutex_lock(&pmon_mem_lock);
  __free_hook    = orig_free;
  __realloc_hook = orig_realloc;
  __malloc_hook  = orig_malloc;
}

void PerfMon::MemStats::saveHooks()
{
  // store old hooks in buffer for reset later
  // prevent storing ourselves in case of a double-call by user
  if ( __malloc_hook != pmon_mem_malloc ) {
    orig_malloc  = __malloc_hook;
  }

  if ( __realloc_hook != pmon_mem_realloc ) {
    orig_realloc = __realloc_hook;
  }

  if ( __free_hook != pmon_mem_free ) { 
    orig_free = __free_hook;
  }

  return;
}


static void* pmon_mem_malloc(size_t size, const void* /*caller*/)
{
  void *result;

  // reset (system/NULL) hook to prevent recursion
  PerfMon::MemStats::uninstallHooks();

  // let malloc do its thing
  result = malloc( size );
  
  // update MemStats state
  PerfMon::MemStats::m_nbytes   += size;
  PerfMon::MemStats::m_nmallocs += 1;

  // store current hooks
  PerfMon::MemStats::saveHooks();

  // reset hooks to our ones
  PerfMon::MemStats::installHooks();

  return result;
}

static void* pmon_mem_realloc(void* ptr, size_t size, const void* /*caller*/)
{
  void *result;

  // reset (system/NULL) hook to prevent recursion
  PerfMon::MemStats::uninstallHooks();

  // let realloc do its thing
  result = realloc( ptr, size );

  // update MemStats state
  PerfMon::MemStats::m_nbytes   += size;
  PerfMon::MemStats::m_nmallocs += 1;

  // store current hooks
  PerfMon::MemStats::saveHooks();

  // reset our hooks
  PerfMon::MemStats::installHooks();

  return result;
}

static void  pmon_mem_free( void* ptr, const void* /*caller*/ )
{
  // reset (system/NULL) hook to prevent recursion
  PerfMon::MemStats::uninstallHooks();

  // let free do its thing
  free( ptr );

  // update counter
  PerfMon::MemStats::m_nfrees += 1;

  // store current hooks
  PerfMon::MemStats::saveHooks();

  // reset our hooks
  PerfMon::MemStats::installHooks();

  return;
}

#endif /* __linux__ */

