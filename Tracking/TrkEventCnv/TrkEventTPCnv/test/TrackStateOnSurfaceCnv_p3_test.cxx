/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/TrackStateOnSurfaceCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p3.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
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


void compare (const Trk::TrackParameters& p1,
              const Trk::TrackParameters& p2)
{
  int offs = 0;
  if (typeid (*&p2) == typeid(Trk::CurvilinearParameters) &&
      typeid (*&p1) == typeid(Trk::AtaCone))
    offs = 2;
  else
    assert (typeid (*&p1) == typeid (*&p2));
          
  assert (p1.parameters().size() == p2.parameters().size());
  for (int i = 0; i < offs; i++)
    assert (p2.parameters()[i] == 0);
  for (int i = offs; i < p1.parameters().size(); i++)
    assert (Athena_test::isEqual (p1.parameters()[i], p2.parameters()[i]));
  if (p1.covariance())
    assert (*p1.covariance() == *p2.covariance());
  else
    assert (!p2.covariance());

  
  for (int i = 0; i < 3; i++) {
    assert (Athena_test::isEqual (p1.position()[i], p2.position()[i]));
    assert (Athena_test::isEqual (p1.momentum()[i], p2.momentum()[i]));
  }
  assert (p1.charge() == p2.charge());

  compare (p1.associatedSurface(), p2.associatedSurface());
}


void compare (const Trk::MeasurementBase& p1,
              const Trk::MeasurementBase& p2)
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

  compare (p1.associatedSurface(), p2.associatedSurface());
}


void compare (const Trk::MaterialEffectsBase& p1,
              const Trk::MaterialEffectsBase& p2)
{
  assert (p1.thicknessInX0() ==  p2.thicknessInX0());
  assert (p1.dumpType() == p2.dumpType());
  compare (p1.associatedSurface(), p2.associatedSurface());
}


void compare (const Trk::TrackStateOnSurface& p1,
              const Trk::TrackStateOnSurface& p2)
{
  if (p1.fitQualityOnSurface())
    compare (*p1.fitQualityOnSurface(), *p2.fitQualityOnSurface());
  else
    assert (!p2.fitQualityOnSurface());

  if (p1.trackParameters())
    compare (*p1.trackParameters(), *p2.trackParameters());
  else
    assert (!p2.trackParameters());

  if (p1.measurementOnTrack())
    compare (*p1.measurementOnTrack(), *p2.measurementOnTrack());
  else
    assert (!p2.measurementOnTrack());

  if (p1.materialEffectsOnTrack())
    compare (*p1.materialEffectsOnTrack(), *p2.materialEffectsOnTrack());
  else
    assert (!p2.materialEffectsOnTrack());

  assert (p1.types() == p2.types());
}
              
void testit (const Trk::TrackStateOnSurface& trans1)
{
  MsgStream log (0, "test");
  TrackStateOnSurfaceCnv_p3 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Trk::TrackStateOnSurface_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::TrackStateOnSurface trans2;
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
  Trk::PseudoMeasurementOnTrack pmeas (parms1, cov, psurf);
  Trk::Perigee perigee (100, 200, 1.5, 0.5, 1e-3, psurf, nullptr);
  Trk::FitQuality fq (10, 20);
  Trk::MaterialEffectsOnTrack me (12.5, psurf);

  Trk::TrackStateOnSurface trans1 (new Trk::PseudoMeasurementOnTrack (pmeas),
                                   new Trk::Perigee (perigee),
                                   new Trk::FitQuality (fq),
                                   new Trk::MaterialEffectsOnTrack (me),
                                   nullptr);
  testit (trans1);

  Trk::TrackStateOnSurface trans2 (new Trk::PseudoMeasurementOnTrack (pmeas),
                                   nullptr,
                                   nullptr,
                                   nullptr,
                                   nullptr);
  testit (trans2);

  Trk::TrackStateOnSurface trans3 (nullptr,
                                   new Trk::Perigee (perigee),
                                   nullptr,
                                   nullptr,
                                   nullptr);
  testit (trans3);

  Trk::TrackStateOnSurface trans4 (nullptr,
                                   nullptr,
                                   new Trk::FitQuality (fq),
                                   nullptr,
                                   nullptr);
  testit (trans4);

  Trk::TrackStateOnSurface trans5 (nullptr,
                                   nullptr,
                                   nullptr,
                                   new Trk::MaterialEffectsOnTrack (me),
                                   nullptr);
  testit (trans5);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
