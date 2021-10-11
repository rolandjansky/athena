/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkDetDescrTPCnv/test/BinUtilityCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkDetDescrUtils/BinUtilityCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>



using Athena_test::isEqual;


void compare (const Trk::BinningData& b1, const Trk::BinningData& b2)
{
  assert (b1.type == b2.type);
  assert (b1.option == b2.option);
  assert (b1.binvalue == b2.binvalue);
  assert (b1.bins == b2.bins);
  assert (b1.min == b2.min);
  assert (b1.max == b2.max);
  assert (b1.step == b2.step);
  assert (b1.subStep == b2.subStep);
  assert (b1.boundaries == b2.boundaries);
  assert (b1.refphi == b2.refphi);
  assert (b1.hbounds == b2.hbounds);
}


void compare (const Trk::BinUtility& b1, const Trk::BinUtility& b2)
{
  size_t sz = b1.binningData().size();
  assert (b2.binningData().size() == sz);
  for (size_t i = 0; i < sz; i++)
    compare (b1.binningData()[i], b2.binningData()[i]);
}


void testit (const Trk::BinUtility& trans1)
{
  BinUtilityCnv_p1 cnv;
  Trk::BinUtility_p1 pers;
  MsgStream log (nullptr, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::BinUtility trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  Trk::BinUtility bin1 (10, 0, 10);
  testit (bin1);

  Trk::BinUtility bin2 (10, 0.1, 0, 10);
  testit (bin2);

  std::vector<float> bvalues {0.1, 0.2, 0.3, 0.4};
  Trk::BinUtility bin3 (bvalues);
  testit (bin3);

  std::vector<std::pair<int, float> > bvalues2 {{1,0.1}, {2,0.2}, {3,0.3}};
  Trk::BinUtility bin4 (0.5, bvalues2);
  testit (bin4);
}


int main()
{
  test1();
  return 0;
}
