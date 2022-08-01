/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigConfChainCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigConfChainCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigConfChainCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigConfSig& p1,
              const TrigConfSig& p2)
{
  assert ( p1.getCounter()   == p2.getCounter() );
  assert ( p1.getLogic()     == p2.getLogic() );
  assert ( p1.getLabel()     == p2.getLabel() );
  assert ( p1.getOutputTEs() == p2.getOutputTEs() );
}


void compare (const TrigConfChain& p1,
              const TrigConfChain& p2)
{
  assert ( p1.getName()         == p2.getName() );
  assert ( p1.getLowerName()    == p2.getLowerName() );
  assert ( p1.getId()           == p2.getId() );
  assert ( p1.getLowerId()      == p2.getLowerId() );
  assert ( p1.getCounter()      == p2.getCounter() );
  assert ( p1.getLevelId()      == p2.getLevelId() );
  assert ( p1.getPrescale()     == p2.getPrescale() );
  assert ( p1.getPassThrough()  == p2.getPassThrough() );
  assert ( p1.getLowerIds()     == p2.getLowerIds() );
  assert ( p1.getStreamPS()     == p2.getStreamPS() );
  assert ( p1.getStream()       == p2.getStream() );
  assert ( p1.getGroup()        == p2.getGroup() );
  //assert ( p1.getEBHypo()       == p2.getEBHypo() );  // not saved

  assert ( p1.getSignature().size()    == p2.getSignature().size() );
  for (size_t i = 0; i < p1.getSignature().size(); i++) {
    compare ( p1.getSignature()[i], p2.getSignature()[i] );
  }
}


void testit (const TrigConfChain& trans1)
{
  MsgStream log (0, "test");
  TrigConfChainCnv_p1 cnv;
  TrigConfChain_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigConfChain trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


TrigConfSig makeSig (int i)
{
  TrigConfSig sig (i, i+1, "lab" + std::to_string (i+2));
  sig.addOutputTE (i+3);
  sig.addOutputTE (i+4);
  return sig;
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigConfChain trans1 ("chainname",
                        1, // chain_counter
                        2, // chain_id
                        "HLT",
                        "lower_chainname",
                        3, // lower_chain_counter
                        4, // lower_chain_id
                        5.5, // prescale
                        6.5  // pass_through
                        );

  trans1.addLowerChainId (101);
  trans1.addLowerChainId (103);
  trans1.addLowerChainId (108);

  trans1.getStreamPS().assign ({201.5, 204.5});
  trans1.getStream().assign ({"s1", "s2", "s3"});
  trans1.getGroup().assign ({"g1", "g2"});
  trans1.getEBHypo().assign ({"eb1", "eb2", "eb3"});

  trans1.addSignature (makeSig (100));
  trans1.addSignature (makeSig (200));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigConfChainCnv_p1_test\n";
  test1();
  return 0;
}
