/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/AtlfInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/AtlfInfoCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
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


void compare (const Analysis::AtlfInfo& p1,
              const Analysis::AtlfInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.isBTagged() == p2.isBTagged());
  assert (p1.isTauTagged() == p2.isTauTagged());
  assert (p1.isTau1PTagged() == p2.isTau1PTagged());
  assert (p1.isTau3PTagged() == p2.isTau3PTagged());
  assert (p1.LightHypoCalFactor() == p2.LightHypoCalFactor());
  assert (p1.TauHypoCalFactor() == p2.TauHypoCalFactor());
  assert (p1.Tau1P3PHypoCalFactor() == p2.Tau1P3PHypoCalFactor());
  assert (p1.Tau1P3PHypoCalFactor() == p2.Tau1P3PHypoCalFactor());
  assert (p1.BHypoCalFactor() == p2.BHypoCalFactor());
  assert (p1.deltaRMinTo("B") == p2.deltaRMinTo("B"));
  assert (p1.deltaRMinTo("C") == p2.deltaRMinTo("C"));
  assert (p1.deltaRMinTo("T") == p2.deltaRMinTo("T"));
}


void testit (const Analysis::AtlfInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::AtlfInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::AtlfInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::AtlfInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::AtlfInfo trans1 ("typ1");
  testit (trans1);

  Analysis::AtlfInfo trans2 ("typ2");
  trans2.setWeight (1.5);
  trans2.setTagLikelihood ({2.5, 3.5, 4.5});
  trans2.isBTagged (true);
  trans2.isTau1PTagged (true);
  trans2.LightHypoCalFactor (5.5);
  trans2.TauHypoCalFactor (6.5);
  trans2.Tau1P3PHypoCalFactor (7.5);
  trans2.BHypoCalFactor (8.5);
  trans2.deltaRMinTo ("B", 9.5);
  trans2.deltaRMinTo ("C", 10.5);
  trans2.deltaRMinTo ("T", 11.5);
  trans2.makeValid();
  testit (trans2);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/AtlfInfoCnv_p1_test\n";
  test1();
  return 0;
}
