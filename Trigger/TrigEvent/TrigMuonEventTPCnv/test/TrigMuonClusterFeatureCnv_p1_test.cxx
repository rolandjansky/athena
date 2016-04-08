/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMuonEventTPCnv/test/TrigMuonClusterFeatureCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigMuonClusterFeatureCnv_p1.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigMuonClusterFeature& p1,
              const TrigMuonClusterFeature& p2)
{
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.getNRoi() == p2.getNRoi());
  assert (p1.getNTRK() == p2.getNTRK());
  assert (p1.getNJet() == p2.getNJet());
}


void testit (const TrigMuonClusterFeature& trans1)
{
  MsgStream log (0, "test");
  TrigMuonClusterFeatureCnv_p1 cnv;
  TrigMuonClusterFeature_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMuonClusterFeature trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigMuonClusterFeature trans1 (2.5, 1.5, 3, 4, 5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
