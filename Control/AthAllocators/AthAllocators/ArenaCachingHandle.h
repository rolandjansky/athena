// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaCachingHandle.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaCachingHandle.h
 * @author scott snyder
 * @date May 2007
 * @brief User interface for allocating memory that caches constructed objects.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENACACHINGHANDLE_H
#define ATLALLOCATORS_ARENACACHINGHANDLE_H


#include "AthAllocators/ArenaHandleBaseT.h"
#include "AthAllocators/Arena.h"


namespace SG {


/**
 * @brief User interface for allocating memory that caches constructed objects.
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
 * This particular Handle implementation calls the element constructor
 * only when the memory is first allocated by the system, and calls
 * the destructor only when the memory is returned to the system.
 * The @c allocate method thus returns an already-initialized pointer
 * to the element.  The element's default constructor must work
 * for this.
 *
 * Note that it's possible to set up an additional @c clear method
 * that's called when the element is freed; see @c ArenaAllocatorBase.
 * 
 * Here's an example of how you might create the Handle and allocate
 * memory:
 *
 *@code
 *  SG::ArenaCachingHandle<MyObj, SG::ArenaPoolAllocator> handle;
 *  MyObj* obj = handle.allocate();
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
 * Note that most of the public interface for this class is inherited
 * from the base classes.
 *
 * Be aware that a handle holds a lock on the underlying allocator.
 * Therefore, if you try to create two handle instances referencing
 * the same allocator (i.e, same type and same thread), you'll get a deadlock.
 */
template <class T, class ALLOC>
class ArenaCachingHandle
  : public ArenaHandleBaseT<T, ALLOC>
{
public:
  /// Shorthand for our base class.
  typedef ArenaHandleBaseT<T, ALLOC> Base;

  /// Pointer to an element.
  typedef T* pointer;

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
  ArenaCachingHandle (ArenaHeader* header, size_t index);


  /// The class that initializes the default parameter set.
  typedef typename ALLOC::template initParams<T, false> defaultParams_t;


  /**
   * @brief Constructor, passing in an optional parameter set.
   * @param params Parameters to pass to the Allocator,
   *               or nullptr to use the defaults.
   */
  ArenaCachingHandle (const typename ALLOC::Params* params = nullptr);



  /**
   * @brief Constructor, passing in a Header and an optional parameter set.
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param params Parameters to pass to the Allocator,
   *               or nullptr to use the defaults.
   */
  ArenaCachingHandle (ArenaHeader* header,
                      const typename ALLOC::Params* params = nullptr);


  /**
   * @brief Constructor, passing in a Header, context, and an optional parameter set.
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param ctx Event context identifying the event slot.
   * @param params Parameters to pass to the Allocator,
   *               or nullptr to use the defaults.
   */
  ArenaCachingHandle (ArenaHeader* header,
                      const EventContext& ctx,
                      const typename ALLOC::Params* params = nullptr);


  /**
   * @brief Constructor, passing in an Arena and an optional parameter set.
   * @param arena The Arena in which to create the Allocator.
   * @param params Parameters to pass to the Allocator,
   *               or nullptr to use the defaults.
   */
  ArenaCachingHandle (Arena* arena, const typename ALLOC::Params* params = nullptr);


  /**
   * @brief Allocate a new element.
   *
   * This returns an already-initialized element.
   */
  pointer allocate();


  /**
   * @brief Internal helper: create a new Allocator instance.
   * @param params The parameters for the Allocator.
   */
  static
  std::unique_ptr<ArenaAllocatorBase>
  makeAllocator (const typename ALLOC::Params& params);


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


} //  namespace SG



#include "AthAllocators/ArenaCachingHandle.icc"



#endif // not ATLALLOCATORS_ARENACACHINGHANDLE_H
