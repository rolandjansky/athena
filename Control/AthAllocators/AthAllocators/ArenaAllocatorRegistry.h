// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaAllocatorRegistry.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaAllocatorRegistry.h
 * @author scott snyder
 * @date May 2007
 * @brief Registry of allocator factories.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */

#ifndef ATLALLOCATORS_ARENAALLOCATORREGISTRY_H
#define ATLALLOCATORS_ARENAALLOCATORREGISTRY_H


#include <cstdlib>
#include <string>


namespace SG {


class ArenaAllocatorBase;
class ArenaAllocatorCreator;
class ArenaAllocatorRegistryImpl;


/**
 * @brief Registry of allocator factories.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * Each Allocator type that the application uses is registered here;
 * we assign to each one a small integer index.  We can then create an instance
 * of the Allocator given the index.  Allocators have names; we also
 * handle finding the index for an Allocator given the name.
 */
class ArenaAllocatorRegistry
{
public:
  /**
   * @brief Register a new allocator type.
   * @param name The name of the allocator type.  Must not already exist.
   * @param creator The factory object to create instances of this type.
   *                The registry takes ownership of this pointer.
   * @return The new integer index for this allocator type.
   */
  size_t registerCreator (const std::string& name,
                          ArenaAllocatorCreator* creator);


  /**
   * @brief Look up the index for an allocator type name.
   * @param name The name of the allocator type to find.
   * @return The index corresponding to the type, or @c std::string::npos
   *         if it hasn't yet been registered.
   */
  size_t lookup (const std::string& name);


  /**
   * @brief Create a new instance of an allocator.
   * @param i The index of the allocator to create.
   * @return A newly-allocated allocator instance.
   */
  ArenaAllocatorBase* create (size_t i);


  /**
   * @brief Return a pointer to the global @c ArenaAllocatorRegistry instance.
   */
  static ArenaAllocatorRegistry* instance();


private:
  /// The implementation object.
  ArenaAllocatorRegistryImpl* m_impl;

  // Disallow copying.
  ArenaAllocatorRegistry (const ArenaAllocatorRegistry&);
  ArenaAllocatorRegistry& operator= (const ArenaAllocatorRegistry&);

  /// Constructor.  Called only by @c instance.
  ArenaAllocatorRegistry();

  /// Destructor.  Called only by @c instance.
  ~ArenaAllocatorRegistry();

  // Just to avoid compiler warnings.
  friend class ArenaAllocatorRegistryImpl;
};


} // namespace SG


#endif // not ATLALLOCATORS_ARENAALLOCATORREGISTRY_H
