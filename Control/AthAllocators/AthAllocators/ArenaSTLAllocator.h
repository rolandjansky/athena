// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaSTLAllocator.h 470825 2011-11-25 23:20:57Z ssnyder $
/**
 * @file AthAllocators/ArenaSTLAllocator.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief 
 */


#ifndef ATLALLOCATORS_ARENASTLALLOCATOR_H
#define ATLALLOCATORS_ARENASTLALLOCATOR_H


#include <string>
#include <cstdlib>


namespace SG {


template <class BASE>
class ArenaSTLAllocator
  : public BASE
{
public:
  /// Standard STL allocator typedefs.
  typedef typename BASE::value_type value_type;
  typedef value_type*               pointer;
  typedef const value_type*         const_pointer;
  typedef value_type&               reference;
  typedef const value_type&         const_reference;
  typedef size_t                    size_type;
  typedef std::ptrdiff_t            difference_type;



  /// Standard STL allocator rebinder.
  template <class U> struct rebind {
    typedef ArenaSTLAllocator<U> other;
  };


  /**
   * @brief Default constructor.
   * @param nblock Value to set in the parameters structure for the
   *               number of elements to allocate per block.
   * @param name   Value to use as the base for the allocator names.
   */
  ArenaSTLAllocator (size_t nblock = 1000,
                     const std::string& name = "");


  /**
   * @brief Constructor from another @c ArenaSTLAllocator.
   *
   * The new STL allocator will reference the same set of underlying
   * Arena allocators as the old one.
   */
  ArenaSTLAllocator (const ArenaSTLAllocator& a);


  /**
   * @brief Constructor from another @c ArenaSTLAllocator.
   *
   * The new STL allocator will reference the same set of underlying
   * Arena allocators as the old one.
   */
  template <class U>
  ArenaSTLAllocator (const ArenaSTLAllocator<U>& a);

  // We don't bother to supply a more general constructor --- shouldn't
  // be needed.

};


} // namespace SG



#include "AthAllocators/ArenaSTLAllocator.icc"

#endif // not ATLALLOCATORS_ARENASTLALLOCATOR_H
