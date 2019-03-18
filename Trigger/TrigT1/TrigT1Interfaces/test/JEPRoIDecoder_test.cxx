/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrigT1Interfaces/test/JEPRoIDecoder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2019
 * @brief Unit tests for JEPRoIDecoder.
 */


#undef NDEBUG
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TestTools/FLOATassert.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  const double jemPhiSize = M_PI / 2;
  const double jePhiSize = M_PI / 16;
  const double JEMEtaSize = 0.8;
  const double jeEtaSize = 0.2;

    LVL1::JEPRoIDecoder d;
  assert (d.roiType(0x82345678) == LVL1::TrigT1CaloDefs::JetRoIWordType);
  assert (d.roiType(0x02345678) == LVL1::TrigT1CaloDefs::JetRoIWordType);
  assert (d.roiType(0xa2345678) == LVL1::TrigT1CaloDefs::JetEtRoIWordType);
  assert (d.roiType(0x42345678) == LVL1::TrigT1CaloDefs::EnergyRoIWordType0);
  assert (d.roiType(0x52345678) == LVL1::TrigT1CaloDefs::EnergyRoIWordType2);
  assert (d.roiType(0x62345678) == LVL1::TrigT1CaloDefs::EnergyRoIWordType1);

  assert (d.jetRoIVersion(0x82345678) == 1);
  assert (d.jetRoIVersion(0x02345678) == 2);
  assert (d.jetRoIVersion(0x32345678) == 999);

  // run1; crate=0; jem=13; col=2, row=5 [frame=6 rl=2]; th=4,7; fth=2
  unsigned int word1 = 0x8dd01248;
  assert (d.crate(word1) == 0);
  assert (d.module(word1) == 13);
  assert (d.column(word1) == 2);
  assert (d.row(word1) == 5);
  assert (d.thresholdsPassed(word1)    == (std::vector<unsigned>{4,7}));
  assert (d.fwdThresholdsPassed(word1) == (std::vector<unsigned>{2}));
  assert (d.etLarge(word1) == 0);
  assert (d.etSmall(word1) == 0);
  const double phi1 = 2*jemPhiSize + 6*jePhiSize;
  const double eta1 = JEMEtaSize + 3*jeEtaSize;
  LVL1::CoordinateRange cr1 = d.coordinate(word1);
  FLOAT_EQassert (cr1.phi(), phi1);
  FLOAT_EQassert (cr1.phiRange().min(), phi1-jePhiSize);
  FLOAT_EQassert (cr1.phiRange().max(), phi1+jePhiSize);
  FLOAT_EQassert (cr1.eta(), eta1);
  FLOAT_EQassert (cr1.etaRange().min(), eta1-jeEtaSize);
  FLOAT_EQassert (cr1.etaRange().max(), eta1+jeEtaSize);

  // run2; crate=1; jem=7; col=1, row=6 [frame=3 rl=1]; etLarge=0x0x248; etSmall=0x40
  unsigned int word2 = 0x17681248;
  assert (d.crate(word2) == 1);
  assert (d.module(word2) == 7);
  assert (d.column(word2) == 1);
  assert (d.row(word2) == 6);
  assert (d.thresholdsPassed(word2)    == (std::vector<unsigned>{}));
  assert (d.fwdThresholdsPassed(word2) == (std::vector<unsigned>{}));
  assert (d.etLarge(word2) == 0x248);
  assert (d.etSmall(word2) == 0x04);
  LVL1::CoordinateRange cr2 = d.coordinate(word2);
  const double phi2 = jemPhiSize + 7*jePhiSize;
  FLOAT_EQassert (cr2.phi(), phi2);
  FLOAT_EQassert (cr2.phiRange().min(), phi2-jePhiSize);
  FLOAT_EQassert (cr2.phiRange().max(), phi2+jePhiSize);
  FLOAT_EQassert (cr2.etaRange().min(), 2.7);
  FLOAT_EQassert (cr2.etaRange().max(), 3.2);
  FLOAT_EQassert (cr2.eta(), 2.95);

  // run2; crate=1; jem=8; col=0, row=3 [frame=1 rl=2]; etLarge=0x0x321; etSmall=0x10
  unsigned int word3 = 0x18304321;
  assert (d.crate(word3) == 1);
  assert (d.module(word3) == 8);
  assert (d.column(word3) == 0);
  assert (d.row(word3) == 3);
  assert (d.thresholdsPassed(word3)    == (std::vector<unsigned>{}));
  assert (d.fwdThresholdsPassed(word3) == (std::vector<unsigned>{}));
  assert (d.etLarge(word3) == 0x321);
  assert (d.etSmall(word3) == 0x10);
  LVL1::CoordinateRange cr3 = d.coordinate(word3);
  const double phi3 = 3*jemPhiSize + 4*jePhiSize;
  FLOAT_EQassert (cr3.phi(), phi3);
  FLOAT_EQassert (cr3.phiRange().min(), phi3-jePhiSize);
  FLOAT_EQassert (cr3.phiRange().max(), phi3+jePhiSize);
  FLOAT_EQassert (cr3.etaRange().min(), -4.9);
  FLOAT_EQassert (cr3.etaRange().max(), -2.9);
  FLOAT_EQassert (cr3.eta(), -3.9);

  unsigned int word10 = 0x40241234;
  assert (d.energyX(word10) == 0x1234);
  assert (d.energyY(word10) == 0);
  assert (d.energyT(word10) == 0);
  assert (d.energyOverflow(word10) == false);
  assert (d.mEtSigThresholdsPassed(word10) == (std::vector<unsigned>{3,6}));
  assert (d.mEtSigThresholdPassed(word10, 2) == true);
  assert (d.mEtSigThresholdPassed(word10, 5) == true);
  assert (d.mEtSigThresholdPassed(word10, 4) == false);

  unsigned int word11 = 0x6013d234;
  assert (d.energyX(word11) == 0);
  assert (d.energyY(word11) == -11724);
  assert (d.energyT(word11) == 0);
  assert (d.energyOverflow(word11) == true);
  assert (d.etSumThresholdsPassed(word11) == (std::vector<unsigned>{1,2,5}));
  assert (d.sumEtThresholdPassed(word11, 1) == true);
  assert (d.sumEtThresholdPassed(word11, 2) == false);

  unsigned int word12 = 0x50c2a856;
  assert (d.energyX(word12) == 0);
  assert (d.energyY(word12) == 0);
  assert (d.energyT(word12) == 0x2856);
  assert (d.energyOverflow(word12) == true);
  assert (d.etMissThresholdsPassed(word12) == (std::vector<unsigned>{2,7,8}));
  assert (d.etMissThresholdPassed(word12, 5) == false);
  assert (d.etMissThresholdPassed(word12, 6) == true);
}


int main()
{
  std::cout << "JEPRoIDecoder_test\n";
  test1();
  return 0;
}
