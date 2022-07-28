/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/BaseTagInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"
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


void testit (const Analysis::BaseTagInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::BaseTagInfoCnv_p1 cnv;
  Analysis::BaseTagInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::BaseTagInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::BaseTagInfo trans1 ("typ1");
  testit (trans1);

  Analysis::BaseTagInfo trans2 ("typ2");
  trans2.setWeight (1.5);
  trans2.setTagLikelihood ({2.5, 3.5, 4.5});
  testit (trans2);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/BaseTagInfoCnv_p1_test\n";
  test1();
  return 0;
}
