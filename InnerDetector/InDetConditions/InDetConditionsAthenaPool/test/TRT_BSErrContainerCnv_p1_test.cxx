/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetConditionsAthenaPool/test/TRT_BSErrContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "../src/TRT_BSErrContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>


typedef TRT_BSErrContainer::base_value_type Elt;


void compare (const TRT_BSErrContainer& c1,
              const TRT_BSErrContainer& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    const Elt& p1 = *c1[i];
    const Elt& p2 = *c2[i];
    assert (p1.first == p2.first);
    assert (p1.second == p2.second);
  }
}

void testit (const TRT_BSErrContainer& trans1)
{
  TRT_BSErrContainerCnv_p1 cnv;
  TRT_BSErrContainer_p1 pers;
  MsgStream log (0, "test");
  cnv.transToPers (&trans1, &pers, log);
  TRT_BSErrContainer trans2 (SG::VIEW_ELEMENTS);
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  TRT_BSErrContainer c;

  c.push_back (new Elt (1, 2));
  c.push_back (new Elt (3, 4));
  c.push_back (new Elt (5, 6));
  testit (c);
}


int main()
{
  test1();
  return 0;
}
