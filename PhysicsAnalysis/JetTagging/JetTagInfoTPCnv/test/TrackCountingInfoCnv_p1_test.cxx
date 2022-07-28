/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/TrackCountingInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/TrackCountingInfoCnv_p1.h"
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


void compare (const Analysis::TrackCountingInfo& p1,
              const Analysis::TrackCountingInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.nTracks() == p2.nTracks());
  assert (p1.d0sig_2nd() == p2.d0sig_2nd());
  assert (p1.d0sig_abs_2nd() == p2.d0sig_abs_2nd());
  assert (p1.d0sig_3rd() == p2.d0sig_3rd());
  assert (p1.d0sig_abs_3rd() == p2.d0sig_abs_3rd());
}


void testit (const Analysis::TrackCountingInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::TrackCountingInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::TrackCountingInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::TrackCountingInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::TrackCountingInfo trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();
  trans1.setnTracks (11);
  trans1.setd0sig_2nd (20.5);
  trans1.setd0sig_abs_2nd (21.5);
  trans1.setd0sig_3rd (22.5);
  trans1.setd0sig_abs_3rd (23.5);
  
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/TrackCountingInfoCnv_p1_test\n";
  test1();
  return 0;
}
