/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/CscPrepDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/CscPrepDataCnv_p1.h"
#include "MuonEventTPCnv/CscPrepDataContainerCnv_tlp1.h"
#include "TestTools/leakcheck.h"
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


void compare (const Muon::MuonCluster& p1,
              const Muon::MuonCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&>(p1),
           static_cast<const Trk::PrepRawData&>(p2));
  //assert (p1.globalPosition() == p2.globalPosition());
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

void testit (const Muon::CscPrepData& trans1)
{
  MsgStream log (0, "test");
  CscPrepDataCnv_p1 cnv;
  CscPrepDataContainerCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Muon::CscPrepData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::CscPrepData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
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
                            

  Muon::CscPrepData trans1 (Identifier (0x1234),
                            567,
                            locpos,
                            rdoList,
                            new Amg::MatrixX(cov),
                            nullptr,
                            2,
                            3.5,
                            Muon::CscStatusSimple,
                            Muon::CscTimeEarly);
                            
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
