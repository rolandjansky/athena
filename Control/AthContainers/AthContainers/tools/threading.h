// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/threading.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Threading definitions.
 *
 * In a standard build, we define the locking objects below.
 * In a standalone build, or with ATHCONTAINERS_NO_THREADS defined,
 * they're defined as no-ops.
 */


#ifndef ATHCONTAINERS_THREADING_H
#define ATHCONTAINERS_THREADING_H


#if defined(__GCCXML__)
# ifndef ATHCONTAINERS_NO_THREADS
#  define ATHCONTAINERS_NO_THREADS
# endif
#endif


#ifdef ATHCONTAINERS_NO_THREADS


namespace AthContainers_detail {


/**
 * @brief No-op definition of @c mutex.
 */
class mutex {};


/**
 * @brief No-op definition of @c lock_guard.
 */
template <class LOCKABLE>
class lock_guard
{
public:
  lock_guard() {}
  lock_guard(LOCKABLE&) {}
};


/**
 * @brief No-op definition of @c upgrade_mutex.
 */
class upgrade_mutex
{
public:
  void lock() {}
  void unlock() {}
  void lock_shared() {}
  void unlock_shared() {}
  void lock_upgrade() {}
  void unlock_upgrade() {}
  void unlock_upgrade_and_lock() {}
private:
  // Real upgrade_mutex is not assignable.  Need this for proper
  // dictionary generation.
  upgrade_mutex& operator= (const upgrade_mutex&);
};


/**
 * @brief Single-thread version of thread_specific_ptr.
 */
template <class T>
class thread_specific_ptr
{
public:
  thread_specific_ptr() : m_ptr(0) {}
  ~thread_specific_ptr() { delete m_ptr; }
  thread_specific_ptr (const thread_specific_ptr&) = delete;
  thread_specific_ptr& operator= (const thread_specific_ptr&) = delete;
  T* get() { return m_ptr; }
  T* operator->() { return m_ptr; }
  T& operator*() { return *m_ptr; }
  const T* get() const { return m_ptr; }
  const T* operator->() const { return m_ptr; }
  const T& operator*() const { return *m_ptr; }
  void reset (T* new_value=0) { delete m_ptr; m_ptr = new_value; }
  T* release() { T* ret = m_ptr; m_ptr = 0; return ret; }

private:
  T* m_ptr;
};


/// No-op definitions of fences.
inline void fence_acq_rel() {}
inline void fence_seq_cst() {}


} // namespace AthContainers_detail


#else  // not ATHCONTAINERS_NO_THREADS

//the shared_mutex include generates annoying unused_variable warnings during compilations... silence this in athanalysisbase
#ifdef XAOD_ANALYSIS
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif
#endif

#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/tss.hpp"
#if __cplusplus > 201100
# include <atomic>
# include <mutex>
# include <thread>
namespace SG_STD_OR_BOOST = std;
#else
# include "boost/atomic.hpp"
# include "boost/thread/mutex.hpp"
# include "boost/thread/thread.hpp"
namespace SG_STD_OR_BOOST = boost;
#endif


namespace AthContainers_detail {


// Take these from boost.  (Boost extensions not in C++.)
using boost::upgrade_mutex;
using boost::thread_specific_ptr;

// Other mutex/lock types come from either boost or std.
using SG_STD_OR_BOOST::mutex;
using SG_STD_OR_BOOST::lock_guard;
using SG_STD_OR_BOOST::thread;


/**
 * @brief An acquire/release fence.
 */
void fence_acq_rel();


/**
 * @brief A sequentially-consistent fence.
 */
void fence_seq_cst();


} // namespace AthContainers_detail


#endif // not ATHCONTAINERS_NO_THREADS



namespace AthContainers_detail {


/**
 * @brief Lock object for taking out shared locks.
 *
 * This is like the boost @c strict_lock, except that it takes out a shared
 * lock on the underlying object rather than an exclusive one.
 */
template <typename LOCKABLE>
class strict_shared_lock 
{
public:
  /// The underlying object type.
  typedef LOCKABLE lockable_type;


  /**
   * @brief Take out a shared lock on @c obj and remember it.
   * @param obj The lockable object.
   */
  explicit strict_shared_lock(lockable_type& obj);


  /**
   * @brief Take out a shared lock on @c obj and remember it.
   * @param obj The lockable object.
   */
  explicit strict_shared_lock(const lockable_type& obj);
  

  /**
   * @brief Release the held lock.
   */
  ~strict_shared_lock();


private:
  // Disallow these.
  strict_shared_lock();
  strict_shared_lock(strict_shared_lock const&);
  strict_shared_lock& operator=(strict_shared_lock const&);


private:
  /// The lock being held.
  lockable_type& m_obj;
};


/**
 * @brief Lock object for taking out upgradable locks.
 *
 * The will first be taken out in upgrade mode.
 * If @c upgrade() is called, then the lock will be upgraded to exclusive mode.
 * When this object is destroyed, the object will be unlocked for either
 * upgrade or exclusive mode, as appropriate.
 */
template <class LOCKABLE>
class upgrading_lock
{
public:
  /// The underlying object type.
  typedef LOCKABLE lockable_type;


  /**
   * @brief Take out an upgrade lock on @c obj and remember it.
   * @param obj The lockable object.
   */
  explicit upgrading_lock (lockable_type& obj);


  /**
   * @brief Release the held lock.
   */
  ~upgrading_lock();


  /**
   * @brief Convert the lock from upgrade to exclusive.
   */
  void upgrade();


private:
  // Disallow these.
  upgrading_lock();
  upgrading_lock(upgrading_lock const&);
  upgrading_lock& operator=(upgrading_lock const&);


private:
  /// The lock being held.
  lockable_type& m_obj;

  /// Has the lock been converted to exclusive?
  bool m_exclusive;
};


} // namespace AthContainers_detail


#include "AthContainers/tools/threading.icc"


#endif // not ATHCONTAINERS_THREADING_H
