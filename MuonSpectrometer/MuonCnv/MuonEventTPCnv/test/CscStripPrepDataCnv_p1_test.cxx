/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/CscStripPrepDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonPrepRawData/CscStripPrepDataCnv_p1.h"
#include "MuonEventTPCnv/CscStripPrepDataContainerCnv_tlp1.h"
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


void compare (const Muon::CscStripPrepData& p1,
              const Muon::CscStripPrepData& p2)
{
  compare (static_cast<const Trk::PrepRawData&>(p1),
           static_cast<const Trk::PrepRawData&>(p2));
  assert (p1.collectionHash() == p2.collectionHash());
  //assert (p1.globalPosition() == p2.globalPosition());
  assert (p1.detectorElement() == p2.detectorElement());
  assert (p1.sampleCharges() == p2.sampleCharges());
  assert (p1.timeOfFirstSample() == p2.timeOfFirstSample());
  assert (p1.samplingTime() == p2.samplingTime());
  assert (p1.samplingPhase() == p2.samplingPhase());
}

void testit (const Muon::CscStripPrepData& trans1)
{
  MsgStream log (0, "test");
  CscStripPrepDataCnv_p1 cnv;
  CscStripPrepDataContainerCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Muon::CscStripPrepData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::CscStripPrepData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Amg::Vector2D locpos (1.5, 2.5);

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  std::vector<float> sampleCharges { 5.5, 6.5, 7.5, 8.5 };

  Muon::CscStripPrepData trans1 (Identifier (0x1234),
                                 IdentifierHash (567),
                                 locpos,
                                 new Amg::MatrixX(cov),
                                 nullptr,
                                 sampleCharges,
                                 2.5,
                                 22);
                            
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
