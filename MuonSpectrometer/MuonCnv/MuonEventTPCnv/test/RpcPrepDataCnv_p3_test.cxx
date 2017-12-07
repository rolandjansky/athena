/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/RpcPrepDataCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/RpcPrepDataCnv_p3.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::PrepRawData& p1,
              const Trk::PrepRawData& p2)
{
  //assert (p1.identify() == p2.identify());
  assert (p1.localPosition()[0] == p2.localPosition()[0]);
  assert (p1.localCovariance() == p2.localCovariance());
  assert (p2.rdoList().size() == 1);
  assert (p2.rdoList()[0] == p2.identify());
}


void compare (const Muon::MuonCluster& p1,
              const Muon::MuonCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&>(p1),
           static_cast<const Trk::PrepRawData&>(p2));
  //assert (p1.globalPosition() == p2.globalPosition());
}


void compare (const Muon::RpcPrepData& p1,
              const Muon::RpcPrepData& p2)
{
  compare (static_cast<const Muon::MuonCluster&>(p1),
           static_cast<const Muon::MuonCluster&>(p2));
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.time() == p2.time());
  assert (p1.triggerInfo() == p2.triggerInfo());
  assert (p1.ambiguityFlag() == p2.ambiguityFlag());
  //assert (p1.globalPosition() == p2.globalPosition());
}

void testit (const Muon::RpcPrepData& trans1)
{
  MsgStream log (0, "test");
  RpcPrepDataCnv_p3 cnv;
  Muon::RpcPrepData_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::RpcPrepData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D locpos (2.5, 3.5);

  Amg::MatrixX cov(1,1);
  cov(0,0) = 101;

  std::vector<Identifier> rdoList { Identifier(5432),
                                    Identifier(5361),
                                    Identifier(6456) };

  Muon::RpcPrepData trans1 (Identifier (1234),
                            IdentifierHash (1234),
                            locpos,
                            rdoList,
                            new Amg::MatrixX(cov),
                            nullptr,
                            4.5,
                            6,
                            7);
                            
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
