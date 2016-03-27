// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandle.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHandle.h
 * @author scott snyder
 * @date May 2007
 * @brief User interface for allocating memory.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAHANDLE_H
#define ATLALLOCATORS_ARENAHANDLE_H


#include "AthAllocators/ArenaHandleBaseT.h"
#include "AthAllocators/Arena.h"


namespace SG {


/**
 * @brief User interface for allocating memory.
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
 *
 * This particular Handle implementation does not call element constructors,
 * and calls destructors when elements are freed.  The @c allocate
 * method returns a @c void*, so you're expected to use it in a placement new.
 * Here's an example of how you might create the Handle and allocate
 * memory:
 *
 *@code
 *  SG::ArenaHandle<MyObj, SG::ArenaPoolAllocator> handle;
 *  MyObj* obj = new (handle.allocate()) (...);
 @endcode
 *
 * This associates the Handle with the default @c ArenaHeader.
 * You can then erase all objects allocated though this Handle
 * in the current Allocator with
 *
 *@code
 *  handle.reset();
 @endcode
 *
 * The destructors for the allocated objects will be called at that time.
 *
 * Note that most of the public interface for this class is inherited
 * from the base classes.
 */
template <class T, class ALLOC>
class ArenaHandle
  : public ArenaHandleBaseT<T, ALLOC>
{
public:
  /// Shorthand for our base class.
  typedef ArenaHandleBaseT<T, ALLOC> Base;

  /// Pointer to an element.
  typedef typename Base::pointer pointer;

  /// Iterators over elements.
  /// (May only be instantiated if the underlying Allocator supports them.)
  typedef typename Base::iterator iterator;
  typedef typename Base::const_iterator const_iterator;


  /**
   * @brief Constructor, passing in an index.  (For internal/testing use.)
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param index The index of this Handle's Allocator type.
   */
  ArenaHandle (ArenaHeader* header, size_t index);


  /// The class that initializes the default parameter set.
  typedef typename ALLOC::template initParams<T, false, true> defaultParams_t;


  /**
   * @brief Constructor, passing in an optional parameter set.
   * @param params Parameters to pass to the Allocator.
   */
  ArenaHandle (const typename ALLOC::Params& params =
               defaultParams_t());


  /**
   * @brief Constructor, passing in a Header and an optional parameter set.
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param params Parameters to pass to the Allocator.
   */
  ArenaHandle (ArenaHeader* header,
               const typename ALLOC::Params& params =
               defaultParams_t());


  /**
   * @brief Constructor, passing in an Arena and an optional parameter set.
   * @param arena One Arena in the group which this Handle may reference.
   *               May be null to select the global default.
   * @param params Parameters to pass to the Allocator.
   */
  ArenaHandle (Arena* arena, const typename ALLOC::Params& params =
               defaultParams_t());


  /**
   * @brief Allocate a new element.
   *
   * The element's constructor will not have been called; thus, the memory
   * is returned as a @c void*.
   */
  void* allocate() const;


  /**
   * @brief Internal helper: create a new Allocator instance.
   * @param params The parameters for the Allocator.
   */
  static
  ArenaAllocatorBase* makeAllocator (const typename ALLOC::Params& params);


  // The following methods are inherited:
  //  const ALLOC::Params& params() const;
  //  void reset();
  //  void erase();
  //  void reserve(size_t);
  //  const ArenaAllocatorBase::Stats& stats() const;
  //
  // The following inherited functions may be instantiated only
  // if the Allocator supports them:
  //  iterator begin();
  //  const_iterator begin() const;
  //  iterator end();
  //  const_iterator end() const;
  //  void free (pointer p) const;
  //  void resetTo (pointer p) const;
};


} // namespace SG


#include "AthAllocators/ArenaHandle.icc"


#endif // not ATLALLOCATORS_ARENAHANDLE_H
