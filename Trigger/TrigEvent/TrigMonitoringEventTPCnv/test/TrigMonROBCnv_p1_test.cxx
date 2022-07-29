/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonROBCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonROBCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonROBCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigMonROBData& p1,
              const TrigMonROBData& p2)
{
  assert ( p1.getROBId()        == p2.getROBId() );
  assert ( p1.getEncodedState() == p2.getEncodedState() );
  assert ( p1.getROBSize()      == p2.getROBSize() );
}


void compare (const TrigMonROB& p1,
              const TrigMonROB& p2)
{
  assert ( p1.getWord() == p2.getWord() );

  assert ( p1.getData().size() == p2.getData().size() );
  for (size_t i = 0; i < p1.getData().size(); i++) {
    compare (p1.getData()[i], p2.getData()[i]);
  }
}


void testit (const TrigMonROB& trans1)
{
  MsgStream log (0, "test");
  TrigMonROBCnv_p1 cnv;
  TrigMonROB_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonROB trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


TrigMonROBData makeMonROBData (int i)
{
  TrigMonROBData data (i+123, i+456);
  data.setHistory (static_cast<TrigMonROBData::History> (i%16));
  if ((i&0x10) == 0) data.setStatusOK();
  if ((i&0x20) == 0) data.setStatusPREFETCH();
  return data;
}

  
void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigMonROB trans1 (123, 234, 345, 456, 567);
  trans1.addWord (312);
  trans1.addWord (534);

  trans1.addData (makeMonROBData (100));
  trans1.addData (makeMonROBData (200));
  trans1.addData (makeMonROBData (300));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonROBCnv_p1_test\n";
  test1();
  return 0;
}
