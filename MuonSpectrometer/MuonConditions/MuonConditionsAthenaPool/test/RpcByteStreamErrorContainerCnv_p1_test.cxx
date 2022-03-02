/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonConditionsAthenaPool/test/RpcByteStreamErrorContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "MuonConditionsAthenaPool/MuonByteStreamErrors/RpcByteStreamErrorContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>


void compare (const Muon::RpcByteStreamErrorContainer& c1,
              const Muon::RpcByteStreamErrorContainer& c2)
{
  const DataVector<std::pair<IdentifierHash, int> >& err1 = c1.getAllErrors();
  const DataVector<std::pair<IdentifierHash, int> >& err2 = c2.getAllErrors();
  assert (err1.size() == err2.size());
  for (size_t i = 0; i < err1.size(); i++) {
    assert (*err1[i] == *err2[i]);
  }
}

void testit (const Muon::RpcByteStreamErrorContainer& trans1)
{
  RpcByteStreamErrorContainerCnv_p1 cnv;
  Muon::RpcByteStreamErrorContainer_p1 pers;
  MsgStream log (nullptr, "test");
  cnv.transToPers (&trans1, &pers, log);
  Muon::RpcByteStreamErrorContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Muon::RpcByteStreamErrorContainer c;
  c.addError (1, 2);
  c.addError (3, 4);
  c.addError (5, 6);
  testit (c);
}


int main()
{
  test1();
  return 0;
}
