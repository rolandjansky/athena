// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBase.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaBase.h
 * @author scott snyder
 * @date May 2007
 * @brief Part of @c Arena dealing with the list of allocators.
 *        Broken out from @c Arena to avoid a dependency loop
 *        with @c ArenaHeader.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */

#ifndef ATLALLOCATORS_ARENABASE_H
#define ATLALLOCATORS_ARENABASE_H


#include "AthAllocators/ArenaAllocatorBase.h"
#include "AthAllocators/LockedAllocator.h"
#include <iosfwd>
#include <string>
#include <vector>
#include <memory>
#include <mutex>


namespace SG {


/**
 * @brief Part of @c Arena dealing with the list of allocators.
 *        Broken out from @c Arena to avoid a dependency loop
 *        with @c ArenaHeader.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */
class ArenaBase
{
public:
  /**
   * @brief Constructor.
   * @param name The name of this @c Arena; to use in reports.
   */
  ArenaBase (const std::string& name = "");


  /// Destructor.
  virtual ~ArenaBase();


  ArenaBase (const ArenaBase&) = delete;
  ArenaBase& operator= (const ArenaBase&) = delete;
  


  /**
   * @brief Translate an integer index to an Allocator pointer.
   * @param i The index to look up.
   *
   * If the index isn't valid, an assertion will be tripped.
   */
  LockedAllocator allocator (size_t i);


  /**
   * @brief Reset all contained allocators.  All elements will be freed.
   */
  void reset();


  /**
   * @brief Erase all contained allocators.  All elements will be freed,
   *        and the memory returned to the system.
   */
  void erase();


  /**
   * @brief Generate a report of the memory in use by this @c Arena.
   * @param os The stream to which to write the report.
   */
  void report (std::ostream& os) const;


  /**
   * @brief Return statistics summed over all allocators in this @c Arena.
   */
  ArenaAllocatorBase::Stats stats() const;


  /**
   * @brief Return this @c Arena's name.
   */
  const std::string& name() const;


private:
  typedef std::lock_guard<std::mutex> lock_t;


  /**
   * @brief Make a new Allocator for index i.
   * @param i The index of the Allocator.
   *
   * The Allocator vector was empty for index @c i.  Make an appropriate
   * new Allocator, store it in the vector, and return it.  Will trip
   * an assertion if the index is not valid.
   *
   * This should be called with m_mutex held.
   */
  ArenaAllocatorBase* makeAllocator (size_t i);


  /// Our allocator vector.
  struct AllocEntry
  {
    std::unique_ptr<ArenaAllocatorBase> m_alloc;
    std::unique_ptr<std::mutex> m_mutex;
  };
  std::vector<AllocEntry> m_allocs;

  /// Our name.
  std::string m_name;

  /// To guard access to m_allocs.
  mutable std::mutex m_mutex;
};


} // namespace SG


#include "AthAllocators/ArenaBase.icc"


#endif // not ATLALLOCATORS_ARENABASE_H

