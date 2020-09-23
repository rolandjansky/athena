/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetEventTPCnv/test/SCT_ClusterContainerCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/SCT_ClusterContainerCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "InDetIdentifier/SCT_ID.h"
#include "IdDictParser/IdDictParser.h"
#include "SGTools/TestStore.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "CxxUtils/checker_macros.h"

#include "GaudiKernel/MsgStream.h"

#include <cassert>
#include <iostream>
#include <memory>

void compare (const InDet::SiWidth& p1,
              const InDet::SiWidth& p2)
{
  assert (p1.colRow() == p2.colRow());
  assert (p1.widthPhiRZ() == p2.widthPhiRZ());
}


void compare (const Trk::PrepRawData& p1,
              const Trk::PrepRawData& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.localPosition()[0] == p2.localPosition()[0]);
  assert (p1.rdoList() == p2.rdoList());
  assert (p1.localCovariance() == p2.localCovariance());
}


void compare (const InDet::SiCluster& p1,
              const InDet::SiCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&> (p1),
           static_cast<const Trk::PrepRawData&> (p2));
  compare (p1.width(), p2.width());
  //assert (p1.globalPosition() == p2.globalPosition());
  assert (p1.gangedPixel() == p2.gangedPixel());
  assert (p1.detectorElement() == p2.detectorElement());
}


void compare (const InDet::SCT_Cluster& p1,
              const InDet::SCT_Cluster& p2)
{
  compare (static_cast<const InDet::SiCluster&> (p1),
           static_cast<const InDet::SiCluster&> (p2));
  //assert (p1.hitsInThirdTimeBin() == p2.hitsInThirdTimeBin());
}


void compare (const InDet::SCT_ClusterContainer& p1,
              const InDet::SCT_ClusterContainer& p2)
{
  InDet::SCT_ClusterContainer::const_iterator it1 = p1.begin();
  InDet::SCT_ClusterContainer::const_iterator it1e = p1.end();
  InDet::SCT_ClusterContainer::const_iterator it2 = p2.begin();
  InDet::SCT_ClusterContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    const InDet::SCT_ClusterCollection& coll1 = **it1;
    const InDet::SCT_ClusterCollection& coll2 = **it2;
    assert (coll1.size() == coll2.size());
    for (size_t j = 0; j < coll1.size(); j++) {
      compare (*coll1[j], *coll2[j]);
    }
    ++it1;
    ++it2;
  }
  assert (it1 == it1e && it2 == it2e);
}


void testit (const InDet::SCT_ClusterContainer& trans1, const SCT_ID& sct_id)
{
  MsgStream log (0, "test");
  SCT_ClusterContainerCnv_p2 cnv;
  cnv.setIdHelper(&sct_id);
  cnv.setUseDetectorElement(false);
  InDet::SCT_ClusterContainer_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<InDet::SCT_ClusterContainer> trans2
    (cnv.createTransient (&pers, log));

  compare (trans1, *trans2);
}


std::unique_ptr<const InDet::SCT_ClusterContainer>
makeclusts(const SCT_ID& sct_id)
{
  auto cont = std::make_unique<InDet::SCT_ClusterContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = std::make_unique<InDet::SCT_ClusterCollection>(IdentifierHash(hash));
    coll->setIdentifier (sct_id.wafer_id (hash));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Amg::Vector2D locpos (offs+1.5, offs+2.5);
      std::vector<Identifier> rdoList { Identifier(offs+8000),
                                        Identifier(offs+7000),
                                        Identifier(offs+6456) };
      InDet::SiWidth width (Amg::Vector2D (offs+1, (offs+2) & 0xff),
                            Amg::Vector2D (offs+3.5, offs+4.5));

      Amg::MatrixX cov(2,2);
      for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
          cov(i,j) = 100*(i+1)*(j+1) + offs;

      auto cl = std::make_unique<InDet::SCT_Cluster>
        (Identifier (offs+1234),
         locpos,
         rdoList,
         width,
         nullptr,
         new Amg::MatrixX(cov));
      cl->setHitsInThirdTimeBin (543 + offs);
      cl->setHashAndIndex (hash, i);
      coll->push_back (std::move (cl));
    }
    assert( cont->addCollection (coll.release(), hash).isSuccess() );
  }

  // gcc4.9 doesn't allow returning cont directly here; fixed in 5.2.
  return std::unique_ptr<const InDet::SCT_ClusterContainer> (cont.release());
}


void test1 ATLAS_NOT_THREAD_SAFE (const SCT_ID& sct_id)
{
  std::cout << "test1\n";

  {
    // Do it once without leak checking to get services initialized.
    std::unique_ptr<const InDet::SCT_ClusterContainer> cont = makeclusts(sct_id);
    testit (*cont, sct_id);
  }

  // And again with leak checking.
  Athena_test::Leakcheck check;
  std::unique_ptr<const InDet::SCT_ClusterContainer> cont = makeclusts(sct_id);
  testit (*cont, sct_id);
}


const SCT_ID& make_dd()
{
  auto sct_id = std::make_unique<SCT_ID>();
  const SCT_ID& ret = *sct_id;
  IdDictParser parser;
  parser.register_external_entity ("InnerDetector",
                                   "IdDictInnerDetector.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
  sct_id->initialize_from_dictionary (idd);

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* detStore = 0;
  assert ( svcLoc->service("DetectorStore", detStore).isSuccess() );
  assert ( detStore->record (std::move (sct_id), "SCT_ID") );

  return ret;
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("InDetEventTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  const SCT_ID& sct_id = make_dd();

  test1(sct_id);
  return 0;
}
