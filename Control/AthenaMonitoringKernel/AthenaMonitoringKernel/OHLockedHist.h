/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_OHLockedHist_h
#define AthenaMonitoringKernel_OHLockedHist_h

/**
 * @file   OHLockedHist.h
 * @brief  OH histogram lock header file
 * @author Tomasz Bold, Frank Winklmeier
 *
 * Provides a mutex to lock histogram operations to avoid OH publication while
 * certain operations (e.g. re-binning) are ongoing. This is only relevant for
 * online operations and only has any effect once oh_lock_histogram_mutex::set_histogram_mutex
 * has been called to set a reference to the global OH mutex in tdaq:
 *   https://gitlab.cern.ch/atlas-tdaq-software/oh/blob/master/oh/OHRootMutex.h.
 *
 **/

#include "CxxUtils/checker_macros.h"

#include <mutex>

/**
 * @brief Helper class to set histogram mutex
 *
 * This is used by the HLT framework to set the reference to the OH mutex.
 **/
class oh_lock_histogram_mutex {
public:
  oh_lock_histogram_mutex() = default;
  ~oh_lock_histogram_mutex() = default;

  /// Set mutex to be used in oh_lock_histogram
  static void set_histogram_mutex(std::mutex& mutex) { m_mutex = &mutex; }

  /// Reset (disable) histogram mutex
  static void reset_histogram_mutex() { m_mutex = nullptr; }

  // no copy
  oh_lock_histogram_mutex(const oh_lock_histogram_mutex&) = delete;
  oh_lock_histogram_mutex& operator=(const oh_lock_histogram_mutex&) = delete;

protected:
  static std::mutex* m_mutex ATLAS_THREAD_SAFE;
};

/**
 * @brief Lock to be used for thread unsafe histogram operations
 *
 * The TH1::Fill and other operations may sometimes involve
 * ROOT global variables access. This usually leads to crashes
 * as the histogram may be accessed at the same time for OH
 * publication.
 *
 * This usually happens when histograms have labeled bins, but
 * may also happen in other cases involving re-binning, cloning etc.
 * For those (and only those) operations one should lock the histogram
 * access via the OH mutex:
 *
 * \code
 *   TH1F * myhistogram = new ....
 *   // instead of myhistogram->Fill("blabla")
 *   oh_lock_histogram<TH1F>(myhistogram)->Fill("blabla");
 * \endcode
 **/
template <class H>
class oh_lock_histogram : public oh_lock_histogram_mutex {
public:
  /// Lock
  oh_lock_histogram(H* h) : m_hist(h)
  {
    if (m_mutex) m_mutex->lock();
  }

  /// Access to histogram pointer
  H* operator->() { return m_hist; }

  /// Unlock
  ~oh_lock_histogram()
  {
    if (m_mutex) m_mutex->unlock();
  }

private:
  /// Do not allow dynamic allocation to avoid accidental deadlocks
  void* operator new(size_t);

  H* m_hist;
};

/**
 * @brief Scoped lock to be used for threaded histogram operations
 *
 * This is the scoped version of oh_lock_histogram in case you
 * need to protect several histogram operations.
 *
 * Example:
 * \code
 * {
 *   oh_scoped_lock_histogram lock;
 *   myhistogram->Fill(...);
 *   myhistogram->LabelsDeflate();
 * }
 * \endcode
 **/
class oh_scoped_lock_histogram : public oh_lock_histogram_mutex {
public:
  /// Lock
  oh_scoped_lock_histogram()
  {
    if (m_mutex) m_mutex->lock();
  }

  /// Unlock
  ~oh_scoped_lock_histogram()
  {
    if (m_mutex) m_mutex->unlock();
  }

private:
  /// Do not allow dynamic allocation to avoid accidental deadlocks
  void* operator new(size_t);
};

#endif
