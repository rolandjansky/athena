/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  RootUtils/src/DsoRootLock.cxx
 * @author scott snyder
 * @date May, 2022
 * @brief Workaround for ROOT TClassTable locking problem.
 */


#include "CxxUtils/AthDsoCbk.h"
#include "CxxUtils/checker_macros.h"
#include "TVirtualRWMutex.h"


/**
 * @brief Workaround for ROOT TClassTable locking problem.
 *
 * TClassTable is an internal ROOT structure used to hold information about
 * all classes known to ROOT, whether or not full dictionary information
 * has been built.  This is effectively a singleton; however, TClassTable
 * itself contains no locking.  Rather, it relies on the caller already
 * having acquired the ROOT core mutex.  In most cases, that happens.
 * However, when a shared object containing ROOT dictionaries is loaded,
 * calls are made to TClassTable to record the classes present in that
 * dictionary.  But, in that case, the core mutex is not acquired,
 * resulting in a potential race.
 *
 * This would be best addressed inside ROOT, by acquiring the lock
 * in TGenericClassInfo::Init.  As a temporary workaround, however,
 * use the existing DSO hooks to acquire the root lock around all shared
 * library loads.
 */
class DsoRootLock
{
public:
  /**
   * @brief Constructor.  Register the hook.
   */
  DsoRootLock();


  /**
   * @brief Acquire the ROOT core lock during library loading.
   */
  static int hook (const ath_dso_event* event, void* userdata);
};


/**
 * @brief Constructor.  Register the hook.
 */
DsoRootLock::DsoRootLock()
{
  ath_dso_cbk_register (hook, nullptr);
}


/**
 * @brief Acquire the ROOT core lock during library loading.
 */
int DsoRootLock::hook (const ath_dso_event* event, void* /*userdata*/)
{
  // If the core mutex has been created, acquire it before loading
  // a library and release it on completion.
  // Hope these are always properly nested!
  ROOT::TVirtualRWMutex* m ATLAS_THREAD_SAFE = ROOT::gCoreMutex;
  if (m) {
    if (event->step == 0) {
      m->WriteLock();
    }
    else if (event->step == 1)
    {
      m->WriteUnLock(nullptr);
    }
  }
  return 0;
}


/// Set up this hook when this library gets loaded.
const DsoRootLock dsoRootLock;
