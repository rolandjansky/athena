// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeapAllocator.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHeapAllocator.h
 * @author scott snyder
 * @date May 2007
 * @brief Heap-based allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAHEAPALLOCATOR_H
#define ATLALLOCATORS_ARENAHEAPALLOCATOR_H


#include "AthAllocators/ArenaBlockAllocatorBase.h"
#include <cstdlib>


namespace SG {


class ArenaBlock;


/**
 * @brief Heap-based allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * This is a block-based memory allocator, with heap-like behavior.
 * This allows freeing individual elements, but we don't implement
 * @c resetTo or an iterator.
 *
 * There are some extra costs though.
 *
 *  - We need an additional pointer (`link') for each free element.
 *
 *    By default, this is done by increasing the element size by a pointer,
 *    since we don't know whether there is data in the element itself
 *    that must be preserved across free/allocate.  However, if you know
 *    that part of the element may be safely while it is free, then
 *    the allocator can be configured to use that as the link instead.
 *
 *  - When @c reset() is called, we need to call @c clear() on all
 *    allocated elements (if it is defined).  If @c canReclear is set,
 *    then we just call @c clear() on all elements in allocated blocks
 *    on @c reset(), regardless of whether or not the individual elements
 *    are allocated or not.  Otherwise, we make two passes over the elements,
 *    first to build a list of those that are allocated and second
 *    to actually call @c clear().
 *
 *    An intermediate strategy, not currently implemented, could be used
 *    if the link does not overlap the element: set the link to a magic
 *    value when an element is allocated.
 */
class ArenaHeapAllocator
  : public ArenaBlockAllocatorBase
{
public:
  /**
   * @brief Helper to initialize a parameters structure.
   *
   * This creates a @c Params class appropriately initialized for class @c T.
   * Assumptions made:
   *  - The constructor and destructor calls will be filled in
   *    if non-trivial, unless no_ctor or no_dtor is set to true.
   *  - The clear call will be filled in if the optional template parameter
   *    @c clear is true.
   *  - No space will be reserved for an extra link.
   *  - @c canReclear is @c true and @c mustClear is @c false.
   *  - The link will be allocated externally to the element.
   *
   * If these are not appropriate, you can derive from this class
   * and make the appropriate changes.
   */
  template <typename T,
            bool clear = false,
            bool no_ctor = false,
            bool no_dtor = false>
  struct initParams
    : public ArenaAllocatorBase::initParams<T, clear, no_ctor, no_dtor>
  {
    // Short name for our base class.
    typedef ArenaAllocatorBase::initParams<T, clear, no_ctor, no_dtor> Base;


    /**
     * @brief Constructor.
     * @param nblock Value to set in the parameters structure for the
     *               number of elements to allocate per block.
     * @param name   Value to set in the parameters structure for the
     *               allocator name.
     */
    initParams (size_t nblock = 1000, const std::string& name = "");

    /// Return an initialized parameters structure.
    ArenaAllocatorBase::Params params() const;

    /// Return an initialized parameters structure.
    // Note: gcc 3.2.3 doesn't allow defining this out-of-line.
    operator ArenaAllocatorBase::Params() const { return params(); }
  };


  /**
   * @brief Constructor.
   * @param params The parameters structure for this allocator.
   *               See @c  ArenaAllocatorBase.h for the contents.
   */
  ArenaHeapAllocator (const Params& params);


  /**
   * @brief Destructor.  This will free all the Allocator's storage.
   */
  virtual ~ArenaHeapAllocator();


  /// Don't allow copy construction or assignment.
  ArenaHeapAllocator (const ArenaHeapAllocator&) = delete;
  ArenaHeapAllocator& operator= (const ArenaHeapAllocator&) = delete;


  /**
   * @brief Move constructor.
   */
  ArenaHeapAllocator (ArenaHeapAllocator&& other);


  /**
   * @brief Move assignment.
   */
  ArenaHeapAllocator& operator= (ArenaHeapAllocator&& other);


  /**
   * @brief Swap.
   */
  void swap (ArenaHeapAllocator& other);


  /**
   * @brief Allocate a new element.
   *
   * The fast path of this will be completely inlined.
   */
  pointer allocate();


  /**
   * @brief Free an element.
   * @param p The element to be freed.
   *
   * @c clear() will be called on the element at this point,
   * if it has been defined.
   */
  void free (pointer p);


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


  // These are just placeholders --- the iterators are not implemented.
  typedef void iterator;
  typedef void const_iterator;


private:
  /**
   * @brief Call @c clear() for all allocated elements.
   */
  void slowClear();


  /**
   * @brief Add more free elements to the pool, and allocate a new element.
   */
  pointer refill();


  /**
   * @brief Return a reference to the link for an element.
   * @param p The element.
   */
  pointer& link (pointer p) const;

  /// Pointer to the next free element.
  pointer m_freeptr;
};


} // namespace SG


#include "AthAllocators/ArenaHeapAllocator.icc"


#endif // not ATLALLOCATORS_ARENAHEAPALLOCATOR_H
