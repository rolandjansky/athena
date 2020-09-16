/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/TrackCnv_p4_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkTrack/TrackCnv_p4.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
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


void compare (const DataVector<const Trk::TrackParameters>& p1,
              const DataVector<const Trk::TrackParameters>& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
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


void compare (const DataVector<const Trk::MeasurementBase>& p1,
              const DataVector<const Trk::MeasurementBase>& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
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


void compare (const DataVector<const Trk::TrackStateOnSurface>& p1,
              const DataVector<const Trk::TrackStateOnSurface>& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void compare (const Trk::TrackSummary& p1,
              const Trk::TrackSummary& p2)
{
  for (int i=0; i < Trk::SummaryType::numberOfTrackSummaryTypes; i++) {
    Trk::SummaryType ii = static_cast<Trk::SummaryType>(i);
    assert (p1.get(ii) == p2.get(ii));
  }

  for (int i=0; i < Trk::eProbabilityType::numberOfeProbabilityTypes; i++) {
    Trk::eProbabilityType ii = static_cast<Trk::eProbabilityType>(i);
    assert (p1.getPID(ii) == p2.getPID(ii));
  }
}


void compare (const Trk::TrackInfo& p1,
              const Trk::TrackInfo& p2)
{
  assert (p1.trackFitter() == p2.trackFitter());
  assert (p2.particleHypothesis() == p2.particleHypothesis());
  assert (p1.properties() == p2.properties());
  assert (p1.patternRecognition() == p2.patternRecognition());
}


void compare (const Trk::Track& p1,
              const Trk::Track& p2)
{
#define COMPARE(N)                              \
  if (p1.N())                                   \
    compare (*p1.N(), *p2.N());                 \
  else                                          \
    assert (!p2.N());

  COMPARE(fitQuality);
  COMPARE(perigeeParameters);  // Perigee
  COMPARE(trackParameters);  // DataVector<const TrackParameters>
  COMPARE(measurementsOnTrack);  // DataVector<const MeasurementBase>
  COMPARE(outliersOnTrack);  // DataVector<const MeasurementBase>
  COMPARE(trackStateOnSurfaces);  // DataVector<const TrackStateOnSurface>
  COMPARE(trackSummary);
#undef COMPARE

  compare (p1.info(), p2.info());
}


void testit (const Trk::Track& trans1)
{
  MsgStream log (0, "test");
  TrackCnv_p4 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Trk::Track_p4 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::Track trans2;
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

  Trk::TrackStateOnSurface tsos1 (new Trk::PseudoMeasurementOnTrack (pmeas),
                                  new Trk::Perigee (perigee),
                                  new Trk::FitQuality (fq),
                                  new Trk::MaterialEffectsOnTrack (me),
                                  nullptr);

  DataVector<const Trk::TrackStateOnSurface> tsvec (SG::VIEW_ELEMENTS);
  tsvec.push_back (&tsos1);

  std::bitset<Trk::TrackInfo::NumberOfTrackProperties> properties;
  std::bitset<Trk::TrackInfo::NumberOfTrackRecoInfo> patrec;
  properties[0] = true;
  properties[2] = true;
  properties[3] = true;
  patrec[1] = true;
  patrec[3] = true;
  Trk::TrackInfo info (Trk::TrackInfo::GlobalChi2Fitter,
                       Trk::electron,
                       properties,
                       patrec);

  Trk::Track trans1 (info,
                     new DataVector<const Trk::TrackStateOnSurface> (tsvec),
                     new Trk::FitQuality (fq));
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
