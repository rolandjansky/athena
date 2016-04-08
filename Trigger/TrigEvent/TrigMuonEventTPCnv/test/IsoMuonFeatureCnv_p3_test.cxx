/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMuonEventTPCnv/test/IsoMuonFeatureCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for IsoMuonFeatureCnv_p3.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_p3.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>
#include <sstream>


void compare (const IsoMuonFeature& p1,
              const IsoMuonFeature& p2)
{
  assert (p1.getsumet01() == p2.getsumet01());
  assert (p1.getsumet02() == p2.getsumet02());
  assert (p1.getsumet03() == p2.getsumet03());
  assert (p1.getsumet04() == p2.getsumet04());
  assert (p1.getsumpt01() == p2.getsumpt01());
  assert (p1.getsumpt02() == p2.getsumpt02());
  assert (p1.getsumpt03() == p2.getsumpt03());
  assert (p1.getsumpt04() == p2.getsumpt04());
  assert (p1.getPtMuID() == p2.getPtMuID());
  assert (p1.getMaxPtID() == p2.getMaxPtID());
  assert (p1.getFlag() == p2.getFlag());
  assert (p1.getRoiIdMu() == p2.getRoiIdMu());
  assert (p1.getPtMu() == p2.getPtMu());
  assert (p1.pt() == p2.pt());
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
}


void testit (const IsoMuonFeature& trans1)
{
  MsgStream log (0, "test");
  IsoMuonFeatureCnv_p3 cnv;
  IsoMuonFeature_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  IsoMuonFeature trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  IsoMuonFeature trans1;
  trans1.setCaloInfos (10000, 10010, 10020, 10030);
  trans1.setTrackInfos (10100, 10110, 10120, 10130, 10140, 10150);
  trans1.setRoiMu (123, 30000, 1.3, 2.0, 234);
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
