/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigMonitorBase_TrigLockedHist_h
#define TrigMonitorBase_TrigLockedHist_h

/**
 * @file   TrigLockedHist.h
 * @brief  Histogram lock header file
 * @author Tomasz Bold, Frank Winklmeier
 *
 * $Id:$
 **/

#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>

/**
 * @brief Helper class to set histogram mutex
 *
 * This is used by the HLT framework to provide a reference to the OH mutex.
 **/
class lock_histogram_mutex : public boost::noncopyable {
public:
  /// Set mutex to be used in lock_histogram_operation
  static void set_histogram_mutex(boost::mutex& mutex) {
    m_mutex = &mutex;
  }
  
  /// Reset (disable) histogram mutex
  static void reset_histogram_mutex() {
    m_mutex = 0;
  }

protected:
  static boost::mutex* m_mutex;
};

/**
 * @brief Lock to be used for threaded histogram operations
 *
 * The TH1::Fill and other operations may sometimes involve
 * ROOT global variables access. This usually leads to crashes
 * as the histograms can be at the same time sent to online monitoring 
 * infrastructure by another thread. 
 * Which also involves conversion of TH? into tdaq internal format.
 *
 * This usually happens when histograms have labeled bins.
 * But may also happen in other cases involving re-binning, cloning etc.
 *
 * Usage of this utility:
 * \code
 *   TH1F * myhistogram = new ....
 *   // registration 
 *   // instead of myhistogram->Fill("blabla")
 *   lock_histogram_operation<TH1F>(myhistogram)->Fill("blabla");
 * \endcode
 **/
template<class H>
class lock_histogram_operation : public lock_histogram_mutex {
public:
  /// Lock
  lock_histogram_operation(H* h): m_hist(h) {
    if (m_mutex) m_mutex->lock();      
  }

  /// Access to histogram pointer
  H* operator->() {
    return m_hist;
  }

  /// Unlock
  ~lock_histogram_operation() {
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
 * This is the scoped version of lock_histogram_operation in case you
 * need to protect several histogram operations.
 *
 * Example:
 * \code
 * {
 *   scoped_lock_histogram lock;
 *   myhistogram->Fill(...);
 *   myhistogram->LabelsDeflate();
 * }
 * \endcode
 **/
class scoped_lock_histogram : public lock_histogram_mutex {
public:
  /// Lock
  scoped_lock_histogram() {
    if (m_mutex) m_mutex->lock();
  }

  /// Unlock
  ~scoped_lock_histogram() {
    if (m_mutex) m_mutex->unlock();
  }

private:
  /// Do not allow dynamic allocation to avoid accidental deadlocks
  void* operator new(size_t);  
};

#endif
