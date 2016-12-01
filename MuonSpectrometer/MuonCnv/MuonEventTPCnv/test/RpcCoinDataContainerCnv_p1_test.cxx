/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/RpcCoinDataContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonTrigCoinData/RpcCoinDataContainerCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


#include "make_dd.icc"


void compare (const Trk::PrepRawData& p1,
              const Trk::PrepRawData& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.localPosition()[0] == p2.localPosition()[0]);
  assert (p1.localCovariance() == p2.localCovariance());
  assert (p1.rdoList() == p2.rdoList());
}


void compare (const Muon::MuonCluster& p1,
              const Muon::MuonCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&>(p1),
           static_cast<const Trk::PrepRawData&>(p2));
  for (int i = 0; i < 3; i++) {
    assert (Athena_test::isEqual (p1.globalPosition()[i], p2.globalPosition()[i]));
  }
}


void compare (const Muon::RpcPrepData& p1,
              const Muon::RpcPrepData& p2)
{
  compare (static_cast<const Muon::MuonCluster&>(p1),
           static_cast<const Muon::MuonCluster&>(p2));
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.time() == p2.time());
  assert (p1.triggerInfo() == p2.triggerInfo());
  assert (p1.ambiguityFlag() == p2.ambiguityFlag());
}


void compare (const Muon::RpcCoinData& p1,
              const Muon::RpcCoinData& p2)
{
  compare (static_cast<const Muon::RpcPrepData&>(p1),
           static_cast<const Muon::RpcPrepData&>(p2));
  assert (p1.ijk() == p2.ijk());
  assert (p1.threshold() == p2.threshold());
  assert (p1.overlap() == p2.overlap());
  assert (p1.parentCmId() == p2.parentCmId());
  assert (p1.parentPadId() == p2.parentPadId());
  assert (p1.parentSectorId() == p2.parentSectorId());
  assert (p1.lowPtCm() == p2.lowPtCm());
}


void compare (const Muon::RpcCoinDataContainer& p1,
              const Muon::RpcCoinDataContainer& p2)
{
  Muon::RpcCoinDataContainer::const_iterator it1 = p1.begin();
  Muon::RpcCoinDataContainer::const_iterator it1e = p1.end();
  Muon::RpcCoinDataContainer::const_iterator it2 = p2.begin();
  Muon::RpcCoinDataContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    assert (it1->hasData() == it2->hasData());
    if (it1->hasData()) {
      const Muon::RpcCoinDataCollection& coll1 = **it1;
      const Muon::RpcCoinDataCollection& coll2 = **it2;
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


void testit (const Muon::RpcCoinDataContainer& trans1)
{
  MsgStream log (0, "test");
  Muon::RpcCoinDataContainerCnv_p1 cnv;
  Muon::MuonPRD_Container_p2<Muon::RpcCoinData_p1> pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<Muon::RpcCoinDataContainer> trans2
    (cnv.createTransient (&pers, log));

  compare (trans1, *trans2);
}


std::unique_ptr<const Muon::RpcCoinDataContainer>
makeclusts (const MuonGM::MuonDetectorManager& muo_dd)
{
  auto cont = CxxUtils::make_unique<Muon::RpcCoinDataContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = CxxUtils::make_unique<Muon::RpcCoinDataCollection>(IdentifierHash(hash));
    coll->setIdentifier (muo_dd.rpcIdHelper()->elementID (4, 1, hash, 1));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Identifier clusId = muo_dd.rpcIdHelper()->channelID(4, 1, hash,
                                                          1, 1, 2, 1, 0, 1+i);
      int clusHash = 567 + offs;
      
      Amg::Vector2D locpos (2.5, 0);
      std::vector<Identifier> rdoList {
        muo_dd.rpcIdHelper()->channelID(4, 1, hash,
                                        1, 2, 2, 1, 0, 1+i),
        muo_dd.rpcIdHelper()->channelID(4, 1, hash,
                                        1, 3, 2, 1, 0, 1+i),
        muo_dd.rpcIdHelper()->channelID(4, 1, hash,
                                        2, 1, 2, 1, 0, 1+i)
      };

      Amg::MatrixX cov(1,1);
      cov(0,0) = 101;

      auto cl = CxxUtils::make_unique<Muon::RpcCoinData>
        (clusId,
         clusHash,
         locpos,
         rdoList,
         new Amg::MatrixX(cov),
         muo_dd.getRpcReadoutElement (clusId),
         4.5 + offs,
         6 + offs,
         7 + offs,
         8 + offs,
         9 + offs,
         10 + offs,
         11 + offs,
         12 + offs,
         offs&1);

      coll->push_back (std::move (cl));
    }
    cont->addCollection (coll.release(), hash, true);
  }

  // gcc4.9 doesn't allow returning cont directly here; fixed in 5.2.
  return std::unique_ptr<const Muon::RpcCoinDataContainer> (cont.release());
}


void test1 (const MuonGM::MuonDetectorManager& muo_dd)
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  std::unique_ptr<const Muon::RpcCoinDataContainer> cont = makeclusts (muo_dd);
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
