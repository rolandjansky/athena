/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/LifetimeInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/LifetimeInfoCnv_p1.h"
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


void compare (const Analysis::LifetimeInfo& p1,
              const Analysis::LifetimeInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.signedIP() == p2.signedIP());
  assert (p1.significance() == p2.significance());
  assert (p1.vectorTrackProb() == p2.vectorTrackProb());
  assert (p1.nTrackProb() == p2.nTrackProb());
}


void testit (const Analysis::LifetimeInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::LifetimeInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::LifetimeInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::LifetimeInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::LifetimeInfo trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.setIP ({5.5, 6.5});
  trans1.setSignificance ({7.5, 8.5, 9.5});
  trans1.setTrackProb ({10.5, 11.5, 12.5, 13.5});
  trans1.setNTrackProb (14.5);
  trans1.makeValid();
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/LifetimeInfoCnv_p1_test\n";
  test1();
  return 0;
}
