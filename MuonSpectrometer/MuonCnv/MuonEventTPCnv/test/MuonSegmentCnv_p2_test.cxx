/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MuonSegmentCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonSegment/MuonSegmentCnv_p2.h"
#include "TrkEventTPCnv/SegmentCollectionCnv_tlp3.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkEventTPCnv/SegmentCollectionCnv_tlp3.h"
#include "TestTools/leakcheck.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::LocalDirection& p1,
              const Trk::LocalDirection& p2)
{
  assert (Athena_test::isEqual (p1.angleXZ(), p2.angleXZ()));
  assert (Athena_test::isEqual (p1.angleYZ(), p2.angleYZ()));
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


void compare (const Trk::MeasurementBase& p1,
              const Trk::MeasurementBase& p2,
              bool compsurf = true)
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

  if (compsurf)
    compare (p1.associatedSurface(), p2.associatedSurface());
}


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
}


void compare (const Trk::Segment& p1,
              const Trk::Segment& p2)
{
  compare (static_cast<const Trk::MeasurementBase&> (p1),
           static_cast<const Trk::MeasurementBase&> (p2),
           false);

  compare (*p1.fitQuality(), *p2.fitQuality());
  assert (p1.author() == p2.author());
  assert (p1.numberOfMeasurementBases() ==
          p2.numberOfMeasurementBases());

  for (unsigned int i=0; i < p1.numberOfMeasurementBases(); i++)
    compare (*p1.measurement(i), *p2.measurement(i));
}


void compare (const Muon::MuonSegment& p1,
              const Muon::MuonSegment& p2)
{
  assert (p1.globalDirection() == p2.globalDirection());
  compare (p1.localDirection(), p2.localDirection());
}


void testit (const Muon::MuonSegment& trans1)
{
  MsgStream log (0, "test");
  MuonSegmentCnv_p2 cnv;
  SegmentCollectionCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Muon::MuonSegment_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::MuonSegment trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Trk::LocalParameters locpars (1.5, 2.5, 3.5, 4.5, 5.5);
  Trk::FitQuality fq (1.5, 2.5);

  Amg::MatrixX cov(5,5);
  for (int i=0; i < 5; i++)
    for (int j=0; j < 5; j++)
      cov(i,j) = 100*(i+1)*(j+1);


  Trk::PlaneSurface plsurf (Amg::Vector3D (100, 50, 150),
                            Trk::CurvilinearUVT (Amg::Vector3D (150, 100, 50)));
  Trk::PseudoMeasurementOnTrack pmeas (locpars, cov, plsurf);
  DataVector<const Trk::MeasurementBase> mvec (SG::VIEW_ELEMENTS);
  mvec.push_back (&pmeas);

  Muon::MuonSegment trans1 (locpars,
                      cov,
                      new Trk::PlaneSurface (plsurf),
                      new DataVector<const Trk::MeasurementBase> (mvec),
                      new Trk::FitQuality(fq),
                      Trk::Segment::Muonboy);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
