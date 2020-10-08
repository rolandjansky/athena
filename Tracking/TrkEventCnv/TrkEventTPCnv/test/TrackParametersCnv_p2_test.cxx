/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/TrackParametersCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p2.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


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
              
void testit (const Trk::TrackParameters& trans1)
{
  MsgStream log (0, "test");
  TrackParametersCnv_p2 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::TrackParameters_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::TrackParameters* trans2 = cnv.createTransient (&pers, log);
  compare (trans1, *trans2);
  delete trans2;
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AmgSymMatrix(5) cov;
  for (int i=0; i < 5; i++)
    for (int j=0; j < 5; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  MsgStream log (0, "test");
  Trk::PerigeeSurface psurf (Amg::Vector3D (50, 100, 150));
  Trk::Perigee trans1 (100, 200, 1.5, 0.5, 1e-3, psurf, nullptr);
  testit (trans1);

  Trk::Perigee trans1c (100, 200, 1.5, 0.5, 1e-3, psurf,
                        new AmgSymMatrix(5) (cov));
  testit (trans1c);

  Trk::PlaneSurface plsurf (Amg::Vector3D (100, 50, 150),
                            Trk::CurvilinearUVT (Amg::Vector3D (150, 100, 50)));
  Trk::AtaPlane trans2 (200, 100, 1.4, 0.7, 2e-3, plsurf, nullptr);
  testit (trans2);

  Trk::AtaPlane trans2c (200, 100, 1.4, 0.7, 2e-3, plsurf,
                         new AmgSymMatrix(5) (cov));
  testit (trans2c);

  Trk::StraightLineSurface slsurf
    (std::make_unique<Amg::Transform3D>(Amg::getRotateX3D (0.5)));
  Trk::AtaStraightLine trans3 (200, 100, 1.4, 0.7, 2e-3, slsurf, nullptr);
  testit (trans3);

  Trk::AtaStraightLine trans3c (200, 100, 1.4, 0.7, 2e-3, slsurf,
                                new AmgSymMatrix(5) (cov));
  testit (trans3c);

  Trk::ConeSurface csurf
    (std::make_unique<Amg::Transform3D>(Amg::getRotateX3D (0.5)).release(), 0.45);
  Trk::AtaCone trans4 (200, 100, 1.4, 0.7, 2e-3, csurf, nullptr);
  testit (trans4);

  Trk::AtaCone trans4c (200, 100, 1.4, 0.7, 2e-3, csurf,
                        new AmgSymMatrix(5) (cov));
  testit (trans4c);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
