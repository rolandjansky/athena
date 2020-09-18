/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/SegmentCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventPrimitives/FitQuality.h"
//#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
//#include "TrkMaterialOnSegment/MaterialEffectsOnSegment.h"
#include "TrkEventTPCnv/SegmentCollectionCnv_tlp3.h"
//#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


class TestSegment
  : public Trk::Segment
{
public:
  TestSegment() : m_surf(nullptr) {}
  TestSegment( const Trk::LocalParameters& locpars,
               const Amg::MatrixX& locerr,
               DataVector<const Trk::MeasurementBase>* measurements,
               Trk::FitQuality* fitq,
               Trk::Segment::Author author,
               const Trk::Surface& sf)
    : Trk::Segment (locpars, locerr, measurements, fitq, author),
      m_surf (&sf)
  {}
  
  virtual Trk::Segment* clone() const override
  { std::abort(); }
  virtual const Trk::Surface& associatedSurface() const override
  { return *m_surf; }
  virtual const Amg::Vector3D& globalPosition() const override
  {
    static const Amg::Vector3D v (1, 2, 3);
    return v;
  }
  virtual MsgStream&    dump( MsgStream& /*out*/ ) const override
  { std::abort(); }
  virtual std::ostream& dump( std::ostream& /*out*/ ) const override
  { std::abort(); }

  const Trk::Surface* m_surf;
};


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


void testit (const Trk::Segment& trans1)
{
  MsgStream log (0, "test");
  SegmentCnv_p1 cnv;
  SegmentCollectionCnv_tlp3 tlcnv;
  //cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::Segment_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TestSegment trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Trk::LocalParameters locpars (1.5, 2.5, 3.5, 4.5, 5.5);
  Trk::FitQuality fq (1.5, 2.5);

  Amg::MatrixX cov(5,5);
  for (int i=0; i < 5; i++)
    for (int j=0; j < 5; j++)
      cov(i,j) = 100*(i+1)*(j+1);


  Trk::PerigeeSurface psurf (Amg::Vector3D (50, 100, 150));
  Trk::PseudoMeasurementOnTrack pmeas (locpars, cov, psurf);
  DataVector<const Trk::MeasurementBase> mvec (SG::VIEW_ELEMENTS);
  mvec.push_back (&pmeas);

  TestSegment trans1 (locpars,
                      cov,
                      new DataVector<const Trk::MeasurementBase> (mvec),
                      new Trk::FitQuality(fq),
                      Trk::Segment::Muonboy,
                      psurf);
  testit (trans1);

#if 0
  Trk::PerigeeSurface psurf (Amg::Vector3D (50, 100, 150));
  Trk::LocalParameters parms1 (1.5, 2.5, 3.5, 4.5, 5.5);
  Trk::PseudoMeasurementOnSegment pmeas (parms1, cov, psurf);
  Trk::Perigee perigee (100, 200, 1.5, 0.5, 1e-3, psurf, nullptr);
  Trk::FitQuality fq (10, 20);
  Trk::MaterialEffectsOnSegment me (12.5, psurf);

  Trk::SegmentStateOnSurface tsos1 (new Trk::PseudoMeasurementOnSegment (pmeas),
                                  new Trk::Perigee (perigee),
                                  new Trk::FitQuality (fq),
                                  new Trk::MaterialEffectsOnSegment (me),
                                  nullptr);

  DataVector<const Trk::SegmentStateOnSurface> tsvec (SG::VIEW_ELEMENTS);
  tsvec.push_back (&tsos1);

  std::bitset<Trk::SegmentInfo::NumberOfSegmentProperties> properties;
  std::bitset<Trk::SegmentInfo::NumberOfSegmentRecoInfo> patrec;
  properties[0] = true;
  properties[2] = true;
  properties[3] = true;
  patrec[1] = true;
  patrec[3] = true;
  Trk::SegmentInfo info (Trk::SegmentInfo::GlobalChi2Fitter,
                       Trk::electron,
                       properties,
                       patrec);

  Trk::Segment trans1 (info,
                     new DataVector<const Trk::SegmentStateOnSurface> (tsvec),
                     new Trk::FitQuality (fq));
  testit (trans1);
  #endif
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
