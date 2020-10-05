/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/CTP_RDOCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CTP_RDOCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CTP_RDOCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CTPdataformatVersion& p1,
              const CTPdataformatVersion& p2)
{
  assert (p1.getVersionNumber() == p2.getVersionNumber());
  assert (p1.getNumTrailerWords() == p2.getNumTrailerWords());
  assert (p1.getNumBunchgroups() == p2.getNumBunchgroups());
  assert (p1.getMaxTrigItems() == p2.getMaxTrigItems());
  assert (p1.getNumRndmTriggers() == p2.getNumRndmTriggers());
  assert (p1.getNumPrescaledClocks() == p2.getNumPrescaledClocks());
  assert (p1.getNumProgrammableExtraWords() == p2.getNumProgrammableExtraWords());
  assert (p1.getMaxTIPback() == p2.getMaxTIPback());
  assert (p1.getOverclockingFactorBack() == p2.getOverclockingFactorBack());
  assert (p1.getMaxTIPfront() == p2.getMaxTIPfront());
  assert (p1.getOverclockingFactorFront() == p2.getOverclockingFactorFront());
  assert (p1.getMaxTIPtotal() == p2.getMaxTIPtotal());
  assert (p1.getNumberTimeWords() == p2.getNumberTimeWords());
  assert (p1.getAuxTIPwordPos() == p2.getAuxTIPwordPos());
  assert (p1.getTIPwords() == p2.getTIPwords());
  assert (p1.getRoIBTIPwords() == p2.getRoIBTIPwords());
  assert (p1.getTBPwords() == p2.getTBPwords());
  assert (p1.getTAPwords() == p2.getTAPwords());
  assert (p1.getRoIBTAPwords() == p2.getRoIBTAPwords());
  assert (p1.getTAVwords() == p2.getTAVwords());
  assert (p1.getDAQwordsPerBunch() == p2.getDAQwordsPerBunch());
  assert (p1.getRoIBwordsPerBunch() == p2.getRoIBwordsPerBunch());
  assert (p1.getTimeNanosecondsOffset() == p2.getTimeNanosecondsOffset());
  assert (p1.getTimeNanosecondsTicks() == p2.getTimeNanosecondsTicks());
  assert (p1.getBcidMask() == p2.getBcidMask());
  assert (p1.getBcidShift() == p2.getBcidShift());
  assert (p1.getBunchGroupMask() == p2.getBunchGroupMask());
  assert (p1.getBunchGroupShift() == p2.getBunchGroupShift());
  assert (p1.getRandomTrigMask() == p2.getRandomTrigMask());
  assert (p1.getRandomTrigShift() == p2.getRandomTrigShift());
  assert (p1.getPrescaledClockMask() == p2.getPrescaledClockMask());
  assert (p1.getPrescaledClockShift() == p2.getPrescaledClockShift());
  assert (p1.getLumiBlockMask() == p2.getLumiBlockMask());
  assert (p1.getLumiBlockShift() == p2.getLumiBlockShift());
  assert (p1.getL1APositionMask() == p2.getL1APositionMask());
  assert (p1.getL1APositionShift() == p2.getL1APositionShift());
  assert (p1.getDetEvtTypeExtraBitsMask() == p2.getDetEvtTypeExtraBitsMask());
  assert (p1.getDetEvtTypeExtraBitsShift() == p2.getDetEvtTypeExtraBitsShift());
  assert (p1.getHltCounterMask() == p2.getHltCounterMask());
  assert (p1.getHltCounterShift() == p2.getHltCounterShift());
  assert (p1.getProgrammableExtraWordsShift() == p2.getProgrammableExtraWordsShift());
  assert (p1.getProgrammableExtraWordsMask() == p2.getProgrammableExtraWordsMask());
  assert (p1.getOffsetExtraPayload() == p2.getOffsetExtraPayload());
  assert (p1.getCTPFormatVersionMask() == p2.getCTPFormatVersionMask());
  assert (p1.getCTPFormatVersionShift() == p2.getCTPFormatVersionShift());
  assert (p1.getTimeNanosecondsPos() == p2.getTimeNanosecondsPos());
  assert (p1.getTimeSecondsPos() == p2.getTimeSecondsPos());
  assert (p1.getPITpos() == p2.getPITpos());
  assert (p1.getFPIpos() == p2.getFPIpos());
  assert (p1.getTIPpos() == p2.getTIPpos());
  assert (p1.getTBPpos() == p2.getTBPpos());
  assert (p1.getRoIBTBPpos() == p2.getRoIBTBPpos());
  assert (p1.getTAPpos() == p2.getTAPpos());
  assert (p1.getTAVpos() == p2.getTAVpos());
  assert (p1.getRoIBTAVpos() == p2.getRoIBTAVpos());
  assert (p1.getHeaderMarkerPos() == p2.getHeaderMarkerPos());
  assert (p1.getHeaderSizePos() == p2.getHeaderSizePos());
  assert (p1.getFormatVersionPos() == p2.getFormatVersionPos());
  assert (p1.getSourceIdPos() == p2.getSourceIdPos());
  assert (p1.getDetectorTypePos() == p2.getDetectorTypePos());
  assert (p1.getTTPhysicsCalibrationBit() == p2.getTTPhysicsCalibrationBit());
  assert (p1.getTTCalibrationDetectorCodeShift() == p2.getTTCalibrationDetectorCodeShift());
  assert (p1.getTTCalibrationDetectorCodeMask() == p2.getTTCalibrationDetectorCodeMask());
}


void compare (const CTP_RDO& p1,
              const CTP_RDO& p2)
{
  assert (p1.getCTPVersionNumber() == p2.getCTPVersionNumber());
  //compare (p1.getCTPVersion(), p2.getCTPVersion());
  assert (p1.getDataWords() == p2.getDataWords());
  assert (p1.getL1AcceptBunchPosition() == p2.getL1AcceptBunchPosition());
  assert (p1.getTurnCounter() == p2.getTurnCounter());
  assert (p1.getNumberOfBunches() == p2.getNumberOfBunches());
  assert (p1.getNumberOfAdditionalWords() == p2.getNumberOfAdditionalWords());
}


void testit (const CTP_RDO& trans1)
{
  MsgStream log (0, "test");
  CTP_RDOCnv_p1 cnv;
  CTP_RDO_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  CTP_RDO trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  std::vector<unsigned int> data (2 + 3*(6+8+8+8));
  for (size_t i=0; i < data.size(); i++)
    data[i] = i;
  CTP_RDO trans1 (0, std::move(data));
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
