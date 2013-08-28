/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaSharedHeapSTLAllocator.cxx 470825 2011-11-25 23:20:57Z ssnyder $
/**
 * @file AthAllocators/src/ArenaSharedHeapSTLAllocator.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief STL-style allocator wrapper for @c ArenaHeapAllocator allowing
 *        the heap to be shared between containers.
 */


#include "AthAllocators/ArenaSharedHeapSTLAllocator.h"


namespace SG {


/**
 * @brief Constructor.
 * @param owner Address of the object that owns this Header.
 * @param nblock Value to set in the parameters structure for the
 *               number of elements to allocate per block.
 * @param name   Value to use as the base for the allocator names.
 */
ArenaSharedHeapSTLHeader::ArenaSharedHeapSTLHeader (const void* owner,
                                                    int nblock,
                                                    const std::string& name)
  : m_owner (owner),
    m_nblock (nblock),
    m_name (name)
{
}


/**
 * @brief Destructor.
 *
 * Destroy all the allocators we own.
 */
ArenaSharedHeapSTLHeader::~ArenaSharedHeapSTLHeader()
{
  size_t sz = m_allocators.size();
  for (size_t i = 0; i < sz; i++)
    delete m_allocators[i];
}


/**
 * @brief Return allocator statistics summed over all our owned allocators.
 */
ArenaAllocatorBase::Stats ArenaSharedHeapSTLHeader::totstats() const
{
  ArenaAllocatorBase::Stats stats;
  size_t sz = m_allocators.size();
  for (size_t i = 0; i < sz; i++) {
    if (m_allocators[i])
      stats += m_allocators[i]->stats();
  }
  return stats;
}
      

} // namespace SG
