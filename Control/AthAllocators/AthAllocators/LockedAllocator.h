// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthAllocators/LockedAllocator.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief A pointer type that holds a lock on an allocator object.
 */


#include <mutex>


#ifndef ATHALLOCATORS_LOCKEDALLOCATOR_H
#define ATHALLOCATORS_LOCKEDALLOCATOR_H


namespace SG {


class ArenaAllocatorBase;


/**
 * @brief A pointer type that holds a lock on an allocator object.
 *
 * A @c SG::LockedAllocator object functions as a pointer
 * to a @c SG::ArenaAllocatorBase object.  It also holds a lock associated
 * with that object; the lock is released when the @c SG::LockedAllocator
 * is destroyed.
 *
 * These objects may be moved, but not copied.
 */
class LockedAllocator
{
public:
  /**
   * @brief Constructor.
   * @param alloc The allocator object to which we're pointing.
   * @param mutex The mutex to lock.
   */
  LockedAllocator (ArenaAllocatorBase* alloc, std::mutex& mutex);


  /**
   * @brief Dereference the pointer.
   */
  ArenaAllocatorBase& operator*();


  /**
   * @brief Dereference the pointer.
   */
  ArenaAllocatorBase* operator->();


  /**
   * @brief Return the underlying pointer.
   */
  ArenaAllocatorBase* get();

  
  /**
   * @brief Return the underlying pointer.
   */
  const ArenaAllocatorBase* get() const;

  
private:
  /// The allocator to which we point.
  ArenaAllocatorBase* m_alloc;

  /// The lock for the allocator.
  std::unique_lock<std::mutex> m_lock;
};


} // namespace SG


#include "AthAllocators/LockedAllocator.icc"


#endif // not ATHALLOCATORS_LOCKEDALLOCATOR_H
