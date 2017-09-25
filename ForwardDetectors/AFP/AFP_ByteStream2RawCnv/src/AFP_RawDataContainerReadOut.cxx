/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_ByteStream2RawCnv/AFP_RawDataContainerReadOut.h"

AFP_RawDataContainerReadOut::AFP_RawDataContainerReadOut()
    :

      m_wordMarker(0),
      m_dataWord(0), m_subdetId(0), m_mrodId(0), m_lvl1Id(0), m_ecrId(0),
      m_bcId(0), m_runNum(0), m_runType(0), m_triggerTypeId(0),
      m_DetEventType(0), m_timestamp(0), m_BCId(0) {}

AFP_RawDataContainerReadOut::~AFP_RawDataContainerReadOut() {}

void AFP_RawDataContainerReadOut::setZero() {
  m_subdetId = 0;
  m_mrodId = 0;
  m_runNum = 0;
  m_runType = 0;
  m_lvl1Id = 0;
  m_ecrId = 0;
  m_bcId = 0;
  m_triggerTypeId = 0;
  m_DetEventType = 0;
}

void AFP_RawDataContainerReadOut::setZeroFull() {
  m_timestamp = 0;
  m_BCId = 0;
}
