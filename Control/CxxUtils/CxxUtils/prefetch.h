// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/prefetch.h
 * @author scott snyder <snyder@bnl.gov>, after code from A. Salnikov
 * @date Dec, 2013
 * @brief Functions to prefetch blocks of memory.
 */


#ifndef CXXUTILS_PREFETCH_H
#define CXXUTILS_PREFETCH_H


#include <cstdlib>


// Macro implementing prefetch on an address.
// Set to a no-op if we don't know how to implement it for this
// compiler / architecture.
// One can also predefine CXXUTILS_PREFETCH_ADDRESS for testing purposes.
#ifndef CXXUTILS_PREFETCH_ADDRESS
# ifdef __GNUC__
// gcc provides special builtin
#  define CXXUTILS_PREFETCH_ADDRESS(ADDR)  __builtin_prefetch(ADDR)
# else
#  define CXXUTILS_PREFETCH_ADDRESS(ADDR) do {} while(0) // no-op
# endif
#endif


namespace CxxUtils {


/**
 *  While it is possible to determine cache line size at run time (e.g. using
 *  sysconf(_SC_LEVEL1_DCACHE_LINESIZE) on Linux or sysctlbyname("hw.cachelinesize", ...)
 *  on Apple systems) that approach creates more problems: the location
 *  that stores cache line size will probably be out of cache when one needs
 *  to know it. This is why we use a compile time constant for cache line size.
 *  Cache line size is 64 for the current generation of Intel/AMD CPUs.
 *
 *  If an object spans multiple cache lines then prefetching whole
 *  object should be done by prefetching at least one address from
 *  each of the cache lines that object occupies. How many lines are
 *  needed depends on three things: object size, cache line size, and
 *  object starting address.  If CacheLineSize is lower than the
 *  actual line size then more prefetches than necessary will be
 *  generated (2, 4, or more per cache line if cache line size is
 *  power of 2). This may be somewhat wasteful so this number may need
 *  to be adjusted in the far future when _all_ CPUs have wider cache
 *  lines.
 */
enum { CacheLineSize = 64 };


/**
 *  @brief  Generic prefetch method
 *  @param[in] address   memory location to prefetch
 *
 *  This is generic method that does not have any extra behavior. It
 *  only prefetches the whole cache line which contains the specified
 *  memory location. It does not incur any additional overhead and may
 *  be the most efficient method for small objects which have a high
 *  chance of being on just a single cache line.
 */
inline
void prefetchOne (const void* address)
{
  CXXUTILS_PREFETCH_ADDRESS(address);
}


/**
 *  @brief  Prefetch an N-byte block of memory.
 *  @param[in] ptr  Starting address of the block.
 */
template <size_t N>
inline
void prefetchN (const void* ptr)
{
  const char* pp = reinterpret_cast<const char*> (ptr);

  // The compiler will unroll this loop for small N.
  for (size_t i = 0; i < N; i += CacheLineSize)
    prefetchOne (pp + i);

  // Issue a prefetch for the last byte as well, in case it crosses
  // cache lines.
  prefetchOne (pp + N - 1);
}


/**
 *  @brief  Generic prefetch of the object of specific types (sizes).
 *  @param[in] address   memory location to prefetch
 *
 *  This method will prefetch as many cache lines as needed to store the object
 *  of the specified type in memory. Type is specified as a template argument.
 */
template <typename T>
inline
void prefetchObj(const T* ptr)
{
  prefetchN<sizeof(T)> (ptr);
}

/**
 * @brief Prefetch next object in sequence.
 * @param iter Current iteration position.
 * @param endIter End of the sequence.
 *
 * @c Iter is a @c ForwardIterator over pointers.
 *
 *  Prefetches next object in a collection of pointers. Accepts two
 *  iterators: the first iterator is the current iteration position,
 *  and the second iterator is the end iterator for the whole
 *  sequence.  The first iterator must not be equal to the end
 *  iterator (this is not checked).  This increments the iterator and,
 *  if not equal to the end, prefetches the complete object referenced
 *  by the pointer.
 */
template <typename Iter>
inline
void prefetchNext (Iter iter, Iter endIter)
{
  if (++iter != endIter)
    prefetchObj(*iter);
}


/**
 * @brief Prefetch two objects.
 * @param iter Current iteration position.
 * @param endIter End of the sequence.
 *
 * @c Iter is a @c ForwardIterator over pointers.
 *
 * Prefetches the current and next objects in a collection of
 * pointers.  Accepts two iterators: the first iterator is the current
 * iteration position, and the second is the end iterator for the
 * whole sequence.  If the first iterator is not equal to end the
 * iterator, the object to which it points is prefetched.  Then the
 * iterator is incremented and, if not equal to the end iterator, the
 * next objects is also prefetched.
 */
template <typename Iter>
inline
void prefetchTwo (Iter iter, Iter endIter)
{
  if (iter != endIter) {
    prefetchObj(*iter);
    if (++iter != endIter) {
      prefetchObj(*iter);
    }
  }
}


} // namespace CxxUtils


#endif // not CXXUTILS_PREFETCH_H
