/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetBCM_EventAthenaPool/test/BCM_RDO_ContainerCnv_p0_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for BCM_RDO_ContainerCnv_p0.
 */


#undef NDEBUG
#include "../src/BCM_RDO_ContainerCnv_p0.h"
#include "../src/BCM_RDO_CollectionCnv_p0.h"
#include "../src/BCM_RawDataCnv_p0.h"

#include <cassert>
#include <iostream>
#include <memory>

void compare (const BCM_RawData& p1,
              const BCM_RawData& p2)
{
  assert (p1.getWord1() == p2.getWord1());
  assert (p1.getWord2() == p2.getWord2());
}


void compare (const BCM_RDO_Collection& p1,
              const BCM_RDO_Collection& p2)
{
  assert (p1.getChannel() == p2.getChannel());
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void compare (const BCM_RDO_Container& p1,
              const BCM_RDO_Container& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const BCM_RDO_Container& trans1)
{
  MsgStream log (nullptr, "test");
  BCM_RDO_ContainerCnv_p0 cnv;
  BCM_RDO_Container_p0 pers;
  cnv.transToPers (&trans1, &pers, log);
  BCM_RDO_Container trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  BCM_RDO_Container trans1;
  for (int i=0; i < 10; i++) {
    int o1 = i*100;
    auto coll = std::make_unique<BCM_RDO_Collection> (543+o1);
    for (int j=0; j<i; j++) {
      int o2 = o1 + j*20;
      coll->push_back (std::make_unique<BCM_RawData> (123+o2, 234+o2));
    }
    trans1.push_back (std::move(coll));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
