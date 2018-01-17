/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/TgcCoinDataContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataContainerCnv_p1.h"
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


#include "make_dd.icc"


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
  //assert (p1.sub() == p2.sub());
  //assert (p1.inner() == p2.inner());
  //assert (p1.isPositiveDeltaR() == p2.isPositiveDeltaR());
  assert (p1.globalposIn() == p2.globalposIn());
  assert (p1.globalposOut() == p2.globalposOut());
}


void compare (const Muon::TgcCoinDataContainer& p1,
              const Muon::TgcCoinDataContainer& p2)
{
  Muon::TgcCoinDataContainer::const_iterator it1 = p1.begin();
  Muon::TgcCoinDataContainer::const_iterator it1e = p1.end();
  Muon::TgcCoinDataContainer::const_iterator it2 = p2.begin();
  Muon::TgcCoinDataContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    assert (it1->hasData() == it2->hasData());
    if (it1->hasData()) {
      const Muon::TgcCoinDataCollection& coll1 = **it1;
      const Muon::TgcCoinDataCollection& coll2 = **it2;
      assert (coll1.size() == coll2.size());
      for (size_t j = 0; j < coll1.size(); j++) {
        compare (*coll1[j], *coll2[j]);
      }
    }
    ++it1;
    ++it2;
  }
  assert (it1 == it1e && it2 == it2e);
}


void testit (const Muon::TgcCoinDataContainer& trans1)
{
  MsgStream log (0, "test");
  Muon::TgcCoinDataContainerCnv_p1 cnv;
  TgcCoinDataContainerCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Muon::MuonCoinDataContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<Muon::TgcCoinDataContainer> trans2
    (cnv.createTransient (&pers, log));

  compare (trans1, *trans2);
}


std::unique_ptr<const Muon::TgcCoinDataContainer>
makeclusts (const MuonGM::MuonDetectorManager& muo_dd)
{
  auto cont = CxxUtils::make_unique<Muon::TgcCoinDataContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = CxxUtils::make_unique<Muon::TgcCoinDataCollection>(IdentifierHash(hash));
    coll->setIdentifier (muo_dd.tgcIdHelper()->elementID (43, 1, hash));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Identifier clusId1 = muo_dd.tgcIdHelper()->channelID (43, 1, hash,
                                                            1, 1, 2+i);
      Identifier clusId2 = muo_dd.tgcIdHelper()->channelID (43, 1, hash,
                                                            2, 1, 2+i);
      int clusHash = 567 + offs;
      
      Amg::Vector2D locpos1 (2.5, 3.5);
      Amg::Vector2D locpos2 (4.5, 5.5);

      Amg::MatrixX cov(1,1);
      cov(0,0) = 101;

      auto cl = CxxUtils::make_unique<Muon::TgcCoinData>
        (clusId1,
         clusId2,
         clusHash,
         muo_dd.getTgcReadoutElement (clusId1),
         muo_dd.getTgcReadoutElement (clusId2),
         Muon::TgcCoinData::TYPE_TRACKLET,
         false,
         10+offs, // phi
         false,
         true,
         false,
         11+offs, // trackletId
         new Amg::Vector2D (locpos1),
         new Amg::Vector2D (locpos2),
         6.5+offs,
         7.5+offs,
         12+offs, // delta
         13+offs,
         14+offs,
         15+offs,
         16+offs,
         17+offs, // trackletIdStrip
         new Amg::MatrixX(cov),
         true,
         false);

      coll->push_back (std::move (cl));
    }
    cont->addCollection (coll.release(), hash, true);
  }

  // gcc4.9 doesn't allow returning cont directly here; fixed in 5.2.
  return std::unique_ptr<const Muon::TgcCoinDataContainer> (cont.release());
}


void test1 (const MuonGM::MuonDetectorManager& muo_dd)
{
  std::cout << "test1\n";

  {
    std::unique_ptr<const Muon::TgcCoinDataContainer> cont = makeclusts (muo_dd);
    testit (*cont);
  }

  Athena_test::Leakcheck check;
  std::unique_ptr<const Muon::TgcCoinDataContainer> cont = makeclusts (muo_dd);
  testit (*cont);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  const MuonGM::MuonDetectorManager& muo_dd = make_dd();

  test1 (muo_dd);
  return 0;
}
