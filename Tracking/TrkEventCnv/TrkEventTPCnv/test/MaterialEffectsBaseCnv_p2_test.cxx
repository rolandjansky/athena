/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/MaterialEffectsBaseCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkMaterialOnTrack//MaterialEffectsBaseCnv_p2.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const Trk::Surface& s1,
              const Trk::Surface& s2)
{
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


void compare (const Trk::MaterialEffectsBase& p1,
              const Trk::MaterialEffectsBase& p2)
{
  assert (p1.thicknessInX0() ==  p2.thicknessInX0());
  assert (p1.dumpType() == p2.dumpType());
  compare (p1.associatedSurface(), p2.associatedSurface());
}


void testit (const Trk::MaterialEffectsBase& trans1)
{
  MsgStream log (0, "test");
  MaterialEffectsBaseCnv_p2 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::MaterialEffectsBase_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::MaterialEffectsOnTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Trk::PerigeeSurface psurf (Amg::Vector3D (50, 100, 150));
  Trk::MaterialEffectsOnTrack me (12.5, psurf);
  testit (me);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
