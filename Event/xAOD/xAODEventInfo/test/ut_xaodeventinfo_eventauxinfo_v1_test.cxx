/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file xAODEventInfo/test/ut_xoadeventinfo_eventauxinfo_v1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2018
 * @brief Unit tests for EventAuxInfo_v1.
 */

#undef NDEBUG
#include "xAODEventInfo/versions/EventAuxInfo_v1.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";

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

  assert (!eai.isDecoration (runid));
  assert ( eai.isDecoration (pixid));
  assert ( eai.isDecoration (larid));

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
}


int main()
{
  std::cout << "ut_xaodeventinfo_eventauxinfo_v1_test\n";
  test1();
  return 0;
}

