/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/TruthInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/TruthInfoCnv_p1.h"
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


void compare (const Analysis::TruthInfo& p1,
              const Analysis::TruthInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.jetTruthLabel() == p2.jetTruthLabel());
  assert (p1.deltaRMinTo("B") == p2.deltaRMinTo("B"));
  assert (p1.deltaRMinTo("C") == p2.deltaRMinTo("C"));
  assert (p1.deltaRMinTo("T") == p2.deltaRMinTo("T"));
  assert (p1.BDecayVertex() == p2.BDecayVertex());
  assert (p1.BHadronPdg() == p2.BHadronPdg());
}


void testit (const Analysis::TruthInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::TruthInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::TruthInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::TruthInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::TruthInfo trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();
  trans1.setJetTruthLabel ("lab");
  trans1.deltaRMinTo ("B", 7.5);
  trans1.deltaRMinTo ("C", 8.5);
  trans1.deltaRMinTo ("T", 9.5);
  trans1.BDecayVertex ({10.5, 11.5, 12.5});
  trans1.BHadronPdg (17);
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/TruthInfoCnv_p1_test\n";
  test1();
  return 0;
}
