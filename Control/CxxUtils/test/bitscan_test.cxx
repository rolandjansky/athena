/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/test/bitscan_test.cxx
 * @author Frank Winklmeier
 * @brief Regression test for bitscan
 */

#undef NDEBUG
#include <iostream>
#include <vector>
#include <bitset>

// Compile with -DTEST_PORTABLE to test the non-GNU/CLANG version
#ifdef TEST_PORTABLE
#undef __GNUC__
#undef __clang_
#endif
#include "CxxUtils/bitscan.h"

using namespace CxxUtils;

/**
 * On GNU/CLANG this will compare the compiler builtin with the portable algorithm
 */
template <typename T>
int check(const std::vector<T>& v)
{
  int error = 0;
  for (auto t : v) {
    unsigned lz1 = count_leading_zeros(t);
    unsigned lz2 = detail::clz_portable(t);
    unsigned tz1 = count_trailing_zeros(t);
    unsigned tz2 = detail::ctz_portable(t);
    unsigned nz1 = count_ones(t);
    unsigned nz2 = detail::popcount_portable(t);

    bool ok = (lz1==lz2) && (tz1==tz2) && (nz1 == nz2);
    if (!ok) ++error;
    std::cout << std::bitset<sizeof(T)*8>(t) << " "
              << lz1 << "=" << lz2 << " "
              << tz1 << "=" << tz2 << " "
              << nz1 << "=" << nz2
              << (ok ? "" : " ERROR")
              << std::endl;
  }
  return error;
}


int main()
{
  // Some test data for 32 and 64 bit
  std::vector<unsigned> t1 = {0x0, 0x1, 0x80000000, 
                              0xcbaa3f00, 0xff1c2301, 0x1aff04};

  std::vector<unsigned long> t2 = {0x0, 0x1, 0x8000000000000000, 
                                   0x12345678cbaa3f00, 0x10000000, 0x1000000000};

  std::vector<unsigned long long> t3 = {0x0, 0x1, 0x8000000000000000, 
                                        0x12345678cbaa3f00, 0x10000000, 0x1000000000};

  int rc(0);
  rc += check(t1);
  std::cout << std::endl;
  rc += check(t2);
  std::cout << std::endl;
  rc += check(t3);

  return rc;
}
