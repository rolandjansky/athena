/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTopoEventTPCnv/test/ElectronMuonTopoInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for ElectronMuonTopoInfoCnv_p1.
 */


#undef NDEBUG
#include "TrigTopoEventTPCnv/ElectronMuonTopoInfoCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const ElectronMuonTopoInfo& p1,
              const ElectronMuonTopoInfo& p2)
{
  assert (p1.RoiWord() == p2.RoiWord());
  assert (p1.DeltaPhi() == p2.DeltaPhi());
  assert (p1.DeltaR() == p2.DeltaR());
  assert (p1.InvMass() == p2.InvMass());
  assert (p1.ElecValid() == p2.ElecValid());
  assert (p1.OppositeCharge() == p2.OppositeCharge());
  assert (p1.VertexState() == p2.VertexState());
}


void testit (const ElectronMuonTopoInfo& trans1)
{
  MsgStream log (0, "test");
  ElectronMuonTopoInfoCnv_p1 cnv;
  ElectronMuonTopoInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ElectronMuonTopoInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  ElectronMuonTopoInfo trans1 (123, 1.5, 2.5, 5000,
                               true, false, 321);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
