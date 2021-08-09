/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
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
                                                    int nblock)
  : m_owner (owner),
    m_nblock (nblock)
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
  for (size_t i = 0; i < sz; i++) {
    delete m_allocators[i];
  }
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
      

/**
 * @brief Generate printable report for all contained allocators.
 * @param os Stream to which to write the report.
 */
void ArenaSharedHeapSTLHeader::report (std::ostream& os) const
{
  for (size_t i = 0; i < m_allocators.size(); i++) {
    if (m_allocators[i])
      m_allocators[i]->report (os);
  }
}


/**
 * @brief Write-protect the memory managed by these allocators.
 *
 * Adjust protection on the memory managed by these allocators
 * to disallow writes.
 */
void ArenaSharedHeapSTLHeader::protect()
{
  for (ArenaHeapAllocator* a : m_allocators) {
    if (a) {
      a->protect();
    }
  }
}


/**
 * @brief Write-enable the memory managed by these allocators.
 *
 * Adjust protection on the memory managed by these allocators
 * to allow writes.
 */
void ArenaSharedHeapSTLHeader::unprotect()
{
  for (ArenaHeapAllocator* a : m_allocators) {
    if (a) {
      a->unprotect();
    }
  }
}


} // namespace SG
