/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetEventTPCnv/test/PixelClusterCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p3.h"
#include "TestTools/leakcheck.h"
#include "InDetIdentifier/PixelID.h"
#include "IdDictParser/IdDictParser.h"
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


void testit (const PixelID& pix_id, const InDet::PixelCluster& trans1)
{
  MsgStream log (0, "test");
  PixelClusterCnv_p3 cnv (&pix_id);
  InDet::PixelCluster_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::PixelCluster trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE (const PixelID& pix_id)
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D locpos (1.5, 2.5);
  std::vector<Identifier> rdoList { Identifier(54436),
                                    Identifier(132456),
                                    Identifier(6456) };
  std::vector<int> totList { 6, 5, 4 };
  std::vector<float> chargeList { 4.5, 6.5, 1.5, 3.5 };
  InDet::SiWidth width (Amg::Vector2D (1, 2),
                        Amg::Vector2D (3.5, 4.5));

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  InDet::PixelCluster trans1 (Identifier (1234),
                              locpos,
                              rdoList,
                              13,  // lvl1
                              totList,
                              chargeList,
                              width,
                              nullptr,
                              new Amg::MatrixX(cov),
                              10.5,
                              11.5,
                              true,
                              2.5,
                              3.5);
  trans1.setFake(true);
  trans1.setGangedPixel(true);
  testit (pix_id, trans1);

  InDet::PixelCluster trans2 (Identifier (1234),
                              locpos,
                              rdoList,
                              13,  // lvl1
                              std::vector<int>(),
                              chargeList,
                              width,
                              nullptr,
                              new Amg::MatrixX(cov),
                              10.5,
                              11.5,
                              true,
                              2.5,
                              3.5);
  trans2.setAmbiguous(true);
  testit (pix_id, trans2);
}


std::unique_ptr<PixelID> make_idhelper()
{
  auto pix_id = std::make_unique<PixelID>();
  IdDictParser parser;
  parser.register_external_entity ("InnerDetector",
                                   "IdDictInnerDetector.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
  pix_id->initialize_from_dictionary (idd);
  return pix_id;
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::unique_ptr<PixelID> pix_id = make_idhelper();
  test1(*pix_id);
  return 0;
}
