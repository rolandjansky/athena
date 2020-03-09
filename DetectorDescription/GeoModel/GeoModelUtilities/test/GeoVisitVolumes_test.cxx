/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeoModelUtilities/test/GeoVisitVolumes_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Unit test for GeoVisitVOlumes.
 */


#undef NDEBUG
#include "GeoModelUtilities/GeoVisitVolumes.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoModelKernel/GeoBox.h"
#include <iostream>
#include <cassert>
#include <memory>
#include <cmath>


PVConstLink makeTest()
{
  GeoMaterial* m0 = new GeoMaterial ("mat", 1);
  GeoShape* s0 = new GeoBox (1, 1, 1);
  GeoLogVol* l0 = new GeoLogVol ("l0", s0, m0);
  GeoFullPhysVol* v0 = new GeoFullPhysVol(l0);

  GeoGenfun::Variable K;
  GeoGenfun::GENFUNCTION f = K;
  GeoXF::TRANSFUNCTION t =  GeoXF::Pow(GeoTrf::TranslateY3D(1.0),f);

  GeoTrf::TranslateX3D t2(1);

  v0->add (new GeoIdentifierTag (10));
  v0->add (new GeoNameTag ("l1"));
  v0->add (new GeoPhysVol (new GeoLogVol ("l1", s0, m0)));
  v0->add (new GeoIdentifierTag (20));
  v0->add (new GeoNameTag ("l2"));
  v0->add (new GeoTransform (t2));
  v0->add (new GeoPhysVol (new GeoLogVol ("l2", s0, m0)));
  v0->add (new GeoSerialIdentifier (100));
  v0->add (new GeoSerialDenominator ("l3"));
  v0->add (new GeoTransform (t2));
  v0->add (new GeoSerialTransformer (new GeoPhysVol (new GeoLogVol ("l3", s0, m0)),
                                     &t, 3));
  v0->add (new GeoIdentifierTag (200));
  v0->add (new GeoNameTag ("l4"));
  v0->add (new GeoPhysVol (new GeoLogVol ("l4", s0, m0)));
  return v0;
}


class TestVisitor
{
public:
  TestVisitor (PVConstLink v0) : m_v0 (v0) {}
  void operator() (int id,
                   const std::string& name,
                   PVConstLink volume,
                   const GeoTrf::Transform3D& transform,
                   const GeoTrf::Transform3D& defTransform);

  PVConstLink m_v0;
  int m_icount = 0;
};


void compareTransform (const GeoTrf::Transform3D& a,
                       const GeoTrf::Transform3D& b)
{
  assert (a.translation().isApprox(b.translation(), 1e-6));
  assert (a.rotation().isApprox(b.rotation(), 1e-6));
}


void TestVisitor::operator() (int id,
                              const std::string& name,
                              PVConstLink volume,
                              const GeoTrf::Transform3D& transform,
                              const GeoTrf::Transform3D& defTransform)
{
  assert (m_v0->getIdOfChildVol (m_icount) == id);
  assert (m_v0->getNameOfChildVol (m_icount) == name);
  assert (m_v0->getChildVol (m_icount) == volume);
  compareTransform (m_v0->getXToChildVol (m_icount), transform);
  compareTransform (m_v0->getDefXToChildVol (m_icount), defTransform);
  ++m_icount;
}


void test1()
{
  std::cout << "test1\n";
  PVConstLink v0 = makeTest();

  TestVisitor visitor (v0);
  geoVisitVolumes (visitor, &*v0);
  assert (visitor.m_icount == 0);

  geoVisitVolumes ([&] (int id,
                        const std::string& name,
                        PVConstLink volume,
                        const GeoTrf::Transform3D& transform,
                        const GeoTrf::Transform3D& defTransform)
                   { visitor (id, name, volume, transform, defTransform); },
                   &*v0);
  assert (visitor.m_icount == 6);
}


void test2()
{
  std::cout << "test2\n";
  PVConstLink v0 = makeTest();

  GeoVolumeVec_t vols = geoGetVolumes (&*v0);

  GeoTrf::Transform3D xform = GeoTrf::Transform3D::Identity();
  assert (vols.size() == 6);
  assert (vols[0].first->getLogVol()->getName() == "l1");
  compareTransform (vols[0].second, xform);

  xform = xform * GeoTrf::TranslateX3D(1);
  assert (vols[1].first->getLogVol()->getName() == "l2");
  compareTransform (vols[1].second, xform);

  assert (vols[2].first->getLogVol()->getName() == "l3");
  compareTransform (vols[2].second, xform);

  xform = xform * GeoTrf::TranslateY3D(1);
  assert (vols[3].first->getLogVol()->getName() == "l3");
  compareTransform (vols[3].second, xform);

  xform = xform * GeoTrf::TranslateY3D(1);
  assert (vols[4].first->getLogVol()->getName() == "l3");
  compareTransform (vols[4].second, xform);

  assert (vols[5].first->getLogVol()->getName() == "l4");
  compareTransform (vols[5].second, GeoTrf::Transform3D::Identity());
}


int main()
{
  std::cout << "GeoModelUtilities/test/GeoVisitVolumes_test\n";
  test1();
  test2();
  return 0;
}
