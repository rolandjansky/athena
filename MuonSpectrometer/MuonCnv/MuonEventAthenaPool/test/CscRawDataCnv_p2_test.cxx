/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/CscRawDataCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/CscRawDataCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/initGaudi.h"
#include "muIdHelper.icc"
#include <cassert>
#include <iostream>

void compare (const CscRawData& p1,
              const CscRawData& p2)
{
  assert (p1.samples() == p2.samples());
  assert (p1.address() == p2.address());
  assert (p1.identify() == p2.identify());
  assert (p1.time() == p2.time());
  assert (p1.rpuID() == p2.rpuID());
  assert (p1.width() == p2.width());
  assert (p1.isTimeComputed() == p2.isTimeComputed());
  assert (p1.hashId() == p2.hashId());
}


void testit (const CscRawData& trans1, Muon::MuonIdHelperSvc* muIdSvc)
{
  MsgStream log (nullptr, "test");
  CscRawDataCnv_p2 cnv;
  cnv.setMuonIdHelperSvc(muIdSvc);
  CscRawData_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  CscRawData trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1(Muon::MuonIdHelperSvc* muIdSvc)
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  CscRawData trans1 (std::vector<uint16_t>(3001, 3002),
                     3, 4, 5, 6);
  testit (trans1, muIdSvc);
  CscRawData trans2 (std::vector<uint16_t>(3011, 3012),
                     13, 14, 15, 16, 17, 18);
  testit (trans2, muIdSvc);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventAthenaPool_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }
  Muon::MuonIdHelperSvc* muIdSvc = muIdHelperSvc();
  test1(muIdSvc);
  return 0;
}
