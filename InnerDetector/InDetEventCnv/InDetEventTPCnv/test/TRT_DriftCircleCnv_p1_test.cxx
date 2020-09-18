/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file InDetEventTPCnv/test/TRT_DriftCircleCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircleCnv_p1.h"
#include "InDetEventTPCnv/TRT_DriftCircleContainerCnv_tlp1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::PrepRawData& p1,
              const Trk::PrepRawData& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.localPosition()[0] == p2.localPosition()[0]);
  assert (p1.rdoList() == p2.rdoList());
  assert (p1.localCovariance()(0,0) == p2.localCovariance()(0,0));
}


void compare (const InDet::TRT_DriftCircle& p1,
              const InDet::TRT_DriftCircle& p2)
{
  compare (static_cast<const Trk::PrepRawData&> (p1),
           static_cast<const Trk::PrepRawData&> (p2));
  assert (p1.getWord() == p2.getWord());
}


void testit (const InDet::TRT_DriftCircle& trans1)
{
  MsgStream log (0, "test");
  TRT_DriftCircleCnv_p1 cnv;
  TRT_DriftCircleContainerCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  InDet::TRT_DriftCircle_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  InDet::TRT_DriftCircle trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D locpos (1.5, 2.5);
  std::vector<Identifier> rdoList { Identifier(5432),
                                    Identifier(5361),
                                    Identifier(6456) };

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  InDet::TRT_DriftCircle trans1 (Identifier (1234),
                                 locpos,
                                 rdoList,
                                 new Amg::MatrixX(cov),
                                 nullptr,
                                 0x0c020100);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
