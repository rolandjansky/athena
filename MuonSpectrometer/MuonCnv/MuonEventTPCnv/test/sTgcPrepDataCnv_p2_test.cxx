/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file MuonEventTPCnv/test/sTgcPrepDataCnv_p2_test.cxx
 * @brief Regression tests, copied from the _p1 version.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/sTgcPrepDataCnv_p2.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::PrepRawData& p1,
              const Trk::PrepRawData& p2)
{
  assert (p1.localPosition()[0] == p2.localPosition()[0]);
  assert (p1.localPosition()[1] == p2.localPosition()[1]);
  assert (p1.localCovariance() == p2.localCovariance());
  assert (p1.rdoList().size() == p2.rdoList().size() );
}


void compare (const Muon::MuonCluster& p1,
              const Muon::MuonCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&>(p1),
           static_cast<const Trk::PrepRawData&>(p2));
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

void testit (const Muon::sTgcPrepData& trans1)
{
  MsgStream log (nullptr, "test");
  sTgcPrepDataCnv_p2 cnv;
  Muon::sTgcPrepData_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::sTgcPrepData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D locpos (2.5, 3.5);

  Amg::MatrixX cov(1,1);
  cov(0,0) = 101;

  std::vector<Identifier> rdoList { Identifier(1274),
                                    Identifier(1234),
                                    Identifier(1178) };

  Muon::sTgcPrepData trans1 (Identifier (1234),
                             IdentifierHash (1234),
                             locpos,
                             rdoList,
                             cov,
                             nullptr,
                             123,
                             78,
                             3);
                            
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
