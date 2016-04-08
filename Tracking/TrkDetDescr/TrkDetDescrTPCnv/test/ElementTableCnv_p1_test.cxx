/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkDetDescrTPCnv/test/ElementTableCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkGeometry/ElementTableCnv_p1.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>



void compare (const Trk::Material& m1, const Trk::Material& m2)
{
  assert (m1.X0 == m2.X0);
  assert (m1.L0 == m2.L0);
  assert (m1.A == m2.A);
  assert (m1.Z == m2.Z);
  assert (m1.rho == m2.rho);
  assert (m1.dEdX == m2.dEdX);
  assert (m1.zOaTr == m2.zOaTr);
  if (m1.composition)
    assert (*m1.composition == *m2.composition);
  else
    assert (m2.composition == nullptr);
}


void compare (const Trk::ElementTable& e1, const Trk::ElementTable& e2)
{
  size_t sz = e1.size();
  assert (e2.size() == sz);

  for (size_t i=0; i < sz; i++) {
    assert (e1.contains(i) == e2.contains(i));
    if (e1.contains(i)) {
      assert (e1.elementName(i) == e2.elementName(i));
      compare (*e1.element(i), *e2.element(i));
    }
  }
}


void testit (const Trk::ElementTable& trans1)
{
  ElementTableCnv_p1 cnv;
  Trk::ElementTable_p1 pers;
  MsgStream log (0, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::ElementTable trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Trk::ElementTable e;
  e.addElement (Trk::Material ( 1.5,  2.5,  3.5, 10,  4.5), "ten");
  e.addElement (Trk::Material (11.5, 12.5, 13.5, 20, 14.5), "twenty");
  e.addElement (Trk::Material (21.5, 22.5, 23.5, 30, 24.5), "thirty");
  testit (e);
}


int main()
{
  test1();
  return 0;
}
