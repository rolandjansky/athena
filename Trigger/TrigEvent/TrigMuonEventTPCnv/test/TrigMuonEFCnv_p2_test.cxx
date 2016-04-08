/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMuonEventTPCnv/test/TrigMuonEFCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigMuonEFCnv_p2.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p2.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const P4IPtCotThPhiM& p1,
              const P4IPtCotThPhiM& p2)
{
  assert (Athena_test::isEqual (p1.iPt(), p2.iPt()));
  assert (p1.cotTh() == p2.cotTh());
  assert (p1.phi() == p2.phi());
  assert (p1.m() == p2.m());
}


void compare (const TrigMuonEF& p1,
              const TrigMuonEF& p2)
{
  compare (static_cast<const P4IPtCotThPhiM&>(p1),
           static_cast<const P4IPtCotThPhiM&>(p2));

  assert (p1.MuonCode() == p2.MuonCode());
  assert (p1.RoINum() == p2.RoINum());
  assert (p1.charge() == p2.charge());
}


void testit (const TrigMuonEF& trans1)
{
  MsgStream log (0, "test");
  TrigMuonEFCnv_p2 cnv;
  TrigMuonEF_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMuonEF trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigMuonEF trans1 (1./80000, 1.5, 2.5, 5000);
  trans1.set_muonCode (123);
  trans1.set_RoINum ("foo");
  trans1.set_Charge (4.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
