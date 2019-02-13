/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  AthAllocators/src/ArenaBase.cxx
 * @author scott snyder
 * @date Mar 2017
 * @brief Part of @c Arena dealing with the list of allocators.
 *        Broken out from @c Arena to avoid a dependency loop
 *        with @c ArenaHeader.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */

#include "AthAllocators/ArenaBase.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"


namespace SG {


/**
 * @brief Constructor.
 * @param name The name of this @c Arena; to use in reports.
 */
ArenaBase::ArenaBase (const std::string& name /*= ""*/)
  : m_name (name)
{
}


/**
 * @brief Destructor.
 */
ArenaBase::~ArenaBase()
{
}


/**
 * @brief Reset all contained allocators.  All elements will be freed.
 */
void ArenaBase::reset()
{
  lock_t l (m_mutex);
  for (AllocEntry& alloc : m_allocs) {
    if (alloc.m_alloc) {
      lock_t alloc_lock (*alloc.m_mutex);
      alloc.m_alloc->reset();
    }
  }
}


/**
 * @brief Erase all contained allocators.  All elements will be freed,
 *        and the memory returned to the system.
 */
void ArenaBase::erase()
{
  lock_t l (m_mutex);
  for (AllocEntry& alloc : m_allocs) {
    if (alloc.m_alloc) {
      lock_t alloc_lock (*alloc.m_mutex);
      alloc.m_alloc->erase();
    }
  }
}


/**
 * @brief Generate a report of the memory in use by this @c Arena.
 * @param os The stream to which to write the report.
 */
void ArenaBase::report (std::ostream& os) const
{
  lock_t l (m_mutex);
  bool first = true;
  for (const AllocEntry& alloc : m_allocs) {
    if (alloc.m_alloc) {
      if (first) {
        ArenaAllocatorBase::Stats::header (os);
        os << std::endl;
        first = false;
      }
      lock_t alloc_lock (*alloc.m_mutex);
      alloc.m_alloc->report (os);
    }
  }
}


/**
 * @brief Return statistics summed over all allocators in this @c Arena.
 */
ArenaAllocatorBase::Stats ArenaBase::stats () const
{
  ArenaAllocatorBase::Stats stats;
  lock_t l (m_mutex);
  for (const AllocEntry& alloc : m_allocs) {
    if (alloc.m_alloc) {
      lock_t alloc_lock (*alloc.m_mutex);
      stats += alloc.m_alloc->stats();
    }
  }
  return stats;
}


/**
 * @brief Return this @c Arena's name.
 */
const std::string& ArenaBase::name() const
{
  return m_name;
}


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
ArenaAllocatorBase* ArenaBase::makeAllocator (size_t i)
{
  // We have to create a new Allocator.
  // Make sure there's room in the vector.
  if (m_allocs.size() <= i) {
    m_allocs.resize (i+1);
  }

  // Create the Allocator, using the Registry.
  std::unique_ptr<ArenaAllocatorBase> alloc =
    ArenaAllocatorRegistry::instance()->create (i);

  // Install it in the vector.
  m_allocs[i].m_alloc = std::move (alloc);
  m_allocs[i].m_mutex = std::make_unique<std::mutex>();

  return m_allocs[i].m_alloc.get();
}


} // namespace SG
