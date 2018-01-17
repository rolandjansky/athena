/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MuonSimDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const MuonMCData& p1,
              const MuonMCData& p2)
{
  assert (p1.firstEntry() == p2.firstEntry());
  assert (p1.secondEntry() == p2.secondEntry());
}


void compare (const MuonSimData& p1,
              const MuonSimData& p2)
{
  assert (p1.word() == p2.word());
  assert (p1.globalPosition() == p2.globalPosition());
  const std::vector< MuonSimData::Deposit >& dep1 = p1.getdeposits();
  const std::vector< MuonSimData::Deposit >& dep2 = p2.getdeposits();
  assert (dep1.size() == dep2.size());
  for (size_t i = 0; i < dep1.size(); i++) {
    assert (dep1[i].first == dep2[i].first);
    compare (dep1[i].second, dep2[i].second);
  }
}


void testit (const MuonSimData& trans1)
{
  MsgStream log (0, "test");
  MuonSimDataCnv_p1 cnv;
  Muon::MuonSimData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  MuonSimData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  std::vector<MuonSimData::Deposit> deps;
  deps.emplace_back (123, MuonMCData ( 2.5,  3.5));
  deps.emplace_back (223, MuonMCData (12.5, 13.5));
  deps.emplace_back (323, MuonMCData (22.5, 23.5));
  MuonSimData trans1 (deps, 4321);
  trans1.setPosition (Amg::Vector3D ( 4.5,  5.5,  6.5));
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
