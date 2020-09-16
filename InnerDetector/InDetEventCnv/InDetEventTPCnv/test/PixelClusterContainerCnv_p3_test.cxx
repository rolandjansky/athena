/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetEventTPCnv/test/PixelClusterContainerCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/PixelClusterContainerCnv_p3.h"
#include "TestTools/leakcheck.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "InDetIdentifier/PixelID.h"
#include "IdDictParser/IdDictParser.h"
#include "SGTools/TestStore.h"
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
  assert (p1.localPosition() == p2.localPosition());
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


void compare (const InDet::PixelCluster& p1,
              const InDet::PixelCluster& p2)
{
  compare (static_cast<const InDet::SiCluster&> (p1),
           static_cast<const InDet::SiCluster&> (p2));
  assert (p1.omegax() == p2.omegax());
  assert (p1.omegay() == p2.omegay());
  if (p1.totalCharge() == 0)
    assert (p1.totalToT() == p2.totalToT());
  assert (p1.totalCharge() == p2.totalCharge());
  assert (p1.isFake() == p2.isFake());
  assert (p1.isAmbiguous() == p2.isAmbiguous());
  assert (p1.LVL1A() == p2.LVL1A());
  assert (p1.tooBigToBeSplit() == p2.tooBigToBeSplit());
  assert (p1.splitInfoRaw() == p2.splitInfoRaw());
  //assert (p1.totList() == p2.totList());
  assert (p1.chargeList() == p2.chargeList());
}


void compare (const InDet::PixelClusterContainer& p1,
              const InDet::PixelClusterContainer& p2)
{
  InDet::PixelClusterContainer::const_iterator it1 = p1.begin();
  InDet::PixelClusterContainer::const_iterator it1e = p1.end();
  InDet::PixelClusterContainer::const_iterator it2 = p2.begin();
  InDet::PixelClusterContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    const InDet::PixelClusterCollection& coll1 = **it1;
    const InDet::PixelClusterCollection& coll2 = **it2;
    assert (coll1.size() == coll2.size());
    for (size_t j = 0; j < coll1.size(); j++) {
      compare (*coll1[j], *coll2[j]);
    }
    ++it1;
    ++it2;
  }
  assert (it1 == it1e && it2 == it2e);
}


void testit (const InDet::PixelClusterContainer& trans1)
{
  MsgStream log (0, "test");
  PixelClusterContainerCnv_p3 cnv;
  cnv.setUseDetectorElement(false);
  InDet::PixelClusterContainer_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<InDet::PixelClusterContainer> trans2
    (cnv.createTransient (&pers, log));

  compare (trans1, *trans2);
}


std::unique_ptr<const InDet::PixelClusterContainer>
makeclusts()
{
  auto cont = std::make_unique<InDet::PixelClusterContainer>(5);

  for (int hash=2; hash <= 3; hash++) {
    auto coll = std::make_unique<InDet::PixelClusterCollection>(IdentifierHash(hash));

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hash*100;
      Amg::Vector2D locpos (offs+1.5, offs+2.5);
      std::vector<Identifier> rdoList { Identifier(offs+54436),
                                        Identifier(offs+132456),
                                        Identifier(offs+6456) };
      std::vector<int> totList { offs+6, offs+5, offs+4 };
      std::vector<float> chargeList { static_cast<float>(offs+4.5),
                                      static_cast<float>(offs+6.5),
                                      static_cast<float>(offs+1.5),
                                      static_cast<float>(offs+3.5) };
      InDet::SiWidth width (Amg::Vector2D (offs+1, (offs+2) & 0xff),
                            Amg::Vector2D (offs+3.5, offs+4.5));

      Amg::MatrixX cov(2,2);
      for (int i=0; i < 2; i++)
        for (int j=0; j < 2; j++)
          cov(i,j) = 100*(i+1)*(j+1) + offs;

      auto cl = std::make_unique<InDet::PixelCluster>
        (Identifier (offs+1234 + (hash<<17) + 0xc000),
         locpos,
         rdoList,
         13,  // lvl1
         totList,
         chargeList,
         width,
         nullptr,
         new Amg::MatrixX(cov),
         offs+10.5,
         offs+11.5,
         i&1,
         (offs+2.5)/500,
         (offs+3.5)/500);
      cl->setFake(i&2);
      cl->setGangedPixel(i&4);
      cl->setAmbiguous(i&8);
      cl->setHashAndIndex (hash, i);
      coll->push_back (std::move (cl));
    }
    assert( cont->addCollection (coll.release(), hash).isSuccess() );
  }

  // gcc4.9 doesn't allow returning cont directly here; fixed in 5.2.
  return std::unique_ptr<const InDet::PixelClusterContainer> (cont.release());
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";

  {
    // Do it once without leak checking to get services initialized.
    std::unique_ptr<const InDet::PixelClusterContainer> cont = makeclusts();
    testit (*cont);
  }

  // And again with leak checking.
  Athena_test::Leakcheck check;
  std::unique_ptr<const InDet::PixelClusterContainer> cont = makeclusts();
  testit (*cont);
}


void make_dd()
{
  auto pix_id = std::make_unique<PixelID>();
  IdDictParser parser;
  parser.register_external_entity ("InnerDetector",
                                   "IdDictInnerDetector.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
  pix_id->initialize_from_dictionary (idd);

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* sg = 0;
  assert ( svcLoc->service("DetectorStore", sg).isSuccess() );
  assert ( sg->record (std::move (pix_id), "PixelID") );
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("InDetEventTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  make_dd();

  test1();
  return 0;
}
