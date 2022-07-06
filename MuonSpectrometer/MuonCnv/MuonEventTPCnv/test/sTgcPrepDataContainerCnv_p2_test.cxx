/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file MuonEventTPCnv/test/sTgcPrepDataContainerCnv_p2_test.cxx
 * @brief Regression tests, copied from the _p1 version.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepDataContainerCnv_p2.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
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
  assert (p1.localPosition()[1] == p2.localPosition()[1]);
  assert (p1.rdoList() == p2.rdoList());
  assert (p1.localCovariance() == p2.localCovariance());
}


void compare (const Muon::MuonCluster& p1,
              const Muon::MuonCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&>(p1),
           static_cast<const Trk::PrepRawData&>(p2));
  assert (p1.globalPosition() == p2.globalPosition());
}


void compare (const Muon::sTgcPrepData& p1,
              const Muon::sTgcPrepData& p2)
{
  compare (static_cast<const Muon::MuonCluster&>(p1),
           static_cast<const Muon::MuonCluster&>(p2));
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.charge() == p2.charge());
  assert (p1.time() == p2.time());
  assert (p1.getBcBitMap() == p2.getBcBitMap());
}


void compare (const Muon::sTgcPrepDataContainer& p1,
              const Muon::sTgcPrepDataContainer& p2)
{
  Muon::sTgcPrepDataContainer::const_iterator it1 = p1.begin();
  Muon::sTgcPrepDataContainer::const_iterator it1e = p1.end();
  Muon::sTgcPrepDataContainer::const_iterator it2 = p2.begin();
  Muon::sTgcPrepDataContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    const Muon::sTgcPrepDataCollection& coll1 = **it1;
    const Muon::sTgcPrepDataCollection& coll2 = **it2;
    assert (coll1.size() == coll2.size());
    for (size_t j = 0; j < coll1.size(); j++) {
      compare (*coll1[j], *coll2[j]);
    }
    ++it1;
    ++it2;
  }
  assert (it1 == it1e && it2 == it2e);
}


void testit (const Muon::sTgcPrepDataContainer& trans1)
{
  MsgStream log (nullptr, "test");
  Muon::sTgcPrepDataContainerCnv_p2 cnv;
  Muon::sTgcPrepDataContainer_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<Muon::sTgcPrepDataContainer> trans2
    (cnv.createTransient (&pers, log));

  compare (trans1, *trans2);
}


std::unique_ptr<const Muon::sTgcPrepDataContainer>
makeclusts (const MuonGM::MuonDetectorManager& muo_dd)
{
  auto cont = std::make_unique<Muon::sTgcPrepDataContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = std::make_unique<Muon::sTgcPrepDataCollection>(IdentifierHash(hash));
    coll->setIdentifier (muo_dd.stgcIdHelper()->elementID (57, 2, hash));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Identifier clusId = muo_dd.stgcIdHelper()->channelID (57, 2, hash,
                                                            1, 1, 0, 1+i);
      int clusHash = 567 + offs;
      
      Amg::Vector2D locpos (2.5+offs, 1.5+offs);
      std::vector<Identifier> rdoList { clusId };

      Amg::MatrixX cov(1,1);
      cov(0,0) = 101 + offs;

      auto cl = std::make_unique<Muon::sTgcPrepData>
        (clusId,
         clusHash,
         locpos,
         rdoList,
         cov,
         muo_dd.getsTgcReadoutElement (clusId),
         123+offs,
         78+i,
         1+i/2);
      coll->push_back (std::move (cl));
    }
    assert(cont->addCollection (coll.release(), hash));
  }

  return cont;
}


void test1 ATLAS_NOT_THREAD_SAFE (const MuonGM::MuonDetectorManager& muo_dd)
{
  std::cout << "test1\n";

  {
    std::unique_ptr<const Muon::sTgcPrepDataContainer> cont = makeclusts (muo_dd);
    testit (*cont);
  }
  
  Athena_test::Leakcheck check;
  std::unique_ptr<const Muon::sTgcPrepDataContainer> cont = makeclusts (muo_dd);
  testit (*cont);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventTPCnv/MuonEventTPCnv_test.txt", pSvcLoc))
  {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  const MuonGM::MuonDetectorManager& muo_dd = make_dd();

  test1 (muo_dd);
  return 0;
}
