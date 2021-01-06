// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Public domain; see below.
 */
/**
 * @file CxxUtils/MurmurHash2.h
 * @author Austin Appleby, adapted for ATLAS by scott snyder <snyder@bnl.gov>
 * @date Feb, 2018
 * @brief Implementation of MurmurHash2.
 *
 * From <https://github.com/aappleby/smhasher>
 */


#ifndef CXXUTILS_MURMURHASH2_H
#define CXXUTILS_MURMURHASH2_H


//-----------------------------------------------------------------------------
// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

#include <cstdint>


#define MurmurHash_mmix(h,k) { k *= M; k ^= k >> R; k *= M; h *= M; h ^= k; }


namespace CxxUtils {


uint32_t MurmurHash2        ( const void * key, int len, uint32_t seed );
uint64_t MurmurHash64A      ( const void * key, int len, uint64_t seed );
uint64_t MurmurHash64B      ( const void * key, int len, uint64_t seed );
uint32_t MurmurHash2A       ( const void * key, int len, uint32_t seed );
uint32_t MurmurHashNeutral2 ( const void * key, int len, uint32_t seed );
uint32_t MurmurHashAligned2 ( const void * key, int len, uint32_t seed );


//-----------------------------------------------------------------------------
// CMurmurHash2A, by Austin Appleby

// This is a sample implementation of MurmurHash2A designed to work 
// incrementally.

// Usage - 

// CMurmurHash2A hasher
// hasher.Begin(seed);
// hasher.Add(data1,size1);
// hasher.Add(data2,size2);
// ...
// hasher.Add(dataN,sizeN);
// uint32_t hash = hasher.End()

class CMurmurHash2A
{
public:

  void Begin ( uint32_t seed = 0 )
  {
    m_hash  = seed;
    m_tail  = 0;
    m_count = 0;
    m_size  = 0;
  }

  void Add ( const unsigned char * data, int len )
  {
    m_size += len;

    MixTail(data,len);

    while(len >= 4)
    {
      uint32_t k = *(uint32_t*)data;

      MurmurHash_mmix(m_hash,k);

      data += 4;
      len -= 4;
    }

    MixTail(data,len);
  }

  uint32_t End ( void )
  {
    MurmurHash_mmix(m_hash,m_tail);
    MurmurHash_mmix(m_hash,m_size);

    m_hash ^= m_hash >> 13;
    m_hash *= M;
    m_hash ^= m_hash >> 15;

    return m_hash;
  }

private:

  static constexpr uint32_t M = 0x5bd1e995;
  static constexpr int R = 24;

  void MixTail ( const unsigned char * & data, int & len )
  {
    while( len && ((len<4) || m_count) )
    {
      m_tail |= (*data++) << (m_count * 8);

      m_count++;
      len--;

      if(m_count == 4)
      {
        MurmurHash_mmix(m_hash,m_tail);
        m_tail = 0;
        m_count = 0;
      }
    }
  }

  uint32_t m_hash;
  uint32_t m_tail;
  uint32_t m_count;
  uint32_t m_size;
};


} // namespace CxxUtils


#endif // not CXXUTILS_MURMURHASH2_H
