// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaBase.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaBase.h
 * @author scott snyder
 * @date May 2007
 * @brief Reporting interface for @c Arena, to avoid a dependency
 *        loop with @c ArenaHeader.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */

#ifndef ATLALLOCATORS_ARENABASE_H
#define ATLALLOCATORS_ARENABASE_H


#include <iosfwd>
#include <string>


namespace SG {


/**
 * @brief Reporting interface for @c Arena, to avoid a dependency
 *        loop with @c ArenaHeader.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */
class ArenaBase
{
public:
  /// Destructor.
  virtual ~ArenaBase() {}


  /**
   * @brief Generate a report of the memory in use by this @c Arena.
   * @param os The stream to which to write the report.
   */
  virtual void report (std::ostream& os) const = 0;


  /**
   * @brief Return this @c Arena's name.
   */
  virtual const std::string& name() const = 0;
};


} // namespace SG


#endif // not ATLALLOCATORS_ARENABASE_H

