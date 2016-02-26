/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetConditionsAthenaPool/test/TRT_BSIdErrContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "../src/TRT_BSIdErrContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>


//  DataVector<std::pair<uint8_t, std::pair<uint32_t, uint8_t> > > TRT_BSIdErrContainer; 
typedef TRT_BSIdErrContainer::base_value_type Elt;


void compare (const TRT_BSIdErrContainer& c1,
              const TRT_BSIdErrContainer& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    const Elt& p1 = *c1[i];
    const Elt& p2 = *c2[i];
    assert (p1.first == p2.first);
    assert (p1.second.first == p2.second.first);
    assert (p1.second.second == p2.second.second);
  }
}

void testit (const TRT_BSIdErrContainer& trans1)
{
  TRT_BSIdErrContainerCnv_p1 cnv;
  TRT_BSIdErrContainer_p1 pers;
  MsgStream log (0, "test");
  cnv.transToPers (&trans1, &pers, log);
  TRT_BSIdErrContainer trans2 (SG::VIEW_ELEMENTS);
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  TRT_BSIdErrContainer c;

  c.push_back (new Elt {1, {2, 3}});
  c.push_back (new Elt {4, {5, 6}});
  c.push_back (new Elt {7, {8, 9}});
  testit (c);
}


int main()
{
  test1();
  return 0;
}
