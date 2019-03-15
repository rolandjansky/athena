/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrigT1Interfaces/test/CPRoIDecoder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2019
 * @brief Unit tests for CPRoIDecoder.
 */


#undef NDEBUG
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TestTools/FLOATassert.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  const double cratePhiSize = M_PI / 2.0;
  const double fpgaPhiSize = M_PI / 16.0;
  const double localCoordPhiSize = M_PI / 32.0;
  const double cpmEtaSize = 0.4;
  const double localCoordEtaSize = 0.1;

  LVL1::CPRoIDecoder d;
  assert (d.roiType(0x22345678) == LVL1::TrigT1CaloDefs::CpRoIWordType);
  assert (d.roiType(0xa2345678) == LVL1::TrigT1CaloDefs::EMRoIWordType);
  assert (d.roiType(0xb2345678) == LVL1::TrigT1CaloDefs::TauRoIWordType);
  assert (d.roiType(0xc2345678) == LVL1::TrigT1CaloDefs::RoIWordTypeError);

  // EMRoI, crate=2, module=11, chip=5, localcoord=3, et=0x48, isol=0x12
  unsigned int word1 = 0xaaeb1248;
  assert (d.roiType(word1) == LVL1::TrigT1CaloDefs::EMRoIWordType);
  assert (d.crate(word1) == 2);
  assert (d.module(word1) == 11);
  assert (d.chip(word1) == 5);
  assert (d.localcoord(word1) == 3);
  assert (d.thresholdsPassed(word1) == (std::vector<unsigned>{}));
  assert (d.et(word1) == 0x48);
  assert (d.isolationWord(word1) == 0x12);
  LVL1::CoordinateRange cr1 = d.coordinate (word1);
  double phi1 = 2*cratePhiSize + 5*fpgaPhiSize + 2*localCoordPhiSize;
  double eta1 = 3*cpmEtaSize + 2*localCoordEtaSize;
  FLOAT_EQassert (cr1.phi(), phi1);
  FLOAT_EQassert (cr1.phiRange().min(), phi1-localCoordPhiSize);
  FLOAT_EQassert (cr1.phiRange().max(), phi1+localCoordPhiSize);
  FLOAT_EQassert (cr1.eta(), eta1);
  FLOAT_EQassert (cr1.etaRange().min(), eta1-localCoordEtaSize);
  FLOAT_EQassert (cr1.etaRange().max(), eta1+localCoordEtaSize);

  // CpRoI, crate=1, module=7, chip=2, localcoord=5, tp=4,7,10,13
  unsigned int word2 = 0x17541248;
  assert (d.roiType(word2) == LVL1::TrigT1CaloDefs::CpRoIWordType);
  assert (d.crate(word2) == 1);
  assert (d.module(word2) == 7);
  assert (d.chip(word2) == 2);
  assert (d.localcoord(word2) == 5);
  assert (d.thresholdsPassed(word2) == (std::vector<unsigned>{4,7,10,13}));
  assert (d.et(word2) == 0);
  assert (d.isolationWord(word2) == 0);
  LVL1::CoordinateRange cr2 = d.coordinate (word2);
  double phi2 = 1*cratePhiSize + 2*fpgaPhiSize + localCoordPhiSize;
  double eta2 = (-1)*cpmEtaSize + 4*localCoordEtaSize;
  FLOAT_EQassert (cr2.phi(), phi2);
  FLOAT_EQassert (cr2.phiRange().min(), phi2-localCoordPhiSize);
  FLOAT_EQassert (cr2.phiRange().max(), phi2+localCoordPhiSize);
  FLOAT_EQassert (cr2.eta(), eta2);
  FLOAT_EQassert (cr2.etaRange().min(), eta2-localCoordEtaSize);
  FLOAT_EQassert (cr2.etaRange().max(), eta2+localCoordEtaSize);
}


int main()
{
  std::cout << "CPRoIDecoder_test\n";
  test1();
  return 0;
}
