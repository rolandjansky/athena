// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/ones.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2014
 * @brief Construct a bit mask.
 */


#ifndef CXXUTILS_ONES_H
#define CXXUTILS_ONES_H


namespace CxxUtils {


/**
 * @brief Return a bit mask with the lower @a n bits set.
 */
template <class T>
inline
T ones (unsigned int n)
{
  if (n >= sizeof(T) * 8)
    return ~static_cast<T>(0);
  // cppcheck-suppress shiftTooManyBits
  return (static_cast<T>(1) << n) - 1;
}


} // namespace CxxUtils


#endif // not CXXUTILS_ONES_H
