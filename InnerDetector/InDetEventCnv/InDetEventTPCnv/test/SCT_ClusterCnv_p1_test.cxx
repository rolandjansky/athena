/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetEventTPCnv/test/SCT_ClusterCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p1.h"
#include "InDetEventTPCnv/SCT_ClusterContainerCnv_tlp1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


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


void testit (const InDet::SCT_Cluster& trans1)
{
  MsgStream log (0, "test");
  SCT_ClusterCnv_p1 cnv;
  SCT_ClusterContainerCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  InDet::SCT_Cluster_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::SCT_Cluster trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

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
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
