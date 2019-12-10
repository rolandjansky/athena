/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_SIDLocRecoEventCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_SIDLocRecoEventCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_SIDLocRecoEventCnv_p1.h"
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


void testit (const AFP_SIDLocRecoEvent& trans1)
{
  MsgStream log (0, "test");
  AFP_SIDLocRecoEventCnv_p1 cnv;
  AFP_SIDLocRecoEvent_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_SIDLocRecoEvent trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_SIDLocRecoEvent trans1 (1, 2,
                              3.5, 4.5, 5.5,
                              6.5, 7.5, 8.5,
                              9, 10, 11.5);

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_SIDLocRecoEventCnv_p1_test\n";
  test1();
  return 0;
}
