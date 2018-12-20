/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CxxUtils/Root/MurmurHash2.cxx
 * @author scott snyder <snyder@bnl.gov>, derived from code by Austin Appleby
 * @date Feb, 2018
 * @brief Unit tests for MurmurHash2.
 *
 * Derived from <https://github.com/aappleby/smhasher>
 * Original code was released under the MIT license.
 */


#undef NDEBUG
#include "CxxUtils/MurmurHash2.h"
#include <vector>
#include <cassert>
#include <iostream>


typedef void hashfn_t (const void*, int, uint32_t, void*);


void MurmurHash2_test ( const void * key, int len, uint32_t seed, void * out )
{
  *(uint32_t*)out = CxxUtils::MurmurHash2(key,len,seed);
}


template <class HASHTYP>
uint32_t verificationTest (HASHTYP hash (const void*, int, HASHTYP))
{
  uint8_t key[256] = {0};
  HASHTYP hashes[256] = {0};

  for (int i=0; i < 256; i++) {
    key[i] = i;
    hashes[i] = hash (key, i, 256-i);
  }

  union {
    HASHTYP val;
    uint8_t bytes[sizeof(HASHTYP)];
  } fin;
  fin.val = hash (hashes, sizeof(hashes), 0);

  uint32_t verification = (fin.bytes[0] << 0) |
                          (fin.bytes[1] << 8) |
                          (fin.bytes[2] << 16) |
                          (fin.bytes[3] << 24);
  return verification;
}


void test1()
{
  std::cout << "test1\n";
  assert (verificationTest<uint32_t> (CxxUtils::MurmurHash2) == 0x27864c1e);
  assert (verificationTest<uint32_t> (CxxUtils::MurmurHash2A) == 0x7fbd4396);
  assert (verificationTest<uint64_t> (CxxUtils::MurmurHash64A) == 0x1f0d3804);
  assert (verificationTest<uint64_t> (CxxUtils::MurmurHash64B) == 0xdd537c05);
}


int main()
{
  std::cout << "MurmurHash2_test\n";
  test1();
  return 0;
}
