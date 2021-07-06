/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigSteeringEventTPCnv/test/HLTResultCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for HLTResultCnv_p1.
 */


#undef NDEBUG
#include "TrigSteeringEventTPCnv/HLTResultCnv_p1.h"
#include "TrigSteeringEventTPCnv/HLTResult_p1.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const HLT::HLTResult& p1,
              const HLT::HLTResult& p2)
{
  assert (p1.getHLTResultClassVersion() == p2.getHLTResultClassVersion());
  assert (p1.getLvl1Id() == p2.getLvl1Id());
  assert (p1.isAccepted() == p2.isAccepted());
  assert (p1.isPassThrough() == p2.isPassThrough());
  assert (p1.getHLTStatus() == p2.getHLTStatus());
  assert (p1.getLvlConverterStatus() == p2.getLvlConverterStatus());
  assert (p1.getHLTLevel() == p2.getHLTLevel());
  assert (p1.getNumOfSatisfiedSigs() == p2.getNumOfSatisfiedSigs());
  unsigned int chainCounter1, step1;
  unsigned int chainCounter2, step2;
  p1.getErrorCoordinates (chainCounter1, step1);
  p2.getErrorCoordinates (chainCounter2, step2);
  assert (chainCounter1 == chainCounter2);
  assert (step1 == step2);
  assert (p1.isCreatedOutsideHLT() == p2.isCreatedOutsideHLT());
  assert (p1.isHLTResultTruncated() == p2.isHLTResultTruncated());
  assert (p1.getConfigSuperMasterKey() == p2.getConfigSuperMasterKey());
  assert (p1.getConfigPrescalesKey() == p2.getConfigPrescalesKey());

  assert (p1.getChainResult() == p2.getChainResult());
  assert (p1.getNavigationResult() == p2.getNavigationResult());
#if 0
  // These aren't saved.
  assert (p1.getNavigationResult_DSonly() == p2.getNavigationResult_DSonly());
  assert (p1.getExtras() == p2.getExtras());
  assert (p1.getNavigationResultIDName() == p2.getNavigationResultIDName());
  assert (p1.getNavigationResultIDName_DSonly() == p2.getNavigationResultIDName_DSonly());
  assert (p1.getScoutingMap() == p2.getScoutingMap());
  assert (p1.getNavigationResultCuts() == p2.getNavigationResultCuts());
  assert (p1.getNavigationResultCuts_DSonly() == p2.getNavigationResultCuts_DSonly());
#endif
}


void testit (const HLT::HLTResult& trans1)
{
  MsgStream log (0, "test");
  HLT::HLTResultCnv_p1 cnv;
  HLT::HLTResult_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  HLT::HLTResult trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  Athena_test::Leakcheck check;

  HLT::HLTResult trans1;
  trans1.setLvl1Id (123);
  trans1.setAccepted (true);
  trans1.setPassThrough (false);
  trans1.setHLTStatus (HLT::ErrorCode (234));
  trans1.setLvlConverterStatus (HLT::ErrorCode (345));
  trans1.setHLTLevel (HLT::EF);
  trans1.setNumOfSatisfiedSigs (11);
  trans1.setErrorCoordinates (22, 33);
  trans1.setCreatedOutsideHLT (true);
  trans1.setHLTResultTruncated (false);
  trans1.setConfigSuperMasterKey (456);
  trans1.setConfigPrescalesKey (567);

  trans1.getChainResult() = std::vector<uint32_t> { 14, 15, 16 };
  trans1.getNavigationResult() = std::vector<uint32_t> { 17, 18 };
  trans1.getNavigationResult_DSonly() = std::vector<uint32_t> { 19 };
  trans1.getExtras() = std::vector<uint32_t> { 20, 21, 22, 23 };
  trans1.getNavigationResultCuts() = std::vector<unsigned int> { 24, 25, 26, 27, 28 };
  trans1.getNavigationResultCuts_DSonly() = std::vector<unsigned int> { 29, 30, 31, 32, 33, 34, 35 };

  trans1.getNavigationResultIDName().emplace_back (987, "foo1");
  trans1.getNavigationResultIDName_DSonly().emplace_back (876, "foo2");
  trans1.getNavigationResultIDName_DSonly().emplace_back (765, "foo3");

  std::set<std::pair<CLID, std::string> > s1;
  s1.emplace (654, "foo4");
  s1.emplace (543, "foo5");
  std::set<std::pair<CLID, std::string> > s2;
  s1.emplace (432, "foo6");

  trans1.getScoutingMap()[66] = s1;
  trans1.getScoutingMap()[77] = s2;

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
