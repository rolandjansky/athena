/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ZdcEventTPCnv/test/ZdcRawChannelCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ZdcRawChannelCollectionCnv_p1.
 */


#undef NDEBUG
#include "ZdcEventTPCnv/ZdcRawChannelCollectionCnv_p1.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void compare (const ZdcRawChannel& p1,
              const ZdcRawChannel& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.getSize() == p2.getSize());
  for (size_t i = 0; i < p1.getSize(); i++) {
    assert (p1.getEnergy(i) == p2.getEnergy(i));
    assert (p1.getTime(i) == p2.getTime(i));
    assert (p1.getChi(i) == p2.getChi(i));
  }
}


void compare (const ZdcRawChannelCollection& p1,
              const ZdcRawChannelCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const ZdcRawChannelCollection& trans1)
{
  MsgStream log (0, "test");
  ZdcRawChannelCollectionCnv_p1 cnv;
  ZdcRawChannelCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ZdcRawChannelCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  ZdcRawChannelCollection trans1;
  for (int i=0; i < 10; i++) {
    auto p = CxxUtils::make_unique<ZdcRawChannel>(Identifier (1234));
    p->setSize (3);
    int o = i*100;
    for (int k=0; k < 3; k++) {
      p->setEnergy (k, 12345.5 + k*10 + o);
      p->setTime (k, 100.5 + k*10 + o);
      p->setChi (k, 20.5 + k*10 + o);
    }
    trans1.push_back (std::move(p));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
