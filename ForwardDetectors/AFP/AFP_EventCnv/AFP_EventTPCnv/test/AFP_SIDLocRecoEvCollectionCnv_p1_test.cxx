/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_SIDLocRecoEvCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_SIDLocRecoEvCollectionCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_SIDLocRecoEvCollectionCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const AFP_SIDLocRecoEvent& p1,
              const AFP_SIDLocRecoEvent& p2)
{
  assert ( p1.getAlgoNum()   == p2.getAlgoNum() );
  assert ( p1.getStationID() == p2.getStationID() );
  assert ( p1.getXposition() == p2.getXposition() );
  assert ( p1.getYposition() == p2.getYposition() );
  assert ( p1.getZposition() == p2.getZposition() );
  assert ( p1.getXslope()    == p2.getXslope() );
  assert ( p1.getYslope()    == p2.getYslope() );
  assert ( p1.getZslope()    == p2.getZslope() );
  assert ( p1.getNHits()     == p2.getNHits()  );
  assert ( p1.getNHoles()    == p2.getNHoles() );
  assert ( p1.getChi2()      == p2.getChi2() );
}


void compare (const AFP_SIDLocRecoEvCollection& p1,
              const AFP_SIDLocRecoEvCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const AFP_SIDLocRecoEvCollection& trans1)
{
  MsgStream log (0, "test");
  AFP_SIDLocRecoEvCollectionCnv_p1 cnv;
  AFP_SIDLocRecoEvCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_SIDLocRecoEvCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_SIDLocRecoEvCollection trans1;
  for (size_t i = 0; i < 5; i++) {
    int offs = i*100;
    trans1.push_back (std::make_unique<AFP_SIDLocRecoEvent>
                      (1+offs, 2+offs,
                       3.5+offs, 4.5+offs, 5.5+offs,
                       6.5+offs, 7.5+offs, 8.5+offs,
                       9+offs, 10+offs, 11.5+offs));
  }

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_SIDLocRecoEvCollectionCnv_p1_test\n";
  test1();
  return 0;
}
