// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHandleBaseAllocT.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHandleBaseAllocT.h
 * @author scott snyder
 * @date May 2007
 * @brief Base class for @c Handle classes, containing parts that
 *        depend only on the Allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAHANDLEBASEALLOCT_H
#define ATLALLOCATORS_ARENAHANDLEBASEALLOCT_H


#include "AthAllocators/ArenaHandleBase.h"
#include "AthAllocators/ArenaAllocatorCreator.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"
#include "GaudiKernel/System.h"
#include <string>


namespace SG {


/**
 * @brief Base class for @c Handle classes, containing parts that
 *        depend only on the Allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * This is the part of @c Handle that depends only on the Allocator class
 * (on which it is templated).  This implementation here is responsible
 * for registering the Allocator and handling its possible creation.
 *
 * A @c Handle uses an index to identify the particular Allocator
 * class which it uses; the @c ArenaAllocatorRegistry class maps between
 * these indices and instances of @c ArenaAllocatorCreator, which are
 * capable of creating new @c Allocator instances.
 * An @c ArenaHandleBaseAllocT may be created by either passing
 * in the index directly or by passing in a concrete instance
 * of @c ArenaAllocatorCreator.  In the latter case, we will
 * look up the index, registering the creator if needed.
 */
template <typename ALLOC>
class ArenaHandleBaseAllocT
  : public ArenaHandleBase
{
public:
  /// The @c Allocator we use.
  typedef ALLOC alloc_t;


  /**
   * @brief Concrete ArenaAllocatorCreator class used to create
   *        the Allocator for this handle.
   *
   * There are two members of this class: @c m_makeFunc is the function
   * which actually creates the Allocator, and @c m_params is the
   * parameters structure to pass to the new Allocator.  These will
   * get filled in by @c ArenaAllocatorCreatorInit, which derives
   * from this.
   */
  class Creator
    : public ArenaAllocatorCreator
  {
  public:
    /// Type for @c m_makeFunc --- a function returning a new Allocator
    /// from a parameters structure.
    typedef
      ArenaAllocatorBase* makeFunc_t (const typename ALLOC::Params&);


    /**
     * @brief Constructor.
     * @param hand Dummy to set the template argument type.
     * @param params Allocator parameters.
     *
     * This initializes the @c Creator for creating an Allocator
     * appropriate for the Handle class @c HANDLE.  The @c HANDLE
     * class should have a static function @c makeAllocator
     */
    template <class HANDLE>
    Creator (HANDLE* hand, const typename ALLOC::Params& params);


    /**
     * @brief Create an allocator instance.
     */
    virtual ArenaAllocatorBase* create();


    /**
     * @brief Return the name of the Allocator we create.
     */
    const std::string& name() const;


  protected:
    /// Function that creates an Allocator given a set of parameters.
    makeFunc_t* m_makeFunc;

    /// Set of parameters to use to create our allocator.
    typename ALLOC::Params m_params;
  };


  /**
   * @brief Constructor, passing in an index.
   * @param header The group of Arenas which this Handle may reference.
   *               May be null to select the global default.
   * @param index The index of this Handle's Allocator type.
   */
  ArenaHandleBaseAllocT (ArenaHeader* header, size_t index);


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
  ArenaHandleBaseAllocT (ArenaHeader* header, const Creator& creator);


  /**
   * @brief Return our Allocator's parameters.
   */
  const typename ALLOC::Params& params() const;


protected:
  /**
   * @brief Return our current Allocator.
   */
  ALLOC* allocator() const;

  
private:
  /**
   * @brief Find the index for @c Creator, registering it if needed.
   *
   * We look up in the registry the Allocator name we get from @c creator.
   * If not found, then we register @c creator and return the new index.
   */
  size_t makeIndex (const Creator& creator);
};


} // namespace SG


#include "AthAllocators/ArenaHandleBaseAllocT.icc"


#endif // not ATLALLOCATORS_ARENAHANDLEBASEALLOCT_H
