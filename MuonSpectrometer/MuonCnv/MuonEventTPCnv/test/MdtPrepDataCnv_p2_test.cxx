/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MdtPrepDataCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/MdtPrepDataCnv_p2.h"
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


void compare (const Muon::MdtPrepData& p1,
              const Muon::MdtPrepData& p2)
{
  compare (static_cast<const Trk::PrepRawData&> (p1),
           static_cast<const Trk::PrepRawData&> (p2));
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.tdc() == p2.tdc());
  assert (p1.adc() == p2.adc());
  assert (p1.status() == p2.status());
  //assert (p1.globalPosition() == p2.globalPosition());
}

void testit (const Muon::MdtPrepData& trans1)
{
  MsgStream log (0, "test");
  MdtPrepDataCnv_p2 cnv;
  Muon::MdtPrepData_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::MdtPrepData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D driftRadius (2.5, 3.5);

  Amg::MatrixX cov(1,1);
  cov(0,0) = 101;

  std::vector<Identifier> rdoList { Identifier(5432),
                                    Identifier(5361),
                                    Identifier(6456) };

  Muon::MdtPrepData trans1 (Identifier (1234),
                            IdentifierHash (1234),
                            driftRadius,
                            CxxUtils::make_unique<Amg::MatrixX>(cov),
                            std::vector<Identifier> (rdoList),
                            nullptr, // detEl,
                            4,
                            5,
                            Muon::MdtStatusDriftTime);
                            
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
