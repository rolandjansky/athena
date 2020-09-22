/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetEventTPCnv/test/SCT_ClusterCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p3.h"
#include "TestTools/leakcheck.h"
#include "InDetIdentifier/SCT_ID.h"
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
  assert (p1.hitsInThirdTimeBin() == p2.hitsInThirdTimeBin());
}


void testit (const SCT_ID& sct_id, const InDet::SCT_Cluster& trans1)
{
  MsgStream log (0, "test");
  SCT_ClusterCnv_p3 cnv (&sct_id);
  InDet::SCT_Cluster_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::SCT_Cluster trans2 = cnv.createSCT_Cluster (&pers,
                                                     trans1.identify(),
                                                     nullptr,
                                                     log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE (const SCT_ID& sct_id)
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D locpos (1.5, 2.5);
  std::vector<Identifier> rdoList { Identifier(5432),
                                    Identifier(5361),
                                    Identifier(6456) };
  InDet::SiWidth width (Amg::Vector2D (1, 2),
                        Amg::Vector2D (3.5, 4.5));

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  InDet::SCT_Cluster trans1 (Identifier (1234),
                             locpos,
                             rdoList,
                             width,
                             nullptr,
                             new Amg::MatrixX(cov));
  trans1.setHitsInThirdTimeBin (543);
  testit (sct_id, trans1);
}


std::unique_ptr<SCT_ID> make_idhelper()
{
  auto sct_id = std::make_unique<SCT_ID>();
  IdDictParser parser;
  parser.register_external_entity ("InnerDetector",
                                   "IdDictInnerDetector.xml");
  IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
  sct_id->initialize_from_dictionary (idd);
  return sct_id;
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::unique_ptr<SCT_ID> sct_id = make_idhelper();
  test1(*sct_id);
  return 0;
}
