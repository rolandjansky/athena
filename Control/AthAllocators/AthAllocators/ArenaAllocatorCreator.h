// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaAllocatorCreator.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaAllocatorCreator.h
 * @author scott snyder
 * @date May 2007
 * @brief Provide an interface for creating an arena Allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAALLOCATORCREATOR_H
#define ATLALLOCATORS_ARENAALLOCATORCREATOR_H


namespace SG {


class ArenaAllocatorBase;


/**
 * @brief Provide an interface for creating an arena Allocator.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * Objects deriving from this will be stored in @c ArenaAllocatorRegistry,
 * and are used to create allocator instances on demand.
 */
class ArenaAllocatorCreator
{
public:
  /// Destructor.
  virtual ~ArenaAllocatorCreator() {}


  /**
   * @brief Create an allocator instance.
   */
  virtual ArenaAllocatorBase* create() = 0;
};


} // namespace SG


#endif // not ATLALLOCATORS_ARENAALLOCATORCREATOR_H
