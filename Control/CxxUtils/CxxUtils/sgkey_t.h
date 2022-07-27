// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/sgkey_t.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Define the type used for hashed StoreGate key+CLID pairs.
 *
 * These are managed by @c IProxyDict.  This definition was broken
 * out from there due to dependency issues.  This would more naturally
 * be in AthenaKernel, but we put it in CxxUtils so that it can be used
 * by xAOD standanone code as well.
 */


#ifndef CXXUTILS_SGKEY_T_H
#define CXXUTILS_SGKEY_T_H


#include <cstdint>
#include <unordered_map>
#include <unordered_set>


namespace SG {


/// Type used for hashed StoreGate key+CLID pairs.
typedef uint32_t sgkey_t;


/**
 * @brief Compare two sgkeys for equality.
 */
inline
constexpr bool sgkeyEqual (const sgkey_t a, const sgkey_t b)
{
  return a == b;
}


/**
 * @brief Compare two sgkeys for ordering.
 */
inline
constexpr bool sgkeyLess (const sgkey_t a, const sgkey_t b)
{
  return a < b;
}


/**
 * @brief Convert a sgkey to the 32-bit form used for older data.
 */
inline
constexpr uint32_t sgkeyShort (const sgkey_t k)
{
  return k;
}


/// A map using sgkey_t as a key.
template <class T>
using SGKeyMap = std::unordered_map<sgkey_t, T>;


/// A set of sgkey_t values.
using SGKeySet = std::unordered_set<sgkey_t>;


} // namespace SG


#endif // not CXXUTILS_SGKEY_T_H
