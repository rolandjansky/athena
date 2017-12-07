/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/TgcCoinDataCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p3.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp3.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Muon::TgcCoinData& p1,
              const Muon::TgcCoinData& p2)
{
  assert (p1.channelIdIn() == p2.channelIdIn());
  assert (p1.channelIdOut() == p2.channelIdOut());
  assert (p1.identifyHash() == p2.identifyHash());
  assert (p1.getHashAndIndex().collHash() == p2.getHashAndIndex().collHash());
  assert (p1.getHashAndIndex().objIndex() == p2.getHashAndIndex().objIndex());
  assert (p1.detectorElementIn() == p2.detectorElementIn());
  assert (p1.detectorElementOut() == p2.detectorElementOut());
  assert (p1.type() == p2.type());
  assert (p1.isAside() == p2.isAside());
  assert (p1.phi() == p2.phi());
  assert (p1.isInner() == p2.isInner());
  assert (p1.isForward() == p2.isForward());
  assert (p1.isStrip() == p2.isStrip());
  assert (p1.trackletId() == p2.trackletId());
  assert (p1.trackletIdStrip() == p2.trackletIdStrip());
  assert (p1.posIn() == p2.posIn());
  assert (p1.posOut() == p2.posOut());
  assert (p1.hasErrMat() == p2.hasErrMat());
  if (p1.hasErrMat())
    assert (p1.errMat() == p2.errMat());
  assert (p1.widthIn() == p2.widthIn());
  assert (p1.widthOut() == p2.widthOut());
  assert (p1.delta() == p2.delta());
  assert (p1.roi() == p2.roi());
  assert (p1.pt() == p2.pt());
  assert (p1.veto() == p2.veto());
  assert (p1.sub() == p2.sub());
  assert (p1.inner() == p2.inner());
  assert (p1.isPositiveDeltaR() == p2.isPositiveDeltaR());
  assert (p1.globalposIn() == p2.globalposIn());
  assert (p1.globalposOut() == p2.globalposOut());
}


void testit (const Muon::TgcCoinData& trans1)
{
  MsgStream log (0, "test");
  TgcCoinDataCnv_p3 cnv;
  TgcCoinDataContainerCnv_tlp3 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Muon::TgcCoinData_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::TgcCoinData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D locpos1 (2.5, 3.5);
  Amg::Vector2D locpos2 (4.5, 5.5);

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  Muon::TgcCoinData trans1 (Identifier (1234),
                            Identifier (1235),
                            IdentifierHash (1234),
                            nullptr,
                            nullptr,
                            Muon::TgcCoinData::TYPE_TRACKLET,
                            false,
                            10, // phi
                            false,
                            true,
                            false,
                            11, // trackletId
                            new Amg::Vector2D (locpos1),
                            new Amg::Vector2D (locpos2),
                            6.5,
                            7.5,
                            12, // delta
                            13,
                            14,
                            15,
                            16,
                            17, // trackletIdStrip
                            new Amg::MatrixX(cov),
                            true,
                            true);
                            
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
