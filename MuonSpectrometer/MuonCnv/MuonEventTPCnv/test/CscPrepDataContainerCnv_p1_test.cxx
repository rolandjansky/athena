/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/CscPrepDataContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataContainerCnv_p1.h"
#include "MuonEventTPCnv/CscPrepDataContainerCnv_tlp1.h"
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
  assert (p1.localCovariance()(0,0) == p2.localCovariance()(0,0));
}


void compare (const Muon::MuonCluster& p1,
              const Muon::MuonCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&>(p1),
           static_cast<const Trk::PrepRawData&>(p2));
  assert (p1.globalPosition() == p2.globalPosition());
}


void compare (const Muon::CscPrepData& p1,
              const Muon::CscPrepData& p2)
{
  compare (static_cast<const Muon::MuonCluster&>(p1),
           static_cast<const Muon::MuonCluster&>(p2));
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.charge() == p2.charge());
  assert (p2.time() == p2.time());
  assert (p1.status() == p2.status());
  assert (p1.timeStatus() == p2.timeStatus());
}


void compare (const Muon::CscPrepDataContainer& p1,
              const Muon::CscPrepDataContainer& p2)
{
  Muon::CscPrepDataContainer::const_iterator it1 = p1.begin();
  Muon::CscPrepDataContainer::const_iterator it1e = p1.end();
  Muon::CscPrepDataContainer::const_iterator it2 = p2.begin();
  Muon::CscPrepDataContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    assert (it1->hasData() == it2->hasData());
    if (it1->hasData()) {
      const Muon::CscPrepDataCollection& coll1 = **it1;
      const Muon::CscPrepDataCollection& coll2 = **it2;
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


void testit (const Muon::CscPrepDataContainer& trans1)
{
  MsgStream log (0, "test");
  Muon::CscPrepDataContainerCnv_p1 cnv;
  CscPrepDataContainerCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Muon::MuonPRD_Container_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<Muon::CscPrepDataContainer> trans2
    (cnv.createTransient (&pers, log));

  compare (trans1, *trans2);
}


std::unique_ptr<const Muon::CscPrepDataContainer>
makeclusts (const MuonGM::MuonDetectorManager& muo_dd)
{
  auto cont = CxxUtils::make_unique<Muon::CscPrepDataContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = CxxUtils::make_unique<Muon::CscPrepDataCollection>(IdentifierHash(hash));
    coll->setIdentifier (muo_dd.cscIdHelper()->elementID (50, 1, hash));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Identifier clusId  = muo_dd.cscIdHelper()->channelID (50, 1, hash,
                                                            1, 1, 0, 1+i);
      int clusHash = 567 + offs;
      
      Amg::Vector2D locpos (offs+1.5, 0);
      std::vector<Identifier> rdoList { clusId };

      Amg::MatrixX cov(2,2);
      for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
          cov(i,j) = 100*(i+1)*(j+1) + offs;

      auto cl = CxxUtils::make_unique<Muon::CscPrepData>
        (clusId,
         clusHash,
         locpos,
         rdoList,
         new Amg::MatrixX(cov),
         muo_dd.getCscReadoutElement (clusId),
         2.5 + offs,
         3.5 + offs,
         Muon::CscStatusSimple,
         Muon::CscTimeEarly);
      coll->push_back (std::move (cl));
    }
    cont->addCollection (coll.release(), hash, true);
  }

  // gcc4.9 doesn't allow returning cont directly here; fixed in 5.2.
  return std::unique_ptr<const Muon::CscPrepDataContainer> (cont.release());
}


void test1 (const MuonGM::MuonDetectorManager& muo_dd)
{
  std::cout << "test1\n";

  {
    std::unique_ptr<const Muon::CscPrepDataContainer> cont = makeclusts (muo_dd);
    testit (*cont);
  }

  Athena_test::Leakcheck check;
  std::unique_ptr<const Muon::CscPrepDataContainer> cont = makeclusts (muo_dd);
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
