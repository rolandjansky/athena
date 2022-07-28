/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SLTrueInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SLTrueInfoCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Analysis::SLTrueInfo& p1,
              const Analysis::SLTrueInfo& p2)
{
  assert (p1.barcode() == p2.barcode());
  assert (p1.pdgId() == p2.pdgId());
  assert (p1.pdgIdMother() == p2.pdgIdMother());
  assert (p1.FromB() == p2.FromB());
  assert (p1.FromD() == p2.FromD());
  assert (p1.FromGH() == p2.FromGH());
  assert (p1.momentum() == p2.momentum());
  assert (p1.prodvtx() == p2.prodvtx());
}


void testit (const Analysis::SLTrueInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::SLTrueInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SLTrueInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SLTrueInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Eigen::Vector3d mom { 1.5, 2.5, 3.5 };
  Eigen::Vector3d vpos { 11.5, 12.5, 13.5 };
  Analysis::SLTrueInfo trans1 (123, 321, 322,
                               true, false, true,
                               mom, vpos);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/SLTrueInfoCnv_p1_test\n";
  test1();
  return 0;
}
