/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/tests/bitmask_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2018
 * @brief Unit tests for bitmask.
 */


#undef NDEBUG
#include "CxxUtils/bitmask.h"
#include <cassert>
#include <iostream>


enum class Bits { None=0, One=1, Two=2, Three=4, Four=8,
                  ATH_BITMASK };


void test1()
{
  std::cout << "test1\n";
  Bits b = Bits::None;
  b |= Bits::Two;
  assert (static_cast<unsigned>(b) == 2);
  b ^= (Bits::One|Bits::Two|Bits::Three);
  assert (static_cast<unsigned>(b) == 5);
  b &= ~Bits::One;
  assert (static_cast<unsigned>(b) == 4);

  assert (static_cast<unsigned> (b & Bits::Three) == 4);
  assert (static_cast<unsigned> (b & Bits::Two) == 0);
  assert (static_cast<unsigned> (b | Bits::One) == 5);
  assert (static_cast<unsigned> (b ^ Bits::One) == 5);
  assert (static_cast<unsigned> (b ^ Bits::Three) == 0);

  assert (!CxxUtils::test (b, Bits::One));
  assert (CxxUtils::test (b, Bits::Three));
  CxxUtils::set (b, Bits::One);
  assert (static_cast<unsigned>(b) == 5);
  assert (CxxUtils::test (b, Bits::One));
  assert (CxxUtils::test (b, Bits::Three));
  CxxUtils::reset (b, Bits::Three);
  assert (static_cast<unsigned>(b) == 1);
  assert (CxxUtils::test (b, Bits::One));
  assert (!CxxUtils::test (b, Bits::Three));
}


int main()
{
  std::cout << "bitmask_test\n";
  test1();
  return 0;
}
