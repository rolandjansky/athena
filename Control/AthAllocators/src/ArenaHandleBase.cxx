/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandleBase.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthAllocators/src/ArenaHandleBase.cxx
 * @author scott snyder
 * @date May 2007
 * @brief Base class for all @c Handle classes, containing parts that
 *        do not depend on the referenced type.
 */


#include "AthAllocators/ArenaHandleBase.h"
#include "AthAllocators/ArenaAllocatorBase.h"


namespace SG {


/**
 * @brief Constructor.
 * @param header The group of Arenas which this Handle may reference.
 *               May be null to select the global default.
 * @param index The index of this Handle's Allocator type.
 */
ArenaHandleBase::ArenaHandleBase (ArenaHeader* header, size_t index)
{
  // If the supplied header is null, use the global default.
  if (!header)
    header = ArenaHeader::defaultHeader();

  // Get the allocator.
  m_allocator = header->allocator (index);
}


/**
 * @brief Free all allocated elements (of this type in the current Arena).
 *
 * All elements allocated in the current Arena by our associated
 * Allocator are returned to the
 * free state.  @c clear should be called on them if it was provided.
 * The elements may continue to be cached internally, without
 * returning to the system.
 */
void ArenaHandleBase::reset()
{
  return baseAllocator()->reset();
}


/**
 * @brief Free all allocated elements and release memory back to the system
 *        (of this type in the current Arena).
 *
 * All elements allocated in the current Arena by our associated
 * Allocator are freed, and all allocated blocks of memory
 * are released back to the system.
 * @c destructor should be called on them if it was provided
 * (preceded by @c clear if provided and @c mustClear was set).
 */
void ArenaHandleBase::erase()
{
  return baseAllocator()->erase();
}


/**
 * @brief Set the total number of elements cached by the allocator
 *        (in the current Arena).
 * @param size The desired pool size.
 *
 * This allows changing the number of elements that are currently free
 * but cached.  Any allocated elements are not affected by this call.
 *
 * If @c size is greater than the total number of elements currently
 * cached, then more will be allocated.  This will preferably done
 * with a single block, but that is not guaranteed; in addition, the
 * allocator may allocate more elements than is requested.
 *
 * If @c size is smaller than the total number of elements currently
 * cached, as many blocks as possible will be released back to the system.
 * It may not be possible to release the number of elements requested;
 * this should be implemented on a best-effort basis.
 */
void ArenaHandleBase::reserve (size_t size)
{
  return baseAllocator()->reserve (size);
}


/**
 * @brief Return the statistics block for this allocator,
 * for the current Arena.
 */
const ArenaAllocatorBase::Stats& ArenaHandleBase::stats() const
{
  return baseAllocator()->stats();
}


} // namespace SG
