/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMuonEventTPCnv/test/TileMuFeatureCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TileMuFeatureCnv_p1.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>
#include <sstream>


void compare (const TileMuFeature& p1,
              const TileMuFeature& p2)
{
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.quality() == p2.quality());
  assert (p1.enedep() == p2.enedep());
}


void testit (const TileMuFeature& trans1)
{
  MsgStream log (0, "test");
  TileMuFeatureCnv_p1 cnv;
  TileMuFeature_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileMuFeature trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TileMuFeature trans1 (2.5, 1.5,
                        std::vector<float> {3.5, 4.5, 5.5, 6.5},
                        10.5);
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
