/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/CscSimDataCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollectionCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CscMcData& p1,
              const CscMcData& p2)
{
  assert (p1.energy() == p2.energy());
  assert (p1.ypos() == p2.ypos());
  assert (p1.zpos() == p2.zpos());
  assert (p1.charge() == p2.charge());
}


void compare (const CscSimData& p1,
              const CscSimData& p2)
{
  assert (p1.word() == p2.word());
  const std::vector< CscSimData::Deposit >& dep1 = p1.getdeposits();
  const std::vector< CscSimData::Deposit >& dep2 = p2.getdeposits();
  assert (dep1.size() == dep2.size());
  for (size_t i = 0; i < dep1.size(); i++) {
    assert (dep1[i].first == dep2[i].first);
    compare (dep1[i].second, dep2[i].second);
  }
}


void compare (const CscSimDataCollection& p1,
              const CscSimDataCollection& p2)
{
  assert (p1.size() == p2.size());
  CscSimDataCollection::const_iterator it1 = p1.begin();
  CscSimDataCollection::const_iterator it2 = p2.begin();
  for (; it1 != p1.end(); ++it1, ++it2) {
    assert (it1->first == it2->first);
    compare (it1->second, it2->second);
  }
}


void testit (const CscSimDataCollection& trans1)
{
  MsgStream log (0, "test");
  CscSimDataCollectionCnv_p1 cnv;
  Muon::CscSimDataCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  CscSimDataCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  CscSimDataCollection trans1;
  for (int i=0; i < 3; i++) {
    std::vector<CscSimData::Deposit> deps;
    deps.emplace_back (123, CscMcData ( 2.5+i,  3.5+i,  4.5+i));
    deps.emplace_back (223, CscMcData (12.5+i, 13.5+i, 14.5+i));
    deps.emplace_back (323, CscMcData (22.5+i, 23.5+i, 24.5+i));
    deps[0].second.setCharge ( 5.5+i);
    deps[1].second.setCharge (15.5+i);
    deps[2].second.setCharge (25.5+i);
    trans1[Identifier(1234+i)] = CscSimData (deps, 4321+i);
  }
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
