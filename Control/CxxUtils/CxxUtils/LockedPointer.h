// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/LockedPointer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2021
 * @brief A pointer together with a movable lock.
 */


#ifndef CXXUTILS_LOCKEDPOINTER_H
#define CXXUTILS_LOCKEDPOINTER_H


#include <mutex>


namespace CxxUtils {


/**
 * @brief A pointer together with a movable lock.
 *
 * This class holds a pointer to T along with a unique_lock.
 * It can be used where we want to return a pointer to an object
 * protected by a lock, and so want to return the lock along
 * with the pointer.
 *
 * Objects of this class may be moved but not copied (like unique_ptr).
 */
template <class T, class MUTEX=std::recursive_mutex>
class LockedPointer
{
public:
  LockedPointer (T& p, std::unique_lock<MUTEX>&& lock)
    : m_lock (std::move (lock)),
      m_p (p)
  {
  }

  T* get() { return &m_p; }
  T* operator->() { return &m_p; }
  T& operator*() { return m_p; }

private:
  std::unique_lock<MUTEX> m_lock;
  T& m_p;
};

} // namespace CxxUtils


#endif // not CXXUTILS_LOCKEDPOINTER_H
