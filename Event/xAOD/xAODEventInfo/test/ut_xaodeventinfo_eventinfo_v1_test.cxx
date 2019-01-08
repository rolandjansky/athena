/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file xAODEventInfo/test/ut_xoadeventinfo_eventinfo_v1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2018
 * @brief Unit tests for EventInfo_v1.  (Incomplete.)
 */

#undef NDEBUG
#include "xAODEventInfo/versions/EventInfo_v1.h"
#include "xAODEventInfo/versions/EventAuxInfo_v1.h"
//#include "AthContainers/AuxTypeRegistry.h"
//#include "AthContainers/exceptions.h"
//#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>


// Testing detector flags.
void test1()
{
  std::cout << "test1\n";

  typedef xAOD::EventInfo_v1 EI;

  EI ei;
  xAOD::EventAuxInfo_v1 eia;
  ei.setStore (&eia);

  assert (ei.eventFlags (EI::SCT) == 0);
  assert (ei.isEventFlagBitSet (EI::SCT, 1) == false);
  assert (ei.setEventFlags (EI::SCT, 0x43) == true);
  assert (ei.eventFlags (EI::SCT) == 0x43);
  assert (ei.isEventFlagBitSet (EI::SCT, 1) == true);
  assert (ei.isEventFlagBitSet (EI::SCT, 2) == false);
  assert (ei.setEventFlags (EI::SCT, 0x34) == true);
  assert (ei.eventFlags (EI::SCT) == 0x34);
  assert (ei.isEventFlagBitSet (EI::SCT, 1) == false);
  assert (ei.isEventFlagBitSet (EI::SCT, 2) == true);
  assert (ei.setEventFlagBit (EI::SCT, 1));
  assert (ei.eventFlags (EI::SCT) == 0x36);
  assert (ei.isEventFlagBitSet (EI::SCT, 1) == true);
  assert (ei.isEventFlagBitSet (EI::SCT, 2) == true);
  assert (ei.resetEventFlagBit (EI::SCT, 2));
  assert (ei.eventFlags (EI::SCT) == 0x32);
  assert (ei.isEventFlagBitSet (EI::SCT, 1) == true);
  assert (ei.isEventFlagBitSet (EI::SCT, 2) == false);

  assert (ei.errorState (EI::SCT) == EI::NotSet);
  assert (ei.setErrorState (EI::SCT, EI::Error) == true);
  assert (ei.errorState (EI::SCT) == EI::Error);
  assert (ei.eventFlags (EI::SCT) == 0x32);

  ei.container()->lock();
  eia.lock();
  const EI& cei = ei;
  assert (cei.updateEventFlagBit (EI::SCT, 0) == true);
  assert (cei.eventFlags (EI::SCT) == 0x33);
  assert (cei.updateEventFlags (EI::SCT, 0x08) == true);
  assert (cei.eventFlags (EI::SCT) == 0x3b);

  assert (cei.errorState (EI::TRT) == EI::NotSet);
  assert (cei.updateErrorState (EI::TRT, EI::Warning) == true);
  assert (cei.errorState (EI::TRT) == EI::Warning);
  assert (cei.updateErrorState (EI::TRT, EI::Warning) == true);
  assert (cei.errorState (EI::TRT) == EI::Warning);
  assert (cei.updateErrorState (EI::TRT, EI::NotSet) == false);
  assert (cei.errorState (EI::TRT) == EI::Warning);

#if 0  
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  xAOD::EventAuxInfo_v1 eai;

  SG::auxid_t runid = r.findAuxID ("runNumber");
  SG::auxid_t pixid = r.findAuxID ("pixelFlags");
  SG::auxid_t sctid = r.findAuxID ("sctFlags");
  SG::auxid_t trtid = r.findAuxID ("trtFlags");
  SG::auxid_t larid = r.findAuxID ("larFlags");

  uint32_t* run = reinterpret_cast<uint32_t*> (eai.getData(runid, 1, 1));
  uint32_t* pix = reinterpret_cast<uint32_t*> (eai.getData(pixid, 1, 1));
  uint32_t* sct = reinterpret_cast<uint32_t*> (eai.getData(sctid, 1, 1));
  uint32_t* trt = reinterpret_cast<uint32_t*> (eai.getData(trtid, 1, 1));
  uint32_t* lar = reinterpret_cast<uint32_t*> (eai.getData(larid, 1, 1));
  assert (*run == 0);
  assert (*pix == 0);
  assert (*sct == 0);
  assert (*trt == 0);
  assert (*lar == 0);

  assert (run == eai.getDecoration(runid, 1, 1));
  assert (pix == eai.getDecoration(pixid, 1, 1));
  assert (sct == eai.getDecoration(sctid, 1, 1));

  eai.lock();
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eai.getDecoration(runid, 1, 1));
  assert (pix == eai.getDecoration(pixid, 1, 1));
  assert (sct == eai.getDecoration(sctid, 1, 1));

  eai.lockDecoration (sctid);
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eai.getDecoration(runid, 1, 1));
  assert (pix == eai.getDecoration(pixid, 1, 1));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eai.getDecoration(sctid, 1, 1));

  eai.toTransient();
  assert (trt == eai.getDecoration(trtid, 1, 1));
  assert (lar == eai.getDecoration(larid, 1, 1));

  *lar = 1;
  eai.toTransient();
  assert (trt == eai.getDecoration(trtid, 1, 1));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eai.getDecoration(larid, 1, 1));
#endif
}


int main()
{
  std::cout << "ut_xaodeventinfo_eventinfo_v1_test\n";
  test1();
  return 0;
}

