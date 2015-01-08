/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/PackedContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2014
 * @brief Regression tests for PackedContainer
 */

#undef NDEBUG
#include "AthContainers/PackedContainer.h"
#include <iostream>
#include <cassert>
#include <stdint.h>


void test1()
{
  std::cout << "test1\n";

  SG::PackedContainer<int32_t> pc1;
  assert (pc1.parms().nbits() == 32);
  assert (pc1.parms().isSigned() == 1);
  assert (pc1.parms().isFloat() == 0);

  SG::PackedContainer<uint32_t> pc2;
  assert (pc2.parms().nbits() == 32);
  assert (pc2.parms().isSigned() == 0);
  assert (pc2.parms().isFloat() == 0);

  SG::PackedContainer<uint16_t> pc3;
  assert (pc3.parms().nbits() == 16);
  assert (pc3.parms().isSigned() == 0);
  assert (pc3.parms().isFloat() == 0);

  SG::PackedContainer<int8_t> pc4;
  assert (pc4.parms().nbits() == 8);
  assert (pc4.parms().isSigned() == 1);
  assert (pc4.parms().isFloat() == 0);

  SG::PackedContainer<float> pc5;
  assert (pc5.parms().nbits() == 32);
  assert (pc5.parms().nmantissa() == 23);
  assert (pc5.parms().isSigned() == 1);
  assert (pc5.parms().isFloat() == 1);
  assert (pc5.parms().rounding() == 0);
  assert (pc5.parms().scale() == 0);
  assert (pc5.parms().hasScale() == 0);

  SG::PackedContainer<double> pc6;
  assert (pc6.parms().nbits() == 32);
  assert (pc6.parms().nmantissa() == 23);
  assert (pc6.parms().isSigned() == 1);
  assert (pc6.parms().isFloat() == 1);
  assert (pc6.parms().rounding() == 0);
  assert (pc6.parms().scale() == 0);
  assert (pc6.parms().hasScale() == 0);

  SG::PackedContainer<std::vector<int16_t> > pc7;
  assert (pc7.parms().nbits() == 16);
  assert (pc7.parms().isSigned() == 1);
  assert (pc7.parms().isFloat() == 0);

  SG::PackedContainer<std::vector<std::vector<float> > > pc8;
  assert (pc8.parms().nbits() == 32);
  assert (pc8.parms().isSigned() == 1);
  assert (pc8.parms().isFloat() == 1);

  assert (pc1.setOption ("nbits", 23));
  assert (pc1.parms().nbits() == 23);
  assert (pc1.parms().isSigned() == 1);
  assert (pc1.parms().isFloat() == 0);

  assert (pc5.setOption ("scale", 2.5));
  assert (pc5.parms().scale() == 2.5);
  assert (pc5.parms().hasScale() == 1);
  assert (pc5.setOption ("scale", static_cast<double>(3.5)));
  assert (pc5.parms().scale() == 3.5);
  assert (pc5.parms().hasScale() == 1);

  pc2.setParms (pc1.parms());
  assert (pc2.parms().nbits() == 23);
  assert (pc2.parms().isSigned() == 1);
  assert (pc2.parms().isFloat() == 0);
}


int main()
{
  test1();
  return 0;
}
