/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SoftLeptonTruthInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SoftLeptonTruthInfoCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Analysis::BaseTagInfo& p1,
              const Analysis::BaseTagInfo& p2)
{
  assert (p1.isValid() == p2.isValid());
  assert (p1.tagLikelihood() == p2.tagLikelihood());
  assert (p1.weight() == p2.weight());
  assert (p1.infoType() == p2.infoType());
}


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


void compare (const Analysis::SoftLeptonTruthInfo& p1,
              const Analysis::SoftLeptonTruthInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.numSLTrueInfo() == p2.numSLTrueInfo());
  for (int i=0; i < p1.numSLTrueInfo(); i++) {
    compare (p1.getSLTrueInfo(i), p2.getSLTrueInfo(i));
  }
}


void testit (const Analysis::SoftLeptonTruthInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::SoftLeptonTruthInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SoftLeptonTruthInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SoftLeptonTruthInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::SoftLeptonTruthInfo trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();

  Eigen::Vector3d mom1 { 1.5, 2.5, 3.5 };
  Eigen::Vector3d vpos1 { 11.5, 12.5, 13.5 };
  Analysis::SLTrueInfo ti1 (123, 321, 322,
                            true, false, true,
                            mom1, vpos1);
  trans1.addSLTrueInfo (ti1);

  Eigen::Vector3d mom2 { 21.5, 22.5, 23.5 };
  Eigen::Vector3d vpos2 { 31.5, 32.5, 33.5 };
  Analysis::SLTrueInfo ti2 (223, 221, 222,
                            false, true, false,
                            mom2, vpos2);
  trans1.addSLTrueInfo (ti2);
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/SoftLeptonTruthInfoCnv_p1_test\n";
  test1();
  return 0;
}
