/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIG_VSI_HASHTOOLS
#define TRIGTOOLS_TRIG_VSI_HASHTOOLS

/**
*	@file		HashTools.h
* @brief 	Hash functions for std::pair
*/
#include <cstdint>

namespace TrigVSI {

   // The Function to make new hash from some existed hashes
   // from: Boost.ContainerHash https://github.com/boostorg/container_hash/blob/develop/include/boost/container_hash/hash.hpp
   inline void HashCombineImpl(size_t& h, size_t k) noexcept
   {
      constexpr std::uint64_t m = 0xc6a4a7935bd1e995;
      constexpr int r = 47;
      k *= m;
      k ^= k >> r;
      k *= m;
      h ^= k;
      h *= m;
      // Completely arbitrary number, to prevent 0's
      // from hashing to 0.
      h += 0xe6546b64;
   }

   // Combines hashes to make new hash
   template <class Type>
   inline void HashCombine(std::size_t& h, const Type& value) noexcept
   {
      HashCombineImpl(h, std::hash<Type>{}(value));
   }

   // hash func for std::pair
   template<typename T1, typename T2>
   struct PairHash
   {
      size_t operator() (const std::pair<T1,T2>& p) const noexcept
      {
         size_t seed = 0;
         HashCombine(seed, p.first);
         HashCombine(seed, p.second);
         return seed;
      }
   };

} // end of namespace bracket TrigVSI

#endif
