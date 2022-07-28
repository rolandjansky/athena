/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SvxSummaryCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SvxSummaryCnv_p1.h"
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


void compare (const Trk::Vertex& p1,
              const Trk::Vertex& p2)
{
  assert (p1.position() == p2.position());
}


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
}


void compare (const Trk::RecVertex& p1,
              const Trk::RecVertex& p2)
{
  compare (static_cast<const Trk::Vertex&>(p1),
           static_cast<const Trk::Vertex&>(p2));
  // SvxSummaryCnv_p1 saves only the the Trk::Vertex part.
  //assert (p1.covariancePosition() == p2.covariancePosition());
  //compare (p1.fitQuality(), p2.fitQuality());
}


void compare (const Analysis::SvxSummary& p1,
              const Analysis::SvxSummary& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  compare (p1.Svx(), p2.Svx());
  assert (p1.Results() == p2.Results());
  assert (p1.TrkInSvx().size() == p2.TrkInSvx().size());
}


void testit (const Analysis::SvxSummary& trans1)
{
  MsgStream log (0, "test");
  Analysis::SvxSummaryCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SvxSummary_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SvxSummary trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::SvxSummary trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();

  trans1.Results ({2.5, 7.5});
  
  Amg::Vector3D pos1 (10.5, 11.5, 12.5);
  AmgSymMatrix(3) cov1;
  for (int i=0; i < 3; i++) {
    for (int j=0; j<3; j++) {
      cov1(i,j) = 100*(i+1)*(j+1);
    }
  }
  Trk::RecVertex rv1 (pos1, cov1, 21.5, 22.5);
  trans1.Svx (rv1);

  // Don't test the TrackParticle parts.  That's buggy and will leak memory,
  // and doesn't actually appear to be used in any case.

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/SvxSummaryCnv_p1_test\n";
  test1();
  return 0;
}
