/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/PseudoMeasurementOnTrackCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrackCnv_p2.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const Trk::PseudoMeasurementOnTrack& p1,
              const Trk::PseudoMeasurementOnTrack& p2)
{
  assert (p1.localParameters().size() == p2.localParameters().size());
  for (int i = 0; i < p1.localParameters().size(); i++) {
    Trk::ParamDefs ii = static_cast<Trk::ParamDefs>(i);
    assert (Athena_test::isEqual (p1.localParameters()[ii], p2.localParameters()[ii]));
  }
  assert (p1.localCovariance() == p2.localCovariance());

  
  for (int i = 0; i < 3; i++) {
    assert (Athena_test::isEqual (p1.globalPosition()[i], p2.globalPosition()[i]));
  }

  const Trk::Surface& s1 = p1.associatedSurface();
  const Trk::Surface& s2 = p2.associatedSurface();
  if (s1.name() == "Trk::ConeSurface") {
    assert (s2.name() == "Trk::PlaneSurface");
  }
  else {
    assert (s1.name() == s2.name());
    if (s1.name() == "Trk::PerigeeSurface" ||
        s1.name() == "Trk::PlaneSurface" ||
        s1.name() == "Trk::StraightLineSurface")
    {
      for (int i=0; i < 4; i++)
        for (int j=0; j < 4; j++)
          assert (Athena_test::isEqual (s1.transform()(i,j), s2.transform()(i,j)));
    }
    else {
      std::cout << "Unknown surface type: " << s1.name() << "\n";
      std::abort();
    }
  }
}
              
void testit (const Trk::PseudoMeasurementOnTrack& trans1)
{
  MsgStream log (0, "test");
  PseudoMeasurementOnTrackCnv_p2 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::PseudoMeasurementOnTrack_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::PseudoMeasurementOnTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AmgSymMatrix(5) cov;
  for (int i=0; i < 5; i++)
    for (int j=0; j < 5; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  Trk::PerigeeSurface psurf (Amg::Vector3D (50, 100, 150));
  Trk::LocalParameters parms1 (1.5, 2.5, 3.5, 4.5, 5.5);
  Trk::PseudoMeasurementOnTrack trans (parms1, cov, psurf);

  testit (trans);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
