// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/AuxDataTraits.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2014
 * @brief Allow customizing how aux data types are treated.
 */


#ifndef ATHCONTAINERS_AUXDATATRAITS_H
#define ATHCONTAINERS_AUXDATATRAITS_H


#include <vector>
#include <cstdlib>


namespace SG {


/**
 * @brief Allow customizing how aux data types are treated.
 *
 * @c T here is the type that the user requests, eg in the template
 * argument of a decorator.
 */
template <class T>
class AuxDataTraits
{
public:
  /// The type the user sees.
  typedef T element_type;

  /// Reference types returned by aux data accessors.
  typedef T& reference_type;
  typedef const T& const_reference_type;

  /// Container type used to store this variable.
  typedef std::vector<T> vector_type;

  /// Pointers to the data within the container.
  typedef typename vector_type::pointer container_pointer_type;
  typedef typename vector_type::const_pointer const_container_pointer_type;

  /// Look up an element in the container by index.
  /// ptr is a pointer to the start of the container's data.
  static reference_type index (void* ptr, size_t ndx)
  {
    return reinterpret_cast<container_pointer_type>(ptr)[ndx];
  }
    
  /// Look up an element in the container by index.
  /// ptr is a pointer to the start of the container's data.
  static const_reference_type index (const void* ptr, size_t ndx)
  {
    return reinterpret_cast<const_container_pointer_type>(ptr)[ndx];
  }
    
};


/**
 * @brief Allow customizing how aux data types are treated.
 *
 * Special case for bool variables.
 *
 * @c T here is the type that the user requests, eg in the template
 * argument of a decorator.
 */
template <>
class AuxDataTraits<bool>
{
public:
  /// The type the user sees.
  typedef bool element_type;

  /// Reference types returned by aux data accessors.
  typedef bool& reference_type;
  typedef const bool& const_reference_type;

  /// Container type used to store this variable.
  typedef std::vector<char> vector_type;

  /// Pointers to the data within the container.
  typedef vector_type::pointer container_pointer_type;
  typedef vector_type::const_pointer const_container_pointer_type;

  /// Look up an element in the container by index.
  /// ptr is a pointer to the start of the container's data.
  static reference_type index (void* ptr, size_t ndx)
  {
    return reinterpret_cast<bool*>(ptr)[ndx];
  }
    
  /// Look up an element in the container by index.
  /// ptr is a pointer to the start of the container's data.
  static const_reference_type index (const void* ptr, size_t ndx)
  {
    return reinterpret_cast<const bool*>(ptr)[ndx];
  }
};


} // namespace SG


#endif // not ATHCONTAINERS_AUXDATATRAITS_H
