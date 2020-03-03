/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/test/aligned_vector_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2020
 * @brief Unit test for aligned_vector.
 */


#undef NDEBUG
#include "CxxUtils/aligned_vector.h"
#include <cassert>
#include <iostream>


template <size_t N>
void testone()
{
  std::vector<CxxUtils::aligned_vector<float, N> > v;
  v.resize (10);
  for (auto& vv : v) {
    vv.resize (10);
    assert ((reinterpret_cast<unsigned long>(vv.data()) & (N-1)) == 0);
  }
}


void test1()
{
  std::cout << "test1\n";
  testone<4>();
  testone<8>();
  testone<16>();
  testone<32>();
  testone<64>();
  testone<128>();
  testone<256>();
  testone<512>();
  testone<1024>();
  testone<2048>();
  testone<4096>();
  testone<8192>();
  testone<16384>();
  testone<32768>();
  testone<65536>();
}


int main()
{
  test1();
  return 0;
}

