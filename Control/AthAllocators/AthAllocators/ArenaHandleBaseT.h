// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandleBaseT.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHandleBaseT.h
 * @author scott snyder
 * @date May 2007
 * @brief Base class for @c Handle classes, containing parts that
 *        are independent of how the Allocator gets created.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAHANDLEBASET_H
#define ATLALLOCATORS_ARENAHANDLEBASET_H

#include "AthAllocators/ArenaHandleBaseAllocT.h"
#include "AthAllocators/ArenaHeader.h"
#include "boost/iterator/iterator_adaptor.hpp"
#include <cstdlib>


namespace SG {


/**
 * @brief Base class for @c Handle classes, containing parts that
 *        are independent of how the Allocator gets created.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */
template <class T, class ALLOC>
class ArenaHandleBaseT
  : public ArenaHandleBaseAllocT<ALLOC>
{
public:
  /// A pointer to the element type we're allocating.
  typedef T* pointer;

  /// Shorthand for our base class.
  typedef ArenaHandleBaseAllocT<ALLOC> Base;

  /// @c AllocatorCreatorBase concrete derived class for creating
  /// our Allocator.
  typedef typename Base::Creator Creator;


  /**
   * @brief Constructor, passing in an index.
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param index The index of this Handle's Allocator type.
   */
  ArenaHandleBaseT (ArenaHeader* header, size_t index);


  /**
   * @brief Constructor, passing in a creator instance.
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param creator A @c Creator instance that will create an instance
   *                of the Allocator we use.
   *
   * We'll try looking up the allocator name (from @c creator) in the
   * registry to find the proper index.  If it's not found, we'll
   * register @c creator.
   */
  ArenaHandleBaseT (ArenaHeader* header, const Creator& creator);


  // Forward declaration.
  class const_iterator;


  /**
   * @brief Non-const iterator.
   *        It iterates over all unallocated blocks in the current
   *        Allocator (in unspecified order).  It will be at least
   *        a @c forward_iterator.
   *
   *        Note that this is only supported if the underlying Allocator
   *        supports it.  If it does not, you will not be able
   *        to instantiate this type.
   *
   *        This uses @c boost::iterator_adaptor to handle casting
   *        the element pointers to the proper type.
   */
  class iterator
    : public boost::iterator_adaptor<
    iterator,
    typename ALLOC::iterator,
    T,
    boost::forward_traversal_tag>
  {
  public:
    /**
     * @brief Constructor.
     * @param it The base iterator.
     */
    iterator (const typename ALLOC::iterator& it);


    // To allow initializing a @c const_iterator from an @c iterator.
    friend class const_iterator;


  private:
    // Required by @c iterator_adaptor.
    friend class boost::iterator_core_access;


    /**
     * @brief Dereference the iterator.
     */
    typename iterator::reference dereference() const;
  };


  /**
   * @brief Const iterator.
   *        It iterates over all unallocated blocks in the current
   *        Allocator (in unspecified order).  It will be at least
   *        a @c forward_iterator.
   *
   *        Note that this is only supported if the underlying Allocator
   *        supports it.  If it does not, you will not be able
   *        to instantiate this type.
   *
   *        This uses @c boost::iterator_adaptor to handle casting
   *        the element pointers to the proper type.
   */
  class const_iterator
    : public boost::iterator_adaptor<
    const_iterator,
    typename ALLOC::const_iterator,
    T const,
    boost::forward_traversal_tag>
  {

  public:
    /**
     * @brief Constructor.
     * @param it The base iterator.
     */
    const_iterator (const typename ALLOC::const_iterator& it);


    /**
     * @brief Constructor from a non-const iterator.
     * @param it The non-const iterator.
     */
    const_iterator (const iterator& it);


  private:
    // Required by @c iterator_adaptor.
    friend class boost::iterator_core_access;


    /**
     * @brief Dereference the iterator.
     */
    typename const_iterator::reference dereference() const;
  };


  /**
   * @brief Starting iterator.
   *
   * This will iterate over all allocated elements (in unspecified order).
   * It is at least a @c forward_iterator.
   * This may only be instantiated if the underlying Allocator
   * supports iterators.
   */
  iterator begin();


  /**
   * @brief Starting const iterator.
   *
   * This will iterate over all allocated elements (in unspecified order).
   * It is at least a @c forward_iterator.
   * This may only be instantiated if the underlying Allocator
   * supports iterators.
   */
  const_iterator begin() const;


  /**
   * @brief Ending iterator.
   *
   * This may only be instantiated if the underlying Allocator
   * supports iterators.
   */
  iterator end();


  /**
   * @brief Ending const iterator.
   *
   * This may only be instantiated if the underlying Allocator
   * supports iterators.
   */
  const_iterator end() const;


  /**
   * @brief Free an element.
   * @param p The element to be freed.
   *
   * @c clear() will be called on the element at this point,
   * if it has been defined.
   *
   * This may only be instantiated if it's supported
   * by the underlying Allocator.
   */
  void free (pointer p);


  /**
   * @brief Reset pool back to a previous state.
   * @param p The pointer back to which to reset.
   *
   * This will free (a la @c reset) the element @c p and all elements
   * that have been allocated after it from this allocator.
   *
   * This may only be instantiated if it's supported
   * by the underlying Allocator.
   */
  void resetTo (pointer p);
};


} // namespace SG


#include "AthAllocators/ArenaHandleBaseT.icc"


#endif // not ATLALLOCATORS_ARENAHANDLEBASET_H
