/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MdtPrepDataContainerCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataContainerCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/make_unique.h"
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


void compare (const Muon::MdtPrepData& p1,
              const Muon::MdtPrepData& p2)
{
  compare (static_cast<const Trk::PrepRawData&> (p1),
           static_cast<const Trk::PrepRawData&> (p2));
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.tdc() == p2.tdc());
  assert (p1.adc() == p2.adc());
  assert (p1.status() == p2.status());
}



void compare (const Muon::MdtPrepDataContainer& p1,
              const Muon::MdtPrepDataContainer& p2)
{
  Muon::MdtPrepDataContainer::const_iterator it1 = p1.begin();
  Muon::MdtPrepDataContainer::const_iterator it1e = p1.end();
  Muon::MdtPrepDataContainer::const_iterator it2 = p2.begin();
  Muon::MdtPrepDataContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    assert (it1->hasData() == it2->hasData());
    if (it1->hasData()) {
      const Muon::MdtPrepDataCollection& coll1 = **it1;
      const Muon::MdtPrepDataCollection& coll2 = **it2;
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


void testit (const Muon::MdtPrepDataContainer& trans1)
{
  MsgStream log (0, "test");
  Muon::MdtPrepDataContainerCnv_p2 cnv;
  Muon::MdtPrepDataContainer_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<Muon::MdtPrepDataContainer> trans2
    (cnv.createTransient (&pers, log));

  compare (trans1, *trans2);
}


std::unique_ptr<const Muon::MdtPrepDataContainer>
makeclusts (const MuonGM::MuonDetectorManager& muo_dd)
{
  auto cont = CxxUtils::make_unique<Muon::MdtPrepDataContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = CxxUtils::make_unique<Muon::MdtPrepDataCollection>(IdentifierHash(hash));
    coll->setIdentifier (muo_dd.mdtIdHelper()->elementID (0, 1, hash));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Identifier clusId = muo_dd.mdtIdHelper()->channelID(0, 1, hash, 1, 1, i+1);
      int clusHash = 567 + offs;
      
      Amg::Vector2D driftRadius (offs + 2.5, offs + 3.5);
      std::vector<Identifier> rdoList { clusId };

      Amg::MatrixX cov(1,1);
      cov(0,0) = 101 + offs;

      auto cl = CxxUtils::make_unique<Muon::MdtPrepData>
        (clusId,
         clusHash,
         driftRadius,
         CxxUtils::make_unique<Amg::MatrixX>(cov),
         std::vector<Identifier> (rdoList),
         muo_dd.getMdtReadoutElement (clusId),
         4.5 + offs,
         5.5 + offs,
         Muon::MdtStatusDriftTime);
      coll->push_back (std::move (cl));
    }
    cont->addCollection (coll.release(), hash, true);
  }

  // gcc4.9 doesn't allow returning cont directly here; fixed in 5.2.
  return std::unique_ptr<const Muon::MdtPrepDataContainer> (cont.release());
}


void test1 (const MuonGM::MuonDetectorManager& muo_dd)
{
  std::cout << "test1\n";

  {
    std::unique_ptr<const Muon::MdtPrepDataContainer> cont = makeclusts (muo_dd);
    testit (*cont);
  }

  Athena_test::Leakcheck check;
  std::unique_ptr<const Muon::MdtPrepDataContainer> cont = makeclusts (muo_dd);
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
