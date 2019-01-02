/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file xAODEventInfo/test/ut_xoadeventinfo_eventinfoauxcontainer_v1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2018
 * @brief Unit tests for EventInfoAuxContainer_v1.
 */

#undef NDEBUG
#include "xAODEventInfo/versions/EventInfoAuxContainer_v1.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  xAOD::EventInfoAuxContainer_v1 eiac;

  SG::auxid_t runid = r.findAuxID ("runNumber");
  SG::auxid_t pixid = r.findAuxID ("pixelFlags");
  SG::auxid_t sctid = r.findAuxID ("sctFlags");
  SG::auxid_t trtid = r.findAuxID ("trtFlags");
  SG::auxid_t larid = r.findAuxID ("larFlags");

  uint32_t* run = reinterpret_cast<uint32_t*> (eiac.getData(runid, 5, 5));
  uint32_t* pix = reinterpret_cast<uint32_t*> (eiac.getData(pixid, 5, 5));
  uint32_t* sct = reinterpret_cast<uint32_t*> (eiac.getData(sctid, 5, 5));
  uint32_t* trt = reinterpret_cast<uint32_t*> (eiac.getData(trtid, 5, 5));
  uint32_t* lar = reinterpret_cast<uint32_t*> (eiac.getData(larid, 5, 5));
  assert (*run == 0);
  assert (*pix == 0);
  assert (*sct == 0);
  assert (*trt == 0);
  assert (*lar == 0);

  assert (run == eiac.getDecoration(runid, 5, 5));
  assert (pix == eiac.getDecoration(pixid, 5, 5));
  assert (sct == eiac.getDecoration(sctid, 5, 5));

  assert (!eiac.isDecoration (runid));
  assert ( eiac.isDecoration (pixid));
  assert ( eiac.isDecoration (larid));

  eiac.lock();
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eiac.getDecoration(runid, 5, 5));
  assert (pix == eiac.getDecoration(pixid, 5, 5));
  assert (sct == eiac.getDecoration(sctid, 5, 5));

  eiac.lockDecoration (sctid);
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eiac.getDecoration(runid, 5, 5));
  assert (pix == eiac.getDecoration(pixid, 5, 5));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eiac.getDecoration(sctid, 5, 5));

  eiac.toTransient();
  assert (trt == eiac.getDecoration(trtid, 5, 5));
  assert (lar == eiac.getDecoration(larid, 5, 5));

  lar[2] = 1;
  eiac.toTransient();
  assert (trt == eiac.getDecoration(trtid, 5, 5));
  EXPECT_EXCEPTION (SG::ExcStoreLocked, eiac.getDecoration(larid, 5, 5));
}


int main()
{
  std::cout << "ut_xaodeventinfo_eventinfoauxcontainer_v1_test\n";
  test1();
  return 0;
}

