/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigConfSeqCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigConfSeqCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigConfSeqCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigConfAlg& p1,
              const TrigConfAlg& p2)
{
  assert ( p1.getIndex()    == p2.getIndex() );
  assert ( p1.getPosition() == p2.getPosition() );
  assert ( p1.getNameId()   == p2.getNameId() );
  assert ( p1.getTypeId()   == p2.getTypeId() );
  assert ( p1.getName()     == p2.getName() );
  assert ( p1.getType()     == p2.getType() );
}


void compare (const TrigConfSeq& p1,
              const TrigConfSeq& p2)
{
  assert ( p1.getName()     == p2.getName() );
  assert ( p1.getIndex()    == p2.getIndex() );
  assert ( p1.getId()       == p2.getId() );
  assert ( p1.getTopoTE()   == p2.getTopoTE() );
  assert ( p1.getInputTEs() == p2.getInputTEs() );

  assert ( p1.getAlg().size() == p2.getAlg().size() );
  for (size_t i = 0; i < p1.getAlg().size(); i++) {
    compare ( p1.getAlg()[i], p2.getAlg()[i] );
  }
}


void testit (const TrigConfSeq& trans1)
{
  MsgStream log (0, "test");
  TrigConfSeqCnv_p1 cnv;
  TrigConfSeq_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigConfSeq trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


TrigConfAlg makeAlg (int i)
{
  return TrigConfAlg (i+1, i+2,
                      "nm" + std::to_string(i),
                      "typ" + std::to_string(i),
                      i+3, i+4);
}

void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigConfSeq trans1 (1, 2, "nm");
  trans1.setTopoTE (3);
  trans1.addInputTE (4);
  trans1.addInputTE (5);
  trans1.addInputTE (6);
  trans1.addAlg (makeAlg (100));
  trans1.addAlg (makeAlg (110));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigConfSeqCnv_p1_test\n";
  test1();
  return 0;
}
