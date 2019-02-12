// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaPoolAllocator.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaPoolAllocator.h
 * @author scott snyder
 * @date May 2007
 * @brief Pool-based allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */

#ifndef ATHALLOCATORS_ARENAPOOLALLOCATOR_H
#define ATHALLOCATORS_ARENAPOOLALLOCATOR_H


#include "AthAllocators/ArenaBlockAllocatorBase.h"
#include "boost/iterator/iterator_adaptor.hpp"
#include <cstdlib>


namespace SG {


class ArenaBlock;


/**
 * @brief Pool-based allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * This is a block-based memory allocator, with stack-like behavior
 * (like the @c DataPool class).  We do not allow freeing individual
 * elements; instead, we support @c resetTo(p), which frees @p and all
 * elements allocated after it (in this allocator).  We also implement
 * an iterator over the allocated blocks.
 */
class ArenaPoolAllocator
  : public ArenaBlockAllocatorBase
{
public:
  // Needed forward declaration.
  class const_iterator;

  /**
   * @brief Non-const iterator for the pool.
   *        It iterates over all allocated blocks (in unspecified order).
   *
   *        We use @c boost::iterator_adaptor, and take a @c pointer
   *        as the base iterator type.  Besides that, we also need
   *        to record the current block which we're within.
   */
  class iterator
    : public boost::iterator_adaptor<
    iterator,
    pointer,
    boost::use_default,
    boost::forward_traversal_tag>
  {
  public:
    /**
     * @brief Default constructor.
     */
    iterator ();


    /**
     * @brief Constructor.
     * @param p Pointer to the element.
     * @param block Block containing the element.
     */
    iterator (pointer p, ArenaBlock* block);


    // To allow initializing a @c const_iterator from an @c iterator.
    friend class const_iterator;


  private:
    /// Block containing the current element.
    ArenaBlock* m_block;

    // Required by @c iterator_adaptor.
    friend class boost::iterator_core_access;

    /// Move the iterator forward.
    void increment();
  };


  /**
   * @brief Const iterator for the pool.
   *        It iterates over all allocated blocks (in unspecified order).
   *
   *        We use @c boost::iterator_adaptor, and take a @c pointer
   *        as the base iterator type.  Besides that, we also need
   *        to record the current block which we're within.
   */
  class const_iterator
    : public boost::iterator_adaptor<
    const_iterator,
    const_pointer,
    boost::use_default,
    boost::forward_traversal_tag>
  {
  public:
    /**
     * @brief Default constructor.
     */
    const_iterator ();


    /**
     * @brief Constructor.
     * @param p Pointer to the element.
     * @param block Block containing the element.
     */
    const_iterator (pointer p, ArenaBlock* block);


    /**
     * @brief Constructor from @c iterator.
     * @param it The iterator to copy.
     */
    const_iterator (const iterator& it);
                                           

  private:
    /// Block containing the current element.
    ArenaBlock* m_block;

    // Required by @c iterator_adaptor.
    friend class boost::iterator_core_access;

    /// Move the iterator forward.
    void increment();
  };


  /**
   * @brief Constructor.
   * @param params The parameters structure for this allocator.
   *               See @c  ArenaAllocatorBase.h for the contents.
   */
  ArenaPoolAllocator (const Params& params);

 
  /**
   * @brief Destructor.  This will free all the Allocator's storage.
   */
  virtual ~ArenaPoolAllocator();


  /// Don't allow copy construction or assignment.
  ArenaPoolAllocator (const ArenaPoolAllocator&) = delete;
  ArenaPoolAllocator& operator= (const ArenaPoolAllocator&) = delete;


  /**
   * @brief Move constructor.
   */
  ArenaPoolAllocator (ArenaPoolAllocator&& other);


  /**
   * @brief Move assignment.
   */
  ArenaPoolAllocator& operator= (ArenaPoolAllocator&& other);


  /**
   * @brief Swap.
   */
  void swap (ArenaPoolAllocator& other);


  /**
   * @brief Allocate a new element.
   *
   * The fast path of this will be completely inlined.
   */
  pointer allocate();


  /**
   * @brief Free all allocated elements.
   *
   * All elements allocated are returned to the free state.
   * @c clear should be called on them if it was provided.
   * The elements may continue to be cached internally, without
   * returning to the system.
   */
  virtual void reset() override;


  /**
   * @brief Free all allocated elements and release memory back to the system.
   *
   * All elements allocated are freed, and all allocated blocks of memory
   * are released back to the system.
   * @c destructor should be called on them if it was provided
   * (preceded by @c clear if provided and @c mustClear was set).
   */
  // cppcheck-suppress virtualCallInConstructor ; false positive
  virtual void erase() override final;


  /**
   * @brief Reset pool back to a previous state.
   * @param p The pointer back to which to reset.
   *
   * This will free (a la @c reset) the element @c p and all elements
   * that have been allocated after it from this allocator.
   */
  void resetTo (pointer p);


  /**
   * @brief Starting pool iterator.
   *
   * This will iterate over all allocated elements (in unspecified order).
   * It is a @c forward_iterator.
   */
  iterator begin();


  /**
   * @brief Starting pool const iterator.
   *
   * This will iterate over all allocated elements (in unspecified order).
   * It is a @c forward_iterator.
   */
  const_iterator begin() const;


  /**
   * @brief Ending pool iterator.
   */
  iterator end();


  /**
   * @brief Ending pool const iterator.
   */
  const_iterator end() const;


private:
  /**
   * @brief Add more free elements to the pool.
   */
  void refill();


  /**
   * @brief Reset all elements in the topmost block, and move the block
   *        to the free list.
   */
  void clearBlock();


  /// Pointer to the next free element to allocate, or null.
  pointer m_ptr;

  /// One past the last available element in the current block, of null.
  pointer m_end;
};


} // namespace SG


#include "AthAllocators/ArenaPoolAllocator.icc"


#endif // not ATHALLOCATORS_ARENAPOOLALLOCATOR_H
