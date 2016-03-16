/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigInDetTrackCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigInDetTrackCnv_p5.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp4.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigInDetTrackCollection& p1,
              const TrigInDetTrackCollection& p2)
{
  assert (p1.RoI_ID() == p2.RoI_ID());
  assert (p1.size() == p2.size());
}


void testit (const TrigInDetTrackCollection& trans1)
{
  MsgStream log (0, "test");
  TrigInDetTrackCollectionCnv_p1 cnv;
  TrigInDetTrackCollectionCnv_tlp4 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigInDetTrackCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigInDetTrackCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigInDetTrackCollection trans1;
  trans1.RoI_ID (12345);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
