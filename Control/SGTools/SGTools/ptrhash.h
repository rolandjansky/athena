// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ptrhash.h,v 1.1 2008-04-08 16:05:32 ssnyder Exp $
/**
 * @file SGTools/ptrhash.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mon, 2008
 * @brief Improved hash function for pointers.
 */


#ifndef SGTOOLS_PTRHASH_H
#define SGTOOLS_PTRHASH_H


namespace SG {


/**
 * @brief Improved hash function for pointers.
 *
 * The default hash function for pointers used by @c unordered_map and friends
 * is just the result of casting the pointer to an int.
 * The problem is that most pointers one deals with are going
 * to be aligned; if the pointers are to objects obtained through malloc,
 * then on all systems we deal with, at least the lower three bits
 * of the pointer will always be clear.  Since @c unordered_map
 * uses a bucket-hash scheme, this means that only 1/8 of the buckets
 * will be used in such a case, resulting in lower efficiency.
 *
 * Here, we try to avoid this problem.
 */
struct ptrhash
{
  std::size_t operator() (const void* p) const
  {
    std::size_t x = reinterpret_cast<std::size_t> (p);
    return (x>>3) ^ (x&7);
  }
};


} // namespace SG


#endif // not SGTOOLS_PTRHASH_H
