/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/* **********************************************************
*
* CTP RIO - class to hold ROD Header data and status words
*   author: David Berge
*   author: Ellie Dobson
********************************************************** */

#ifndef TRIGT1RESULT_CTP_RIO_H
#define TRIGT1RESULT_CTP_RIO_H

#include <vector>
#include <cstdint>

#include "AthenaKernel/CLASS_DEF.h"

class MsgStream;

class CTP_RIO {

public:
  CTP_RIO();
  ~CTP_RIO();

  uint32_t getSourceId() const{return m_sourceId;}
  uint32_t getRunNumber() const{return m_runNumber;}
  uint32_t getLvl1Id() const{return m_lvl1Id;}
  uint32_t getBCID() const{return m_bcid;}
  uint32_t getLvl1TriggerType() const{return m_lvl1TriggerType;}
  uint32_t getDetectorEventType() const{return m_detEventType;}
  uint32_t getNumberDataWords() const{return m_nDataWords;}
  uint32_t getNumberStatusWords() const{return m_nStatusWords;}
  const std::vector<uint32_t>& getStatusWords() const{return m_statusWords;}

  void sourceId(uint32_t val) {m_sourceId = val;}
  void runNumber(uint32_t val) {m_runNumber = val;}
  void lvl1Id(uint32_t val) {m_lvl1Id = val;}
  void bcid(uint32_t val) {m_bcid = val;}
  void lvl1TriggerType(uint32_t val) {m_lvl1TriggerType = val;}
  void detectorEventType(uint32_t val) {m_detEventType = val;}
  void numberDataWords(uint32_t val) {m_nDataWords = val;}
  void numberStatusWords(uint32_t val) {m_nStatusWords = val;}
  std::vector<uint32_t>& statusWords() {return m_statusWords;}

  void dumpData() const;
  void dumpData(MsgStream&) const;

private:
  uint32_t m_sourceId;
  uint32_t m_runNumber;
  uint32_t m_lvl1Id;
  uint32_t m_bcid;
  uint32_t m_lvl1TriggerType;
  uint32_t m_detEventType;
  uint32_t m_nDataWords;
  uint32_t m_nStatusWords;
  std::vector<uint32_t> m_statusWords;  

}; // class CTP_RIO

CLASS_DEF( CTP_RIO , 6015 , 0 )

#endif // TRIGT1RESULT_CTP_RIO_H
