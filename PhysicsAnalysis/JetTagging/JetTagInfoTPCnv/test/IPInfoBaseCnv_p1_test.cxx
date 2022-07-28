/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/IPInfoBaseCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/IPInfoBaseCnv_p1.h"
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


void compare (const Analysis::IPInfoBase& p1,
              const Analysis::IPInfoBase& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.nbTracks() == p2.nbTracks());
}


void testit (const Analysis::IPInfoBase& trans1)
{
  MsgStream log (0, "test");
  Analysis::IPInfoBaseCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::IPInfoBase_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::IPInfoBase trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::IPInfoBase trans1 ("typ");
  trans1.nbTracks (3);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/IPInfoBaseCnv_p1_test\n";
  test1();
  return 0;
}
