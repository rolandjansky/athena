/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkDetDescrTPCnv/test/MaterialStepCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialStepCollectionCnv_p1.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>



using Athena_test::isEqual;


void compare (const Trk::Material& m1, const Trk::Material& m2)
{
  assert (m1.X0 == m2.X0);
  assert (m1.L0 == m2.L0);
  assert (m1.A == m2.A);
  assert (m1.Z == m2.Z);
  assert (m1.rho == m2.rho);
  //assert (m1.dEdX == m2.dEdX);
  //assert (m1.zOaTr == m2.zOaTr);
  if (m1.composition)
    assert (*m1.composition == *m2.composition);
  else
    assert (m2.composition == nullptr);
}


void compare (const Trk::MaterialStep& m1,
              const Trk::MaterialStep& m2)
{
  compare (m1.fullMaterial(), m2.fullMaterial());
  assert (isEqual (m1.hitX(), m2.hitX()));
  assert (isEqual (m1.hitY(), m2.hitY()));
  assert (isEqual (m1.hitZ(), m2.hitZ()));
  assert (isEqual (m1.hitR(), m2.hitR()));
  assert (isEqual (m1.steplength(), m2.steplength()));
}


void compare (const Trk::MaterialStepCollection& m1,
              const Trk::MaterialStepCollection& m2)
{
  assert (m1.size() == m2.size());
  for (size_t i = 0; i < m1.size(); i++)
    compare (*m1[i], *m2[i]);
}


void testit (const Trk::MaterialStepCollection& trans1)
{
  MaterialStepCollectionCnv_p1 cnv;
  Trk::MaterialStepCollection_p1 pers;
  MsgStream log (0, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::MaterialStepCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Trk::MaterialStepCollection coll;
  coll.push_back (new Trk::MaterialStep (1.5, 2.5, 3.5, 4.5, 5.5,
                                         6.5, 7.5, 8.5, 9.5, 10.5) );

  std::vector<unsigned char> elements {1, 2, 3};
  std::vector<unsigned char> fractions {4, 5, 6};

  coll.push_back (new Trk::MaterialStep (11.5, 12.5, 13.5, 14.5, 15.5,
                                         16.5, 17.5, 18.5, 19.5,
                                         elements, fractions,
                                         20.5));
  testit (coll);
}


int main()
{
  test1();
  return 0;
}
