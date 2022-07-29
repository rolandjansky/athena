/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonEventCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonEventCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonEventCnv_p1.h"
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


void compare (const TrigMonRoi& p1,
              const TrigMonRoi& p2)
{
  assert ( p1.getWord()   == p2.getWord() );
  assert ( p1.getVarKey() == p2.getVarKey() );
  assert ( p1.getVarVal() == p2.getVarVal() );
}


void compare (const TrigMonAlg& p1,
              const TrigMonAlg& p2)
{
  assert ( p1.getByte() == p2.getByte() );
  assert ( p1.getWord() == p2.getWord() );
}


void compare (const TrigMonSeq& p1,
              const TrigMonSeq& p2)
{
  assert ( p1.getEncoded() == p2.getEncoded() );
  assert ( p1.getVarKey()  == p2.getVarKey() );
  assert ( p1.getVarVal()  == p2.getVarVal() );

  assert ( p1.getAlg().size() == p2.getAlg().size() );
  for (size_t i = 0; i < p1.getAlg().size(); i++) {
    compare (p1.getAlg()[i], p2.getAlg()[i]);
  }
}


template <class T>
void compare (const std::vector<T>& p1,
              const std::vector<T>& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare ( p1[i], p2[i] );
  }
}


void compare (const TrigMonTE& p1,
              const TrigMonTE& p2)
{
  assert ( p1.getId()           == p2.getId() );
  assert ( p1.getIndex()        == p2.getIndex() );
  assert ( p1.getType()         == p2.getType() );
  assert ( p1.getActiveState()  == p2.getActiveState() );
  assert ( p1.getErrorState()   == p2.getErrorState() );
  assert ( p1.isTerminalNode()  == p2.isTerminalNode() );
  assert ( p1.isOutputL2Node()  == p2.isOutputL2Node() );
  assert ( p1.isOutputEFNode()  == p2.isOutputEFNode() );
  assert ( p1.isTopologicalTE() == p2.isTopologicalTE() );
  assert ( p1.getChildIndex()   == p2.getChildIndex() );
  assert ( p1.getParentIndex()  == p2.getParentIndex() );
  assert ( p1.getRoiId()        == p2.getRoiId() );
  assert ( p1.getClid()         == p2.getClid() );
  assert ( p1.getVarKey()       == p2.getVarKey() );
  assert ( p1.getVarVal()       == p2.getVarVal() );
}


void compare (const TrigMonEvent& p1,
              const TrigMonEvent& p2)
{
  assert ( p1.getEvent()   == p2.getEvent() );
  assert ( p1.getLumi()    == p2.getLumi() );
  assert ( p1.getBunchId() == p2.getBunchId() );
  assert ( p1.getRun()     == p2.getRun() );
  assert ( p1.getSec()     == p2.getSec() );
  assert ( p1.getNanoSec() == p2.getNanoSec() );
  assert ( p1.getWord()    == p2.getWord() );
  assert ( p1.getL1Item()  == p2.getL1Item() );
  assert ( p1.getChain()   == p2.getChain() );
  assert ( p1.getVarKey()  == p2.getVarKey() );
  assert ( p1.getVarVal()  == p2.getVarVal() );

  compare ( p1.getVec<TrigMonROB>(), p2.getVec<TrigMonROB>() );
  compare ( p1.getVec<TrigMonRoi>(), p2.getVec<TrigMonRoi>() );
  compare ( p1.getVec<TrigMonSeq>(), p2.getVec<TrigMonSeq>() );
  compare ( p1.getVec<TrigMonTE>(),  p2.getVec<TrigMonTE>() );
}


void testit (const TrigMonEvent& trans1)
{
  MsgStream log (0, "test");
  TrigMonEventCnv_p1 cnv;
  TrigMonEvent_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonEvent trans2;
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

  
TrigMonROB makeMonROB (int i)
{
  TrigMonROB rob (i+123, i+234, i+345, i+456, i+567);
  rob.addWord (i+312);
  rob.addWord (i+534);

  rob.addData (makeMonROBData (i*10));
  rob.addData (makeMonROBData (i*20));
  rob.addData (makeMonROBData (i*30));

  return rob;
}


TrigMonRoi makeMonRoi (int i)
{
  TrigMonRoi roi (i+123);
  roi.addWord (i+234);
  roi.addWord (i+345);
  roi.setEtaPhi (1.5, 2.5);
  roi.setRoIArea (0.25, 0.5);
  roi.addVar (TrigMonVar (i+13, i+12.5));
  return roi;
}


TrigMonAlg makeMonAlg (int i)
{
  TrigMonAlg alg ((i+123)%128, (i%100)&1);
  alg.addRoiId (i+45);
  alg.addRoiId (i+67);
  alg.addTimer (TrigMonTimer (i+345, i+543),
                TrigMonTimer (i+654656, i+23423));
  return alg;
}


TrigMonSeq makeMonSeq (int i)
{
  TrigMonSeq seq (i+123);
  seq.addVar (TrigMonVar (i+4, i+4.5));
  seq.addVar (TrigMonVar (i+6, i+7.5));
  seq.addAlg (makeMonAlg (i*10));
  seq.addAlg (makeMonAlg (i*20));
  seq.addAlg (makeMonAlg (i*30));
  return seq;
}


TrigMonTE makeMonTE (int i)
{
  TrigMonTE te (i+123, i+345);
  te.setType (TrigMonTE::kROI);
  te.addState (TrigMonTE::activeState);
  te.addState (TrigMonTE::terminalNode);
  te.addState (TrigMonTE::topologicalTE);
  te.addChildIndex (i+321);
  te.addChildIndex (i+432);
  te.addParentIndex (i+543);
  te.addParentIndex (i+645);
  te.addParentIndex (i+765);
  te.addRoiId (i+143);
  te.addRoiId (i+93);
  te.addClid (i+23456);
  te.addClid (i+53453);
  te.addClid (i+123123);
  te.addVar (TrigMonVar (i+3, i+5.5));
  te.addVar (TrigMonVar (i+8, i+10.5));
  return te;
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigMonEvent trans1;
  trans1.setEventID (123, 234, 345, 456, 567, 678);
  trans1.addWord (321);
  trans1.addWord (432);
  trans1.addWord (534);
  trans1.getL1Item().assign ({4321, 1234});
  trans1.getChain().assign ({12345, 9876, 423456});
  trans1.addVar (4, 7.5);
  trans1.addVar (5, 8.5);

  trans1.add (makeMonROB (100));
  trans1.add (makeMonROB (200));
  trans1.add (makeMonROB (300));
  trans1.add (makeMonRoi (400));
  trans1.add (makeMonRoi (500));
  trans1.add (makeMonSeq (600));
  trans1.add (makeMonSeq (700));
  trans1.add (makeMonSeq (800));
  trans1.add (makeMonTE  (900));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonEventCnv_p1_test\n";
  test1();
  return 0;
}
