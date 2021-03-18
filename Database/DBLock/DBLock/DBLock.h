// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file DBLock/DBLock.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2021
 * @brief Common database lock.
 */


#ifndef DBLOCK_DBLOCK_H
#define DBLOCK_DBLOCK_H


#include <mutex>
#include <memory>


namespace Athena {


/**
 * @brief Common database lock.
 *
 * Athena has multiple packages that access the database independently,
 * for example IOVDbSvc (via COOL) and RDBAccessSvc.  These are all locked
 * internally, but they don't know about each other.  Further, some of
 * the underlying database code which they can both use, such as frontier,
 * is not thread-safe.  Thus, we can get crashes if, say, IOVDbSvc and
 * RDBAccessSvc are executing in different threads.
 *
 * To solve this without introducing a depedency between these packages,
 * we introduce this package containing only this lock class.  Any Athena
 * package that needs exclusive access to the database layer can then
 * acquire the lock here.
 *
 * The DBLock object copyable; it holds the underlying lock
 * via a shared_ptr.  Further, the underlying mutex is recursive.
 *
 * Beware of potential deadlocks if this is used in conjunction
 * with another lock.  Best to always acquire the locks in the same order.
 */
class DBLock
{
public:
  DBLock();
  DBLock(const DBLock&) = default;
  DBLock(DBLock&&) = default;
  DBLock& operator=(const DBLock&) = default;
  DBLock& operator=(DBLock&&) = default;
  ~DBLock();


private:
  /// Global mutex to protect database access.
  static std::recursive_mutex m_mutex;

  /// Shared pointer to underlying lock.
  /// This allows these objects to be copyable.
  using lock_t = std::unique_lock<std::recursive_mutex>;
  std::shared_ptr<lock_t> m_lockPtr;
};


} // namespace Athena


#endif // not DBLOCK_DBLOCK_H
