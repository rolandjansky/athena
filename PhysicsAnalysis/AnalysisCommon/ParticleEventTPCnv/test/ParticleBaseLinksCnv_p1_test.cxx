/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ParticleEventTPCnv/test/ParticleBaseLinksCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/ParticleBaseLinksCnv_p1.h"
#include "ParticleEvent/ParticleBaseLinks.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const ParticleBaseLinks& c1,
              const ParticleBaseLinks& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++)
    assert (c1[i] == c2[i]);
}


void test1()
{
  std::cout << "test1\n";
  MsgStream log (0, "test");

  ParticleBaseLinks trans1;
  trans1.push_back (ParticleBaseLink ("pp", 10));
  trans1.push_back (ParticleBaseLink ("pp", 20));
  
  ParticleBaseLinksCnv_p1 cnv;
  ParticleLinks_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  ParticleBaseLinks trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
