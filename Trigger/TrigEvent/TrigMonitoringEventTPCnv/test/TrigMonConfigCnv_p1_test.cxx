/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonConfigCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonConfigCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonConfigCnv_p1.h"
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


void compare (const TrigMonConfig& p1,
              const TrigMonConfig& p2)
{
  assert ( p1.getEvent()   == p2.getEvent() );
  assert ( p1.getLumi()    == p2.getLumi() );
  assert ( p1.getRun()     == p2.getRun() );
  assert ( p1.getSec()     == p2.getSec() );
  assert ( p1.getNanoSec() == p2.getNanoSec() );

  assert ( p1.getMasterKey()      == p2.getMasterKey() );
  assert ( p1.getHLTPrescaleKey() == p2.getHLTPrescaleKey() );
  assert ( p1.getLV1PrescaleKey() == p2.getLV1PrescaleKey() );

  assert ( p1.getVarName()        == p2.getVarName() );
  assert ( p1.getVarId()          == p2.getVarId() );
  assert ( p1.getPairKey()        == p2.getPairKey() );
  assert ( p1.getPairVal()        == p2.getPairVal() );

  assert ( p1.getVec<TrigConfChain>().size() == p2.getVec<TrigConfChain>().size());
  for (size_t i = 0; i < p1.getVec<TrigConfChain>().size(); i++) {
    compare (p1.getVec<TrigConfChain>()[i], p2.getVec<TrigConfChain>()[i]);
  }
  
  assert ( p1.getVec<TrigConfSeq>().size() == p2.getVec<TrigConfSeq>().size());
  for (size_t i = 0; i < p1.getVec<TrigConfSeq>().size(); i++) {
    compare (p1.getVec<TrigConfSeq>()[i], p2.getVec<TrigConfSeq>()[i]);
  }
}


void testit (const TrigMonConfig& trans1)
{
  MsgStream log (0, "test");
  TrigMonConfigCnv_p1 cnv;
  TrigMonConfig_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonConfig trans2;
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


TrigConfChain makeChain (int i)
{
  TrigConfChain chain ("chainname" + std::to_string(i),
                       i+1, // chain_counter
                       i+2, // chain_id
                       "HLT",
                       "lower_chainname" + std::to_string(i),
                       i+3, // lower_chain_counter
                       i+4, // lower_chain_id
                       i+5.5, // prescale
                       i+6.5  // pass_through
                       );

  chain.addLowerChainId (i+101);
  chain.addLowerChainId (i+103);
  chain.addLowerChainId (i+108);

  chain.getStreamPS().assign ({i+201.5f, i+204.5f});
  chain.getStream().assign ({"s1", "s2", "s3"});
  chain.getGroup().assign ({"g1", "g2"});
  chain.getEBHypo().assign ({"eb1", "eb2", "eb3"});

  chain.addSignature (makeSig (i*10));
  chain.addSignature (makeSig (i*20));

  return chain;
}


TrigConfAlg makeAlg (int i)
{
  return TrigConfAlg ((i+1) % 65536, (i+2) % 128,
                      "nm" + std::to_string(i),
                      "typ" + std::to_string(i),
                      i+3, i+4);
}


TrigConfSeq makeSeq (int i)
{
  TrigConfSeq seq (i+1, i+2, "nm" + std::to_string(i));
  seq.setTopoTE (i+3);
  seq.addInputTE (i+4);
  seq.addInputTE (i+5);
  seq.addInputTE (i+6);
  seq.addAlg (makeAlg (i*10));
  seq.addAlg (makeAlg (i*20));
  return seq;
}


void test1()
{
  std::cout << "test1\n";
  //Athena_test::Leakcheck check;

  TrigMonConfig trans1;
  trans1.setEventID (1, 2, 3, 4, 5);
  trans1.setTriggerKeys (11, 12, 13);
  trans1.addValue ("k21", "v21");
  trans1.addValue ("k22", "v22");
  trans1.add (TrigConfVar ("v31", 31));
  trans1.add (TrigConfVar ("v32", 32));
  trans1.add (TrigConfVar ("v33", 33));
  trans1.add (makeChain (100));
  trans1.add (makeChain (200));
  trans1.add (makeSeq (300));
  trans1.add (makeSeq (400));
  trans1.add (makeSeq (500));
  
  testit (trans1);
}


int main()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonConfigCnv_p1_test\n";
  test1();
  return 0;
}
