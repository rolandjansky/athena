/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file GeoModelUtilities/test/GeoGetIds_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Unit test for GeoGetIds.
 */


#undef NDEBUG
#include "GeoModelUtilities/GeoGetIds.h"
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

  v0->add (new GeoIdentifierTag (10));
  v0->add (new GeoPhysVol (new GeoLogVol ("l1", s0, m0)));
  v0->add (new GeoIdentifierTag (20));
  v0->add (new GeoPhysVol (new GeoLogVol ("l2", s0, m0)));
  v0->add (new GeoSerialIdentifier (100));
  v0->add (new GeoSerialTransformer (new GeoPhysVol (new GeoLogVol ("l3", s0, m0)),
                                     &t, 3));
  v0->add (new GeoIdentifierTag (200));
  v0->add (new GeoPhysVol (new GeoLogVol ("l4", s0, m0)));
  return v0;
}


void test1()
{
  std::cout << "test1\n";
  PVConstLink v0 = makeTest();

  std::vector<int> ids;
  geoGetIds ([&](int id) { ids.push_back (id); }, &*v0);
  assert (ids == (std::vector<int>{10, 20, 100, 101, 102, 200}));
}


int main()
{
  std::cout << "GeoModelUtilities/test/GeoGetIds_test\n";
  test1();
  return 0;
}
