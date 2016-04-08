/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMuonEventTPCnv/test/MuonFeatureCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for MuonFeatureCnv_p3.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/MuonFeatureCnv_p3.h"
#include "SGTools/TestStore.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>
#include <sstream>


void compare (const MuonFeature& p1,
              const MuonFeature& p2)
{
  assert (p1.algoId() == p2.algoId());
  assert (p1.roiId() == p2.roiId());
  assert (p1.saddress() == p2.saddress());
  assert (p1.pt() == p2.pt());
  assert (p1.radius() == p2.radius());
  assert (p1.eta() == p2.eta());
  assert (p1.phi() == p2.phi());
  assert (p1.dir_phi() == p2.dir_phi());
  assert (p1.zeta() == p2.zeta());
  assert (p1.dir_zeta() == p2.dir_zeta());
  assert (p1.beta() == p2.beta());
  assert (p1.sp1_r() == p2.sp1_r());
  assert (p1.sp1_z() == p2.sp1_z());
  assert (p1.sp1_slope() == p2.sp1_slope());
  assert (p1.sp2_r() == p2.sp2_r());
  assert (p1.sp2_z() == p2.sp2_z());
  assert (p1.sp2_slope() == p2.sp2_slope());
  assert (p1.sp3_r() == p2.sp3_r());
  assert (p1.sp3_z() == p2.sp3_z());
  assert (p1.sp3_slope() == p2.sp3_slope());
  assert (p1.br_radius() == p2.br_radius());
  assert (p1.br_sagitta() == p2.br_sagitta());
  assert (p1.ec_alpha() == p2.ec_alpha());
  assert (p1.ec_beta() == p2.ec_beta());
  assert (p1.dq_var1() == p2.dq_var1());
  assert (p1.dq_var2() == p2.dq_var2());
}


void testit (const MuonFeature& trans1)
{
  MsgStream log (0, "test");
  MuonFeatureCnv_p3 cnv;
  MuonFeature_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  MuonFeature trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  MuonFeature trans1 (123,
                      10000.5, // pt
                      101.5, // radius
                      2.5, // eta
                      1.5, // phi
                      0.5, // dir_phi
                      4.5, // zeta
                      0.4, // dir_zeta
                      5.5, //beta
                      6.5, 7.5, 8.5, //sp1_ r, z, slope
                      9.5, 10.5, 11.5, //sp2_ r, z, slope
                      12.5, 13.5, 14.5, //sp3_ r, z, slope
                      15.5, 16.5, // br_radius, sagitta
                      17.5, 18.5, // ec_alpha, beta
                      19.5, 20.5  // dq_var1, var2
                      );
    
  testit (trans1);
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
