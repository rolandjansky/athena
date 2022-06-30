/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file G4SimTPCnv/test/TrackRecordCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrackRecordCnv_p1.
 */


#undef NDEBUG
#include "G4SimTPCnv/TrackRecordCnv_p1.h"
#include "G4SimTPCnv/TrackRecord_p1.h"
#include "TrackRecord/TrackRecord.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CLHEP::Hep3Vector& p1,
              const CLHEP::Hep3Vector& p2)
{
  assert ( p1.x() == p2.x() );
  assert ( p1.y() == p2.y() );
  assert ( p1.z() == p2.z() );
}


void compare (const TrackRecord& p1,
              const TrackRecord& p2)
{
  assert ( p1.GetPDGCode()  ==  p2.GetPDGCode() );
  assert ( p1.GetEnergy()   ==  p2.GetEnergy()  );
  assert ( p1.GetTime()     ==  p2.GetTime()  );
  assert ( p1.GetBarCode()  ==  p2.GetBarCode()  );
  assert ( p1.GetVolName()  ==  p2.GetVolName()  );
  compare ( p1.GetPosition(), p2.GetPosition() );
  compare ( p1.GetMomentum(), p2.GetMomentum() );
}


void testit (const TrackRecord& trans1)
{
  MsgStream log (0, "test");
  TrackRecordCnv_p1 cnv;
  TrackRecord_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrackRecord trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrackRecord trans1 (123, 124.5,
                      CLHEP::Hep3Vector (10.5, 11.5, 12.5),
                      CLHEP::Hep3Vector (20.5, 21.5, 22.5),
                      125.5,
                      126,
                      "vol");

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "G4SimTPCnv/TrackRecordCnv_p1_test\n";
  test1();
  return 0;
}
