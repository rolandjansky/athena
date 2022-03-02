// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file Navigation/NavigationTokenHash.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Allow overriding the default hash function used within
 *        @a NavigationToken.
 */


#ifndef NAVIGATION_NAVIGATIONTOKENHASH_H
#define NAVIGATION_NAVIGATIONTOKENHASH_H


#include <functional>


/**
 * @brief Allow overriding the default hash function used within
 *        @a NavigationToken.
 *
 * @a NavigationToken relies on iterating over a hash table to define
 * the order of the iteration.  Since we use pointers as keys, and
 * pick up the default C++ function for pointers, that means that the
 * order of iteration may not be reproducible from run to run.
 *
 * If this matters, you can define an alternate hash function that
 * depends on the object contents, rather than on the pointer value.
 * You can specify this as the @a HASH template argument to
 * @a NavigationToken; or, to change the default for all uses
 * of the type, you can add an override for @a navigationHash
 * or a specialization for @a NavigationTokenHash.
 */
inline
std::size_t navigationHash (const void* p)
{
  return std::hash<const void*>() (p);
}


template <typename CHILD>
struct NavigationTokenHash
{
  std::size_t operator() (const CHILD* p) const
  {
    return navigationHash (p);
  }
};



#endif // not NAVIGATION_NAVIGATIONTOKENHASH_H
