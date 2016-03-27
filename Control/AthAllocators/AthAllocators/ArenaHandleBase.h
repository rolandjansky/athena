// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandleBase.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHandleBase.h
 * @author scott snyder
 * @date May 2007
 * @brief Base class for all @c Handle classes, containing parts that
 *        do not depend on the referenced type.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */

#ifndef ATLALLOCATORS_ARENAHANDLEBASE_H
#define ATLALLOCATORS_ARENAHANDLEBASE_H

#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include <cstdlib>


namespace SG {


/**
 * @brief Base class for all @c Handle classes, containing parts that
 *        do not depend on the referenced type.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * A @em Handle is the interface the application uses to allocate memory.
 * A Handle is templated on the type being allocated as well as on the
 * underlying Allocator.  When a Handle is constructed, it is associated
 * with the Allocator associated with the Arena that is current at that time
 * (a new Allocator is automatically created if required).  Therefore,
 * a Handle should not be passed between threads, and Handle objects
 * should not exist across any point where the current store/Arena
 * may be changed.
 * Multiple Handle implementations may be available, implementing
 * different strategies for initializing the elements.
 *
 * This class contains the parts of the Handle interface that do not
 * depend on the template parameters.
 *
 * The first time a given Handle type is seen, it is assigned an index
 * in the the the @c Arena @c Allocator vector.  A @c Handle forwards
 * operations to the underlying @c Allocator.
 */
class ArenaHandleBase
{
public:
  /**
   * @brief Constructor.
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param index The index of this Handle's Allocator type.
   */
  ArenaHandleBase (ArenaHeader* header, size_t index);


  /**
   * @brief Free all allocated elements (of this type in the current Arena).
   *
   * All elements allocated in the current Arena by our associated
   * Allocator are returned to the
   * free state.  @c clear should be called on them if it was provided.
   * The elements may continue to be cached internally, without
   * returning to the system.
   */
  void reset();


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
  void erase();


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
  void reserve (size_t size);


  /**
   * @brief Return the statistics block for this allocator,
   * for the current Arena.
   */
  const ArenaAllocatorBase::Stats& stats() const;


protected:
  /**
   * @brief Return the current Allocator which we are referencing.
   *
   * This may cause a new Allocator to be created.
   */
  ArenaAllocatorBase* baseAllocator() const;


private:
  /// The associated allocator object.
  ArenaAllocatorBase* m_allocator;
};


} // namespace SG


#include "AthAllocators/ArenaHandleBase.icc"


#endif // not ATLALLOCATORS_ARENAHANDLEBASE_H
