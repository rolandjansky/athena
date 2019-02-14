// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBlockAllocatorBase.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaBlockAllocatorBase.h
 * @author scott snyder
 * @date May 2007
 * @brief Common functionality for block-oriented allocators.
 */


#ifndef ATHALLOCATORS_ARENABLOCKALLOCATORBASE_H
#define ATHALLOCATORS_ARENABLOCKALLOCATORBASE_H


#include "AthAllocators/ArenaAllocatorBase.h"


namespace SG {


class ArenaBlock;


/**
 * @brief Common functionality for block-oriented allocators.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * This class factors out some common functionality for allocators
 * that use @c ArenaBlock.
 */
class ArenaBlockAllocatorBase
  : public ArenaAllocatorBase
{
public:
  /**
   * @brief Constructor.
   * @param params The parameters structure for this allocator.
   *               See @c  ArenaAllocatorBase.h for the contents.
   */
  ArenaBlockAllocatorBase (const Params& params);


  /**
   * @brief Destructor.
   */
  virtual ~ArenaBlockAllocatorBase();


  /// Don't allow copy construction or assignment.
  ArenaBlockAllocatorBase (const ArenaBlockAllocatorBase&) = delete;
  ArenaBlockAllocatorBase& operator= (const ArenaBlockAllocatorBase&) = delete;


  /**
   * @brief Move constructor.
   */
  ArenaBlockAllocatorBase (ArenaBlockAllocatorBase&& other);


  /**
   * @brief Move assignment.
   */
  ArenaBlockAllocatorBase& operator= (ArenaBlockAllocatorBase&& other);


  /**
   * @brief Swap.
   */
  void swap (ArenaBlockAllocatorBase& other);



  /**
   * @brief Set the total number of elements cached by the allocator.
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
  virtual void reserve (size_t size) override;


  /**
   * @brief Free all allocated elements and release memory back to the system.
   *
   * All elements allocated are freed, and all allocated blocks of memory
   * are released back to the system.
   * @c destructor should be called on them if it was provided
   * (preceded by @c clear if provided and @c mustClear was set).
   */
  virtual void erase() override;


  /**
   * @brief Return the statistics block for this allocator.
   */
  virtual Stats stats() const override;


  /**
   * @brief Return the name of this allocator.
   */
  virtual const std::string& name() const override;


  /**
   * @brief Return this Allocator's parameters.
   */
  const Params& params() const;


protected:
  /**
   * @brief Return an empty block, either newly-allocated or from the
   *        free list.  Update statistics appropriately.
   */
  ArenaBlock* getBlock();

  /// The parameters for this allocator.
  Params m_params;

  /// The list of blocks currently in use.
  ArenaBlock* m_blocks;

  /// The list of free blocks.
  ArenaBlock* m_freeblocks;

  /// The statistics structure.
  ArenaAllocatorBase::Stats m_stats;
};


} // namespace SG


#endif // not ATHALLOCATORS_ARENABLOCKALLOCATORBASE_H
