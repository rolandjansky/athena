/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_TDLocRecoEvCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_TDLocRecoEvCollectionCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_TDLocRecoEvCollectionCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const AFP_TDLocRecoEvent& p1,
              const AFP_TDLocRecoEvent& p2)
{
  assert ( p1.getAlgoNum()   == p2.getAlgoNum() );
  assert ( p1.getStationID() == p2.getStationID() );
  assert ( p1.getDetectorID() == p2.getDetectorID() );
  assert ( p1.getTrainID() == p2.getTrainID() );
  assert ( p1.getTrainTime() == p2.getTrainTime() );
  assert ( p1.getSaturation() == p2.getSaturation() );
}


void compare (const AFP_TDLocRecoEvCollection& p1,
              const AFP_TDLocRecoEvCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const AFP_TDLocRecoEvCollection& trans1)
{
  MsgStream log (0, "test");
  AFP_TDLocRecoEvCollectionCnv_p1 cnv;
  AFP_TDLocRecoEvCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_TDLocRecoEvCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_TDLocRecoEvCollection trans1;
  for (size_t i = 0; i < 5; i++) {
    int offs = i*100;
    trans1.push_back (std::make_unique<AFP_TDLocRecoEvent>
                      (1+offs, 2+offs, 3+offs, 4+offs,
                       5.5+offs, 6+offs, 7+offs));
  }

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_TDLocRecoEvCollectionCnv_p1_test\n";
  test1();
  return 0;
}
