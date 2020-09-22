/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/MaterialEffectsOnTrackCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p2.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const Trk::EnergyLoss& p1,
              const Trk::EnergyLoss& p2)
{
  assert (p1.deltaE() == p2.deltaE());
  assert (p1.sigmaDeltaE() == p2.sigmaDeltaE());
  assert (p1.sigmaMinusDeltaE() == p2.sigmaMinusDeltaE());
  assert (p1.sigmaPlusDeltaE() == p2.sigmaPlusDeltaE());
}


void compare (const Trk::ScatteringAngles& p1,
              const Trk::ScatteringAngles& p2)
{
  assert (Athena_test::isEqual (p1.deltaPhi(), p2.deltaPhi()));
  assert (Athena_test::isEqual (p1.deltaTheta(), p2.deltaTheta()));
  assert (Athena_test::isEqual (p1.sigmaDeltaPhi(), p2.sigmaDeltaPhi()));
  assert (Athena_test::isEqual (p1.sigmaDeltaTheta(), p2.sigmaDeltaTheta()));
}


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


void compare (const Trk::MaterialEffectsOnTrack& p1,
              const Trk::MaterialEffectsOnTrack& p2)
{
  compare (static_cast<const Trk::MaterialEffectsBase&>(p1),
           static_cast<const Trk::MaterialEffectsBase&>(p2));

  if (p1.energyLoss())
    compare (*p1.energyLoss(), *p2.energyLoss());
  else
    assert (p2.energyLoss() == nullptr);

  if (p1.scatteringAngles())
    compare (*p1.scatteringAngles(), *p2.scatteringAngles());
  else
    assert (p2.scatteringAngles() == nullptr);
}


void testit (const Trk::MaterialEffectsOnTrack& trans1)
{
  MsgStream log (0, "test");
  MaterialEffectsOnTrackCnv_p2 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::MaterialEffectsOnTrack_p2 pers;
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
  Trk::EnergyLoss el (1.5, 2.5, 3.5, 4.5);
  Trk::ScatteringAngles sa (0.1, 0.2, 0.3, 0.4);

  Trk::MaterialEffectsOnTrack me1 (12.5, psurf);
  testit (me1);

  Trk::MaterialEffectsOnTrack me2 (12.5,
                                   new Trk::EnergyLoss (el),
                                   psurf);
  testit (me2);

  Trk::MaterialEffectsOnTrack me3 (12.5,
                                   new Trk::ScatteringAngles (sa),
                                   psurf);
  testit (me3);

  Trk::MaterialEffectsOnTrack me4 (12.5,
                                   new Trk::ScatteringAngles (sa),
                                   new Trk::EnergyLoss (el),
                                   psurf);
  testit (me4);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
