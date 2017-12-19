// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/DVLCast.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief casting operations for @c DataVector/@c DataList.
 */


#ifndef ATHCONTAINERS_TOOLS_DVLCAST_H
#define ATHCONTAINERS_TOOLS_DVLCAST_H


#include <limits>
#include <limits.h>
#include <typeinfo>
#include <cstdlib>
#include <cstddef>


namespace DataModel_detail {


#ifdef __GCCXML__
// Dummy version for reflex.
template <class DVL>
struct DVLCast
{
  typedef typename DVL::base_value_type T;
  template <class U> static T* cast (U* b);
  template <class U> static const T* cast (const U* b);
};
#else
/**
 * @brief casting operations for @c DataVector/@c DataList.
 *
 * When inheritance is used with @c DataVector/List, we need to be able
 * to cast from the pointers actually stored in the container to the
 * @c T* pointers which the derived @c DataVector/List uses.  We want
 * this to use a @c static_cast if at all possible, and @c dynamic_cast
 * otherwise.
 *
 * This is handled by this template.  The operation
 *
 *@code
 *   DVLCast<DVL>::cast (p)
 @endcode
 *
 * will cast the pointer @c p to @c DVL's T* type.  This is done
 * either by @c static_cast or @c dynamic_cast, depending on the value
 * of @c DVL::has_virtual.
 */
template <class DVL, bool has_virtual=DVL::has_virtual> struct DVLCast {};


/**
 * @brief casting operations for @c DataVector/List, @c static_cast version.
 */
template <class DVL>
struct DVLCast<DVL, false>
{
  typedef typename DVL::base_value_type T;


  /**
   * @brief Cast @a b to a @c T*.
   * @param b Pointer to cast.
   */
  template <class U>
  static T* cast (U* b)
  {
    return static_cast<T*> (b);
  }


  /**
   * @brief Cast @a b to a @c const T*.
   * @param b Pointer to cast.
   */
  template <class U>
  static const T* cast (const U* b)
  {
    return static_cast<const T*> (b);
  }
};


/**
 * @brief casting operations for @c DataVector/List, @c dynamic_cast version.
 */
template <class DVL>
struct DVLCast<DVL, true>
{
  typedef typename DVL::base_value_type T;


  /**
   * @brief Find the offset of @c T within @c U.
   */
  template <class U>
  static int find_offset (U* b)
  {
    T* ret = dynamic_cast<T*> (b);
    int offs = reinterpret_cast<char*>(ret) - reinterpret_cast<char*>(b);
    return offs;
  }


  /**
   * @brief Cast @a b to a @c T*.
   * @param b Pointer to cast.
   */
  template <class U>
  static T* cast (U* b)
  {
    // The common case will be for the dynamic type of b to be T.
    // So test to avoid a dynamic_cast in that case.
    // The extra test shouldn't add significant overhead
    // to the case where we really need to run dynamic_cast.
    if (!b)
      return 0;
    if (typeid(*b) == typeid(T)) {
      static const ptrdiff_t offs = find_offset (b);
      return reinterpret_cast<T*> (reinterpret_cast<char*>(b) + offs);
    }
    else
      return dynamic_cast<T*> (b);
  }


  /**
   * @brief Cast @a b to a @c const T*.
   * @param b Pointer to cast.
   */
  template <class U>
  static const T* cast (const U* b)
  {
    // See above.
    if (!b)
      return 0;
    if (typeid(*b) == typeid(T)) {
      static ptrdiff_t offs = LONG_MAX;
      if (offs == LONG_MAX) {
        T* ret = dynamic_cast<const T*> (b);
        offs = reinterpret_cast<char*>(ret) - reinterpret_cast<char*>(b);
        return ret;
      }
      return reinterpret_cast<const T*> (reinterpret_cast<char*>(b) + offs);
    }
    else
      return dynamic_cast<const T*> (b);
  }
};
#endif // not __GCCXML__


} // namespace DataModel_detail


#endif // not ATHCONTAINERS_TOOLS_DVLCAST_H
