/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ParticleEventTPCnv/test/SelectedParticlesCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "ParticleEventTPCnv/SelectedParticlesCnv_p1.h"
#include "ParticleEvent/SelectedParticles.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const SelectedParticles& p1,
              const SelectedParticles& p2)
{
  assert (p1.AllBits() == p2.AllBits());
}


void test1()
{
  std::cout << "test1\n";

  SelectedParticles trans1;
  trans1.SetBit (10);
  trans1.SetBit (20);
  trans1.SetBit (100);

  MsgStream log (0, "test");

  SelectedParticlesCnv_p1 cnv;
  SelectedParticles_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  SelectedParticles trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  test1();
  return 0;
}
