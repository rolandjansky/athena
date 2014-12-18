/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACOLLECTION_P1_H
#define MUON_CSCCRAWDATACOLLECTION_P1_H

/*

Persistent represenation of the Collection of CSC Raw Hits,
Author: Marcin Nowak
        CERN, December, 2005

*/

//#include "MuonRDO/CscRawDataCollection.h"
#include "MuonEventAthenaPool/CscRawData_p1.h"

class CscRawDataCollection_p1
   : public std::vector<CscRawData_p1>
{
public:

  // Default constructor 
  CscRawDataCollection_p1 () :
	m_id(0), m_rodId(0), m_subDetectorId(0), m_numSamples(0),
	m_latency(0), m_numDPU(0), m_rate(0)
  { }


  // get methods
  uint16_t identify() const {return m_id;} 
  uint16_t rodId()     const {return m_rodId;}
  uint16_t subDetectorId() const {return m_subDetectorId;}
  uint16_t numSamples() const {return m_numSamples;}
  uint16_t latency() const {return m_latency;}
  uint16_t numDPU()  const {return m_numDPU;}
  uint16_t samplingRate() const {return m_rate;}
  
  ////DRQ: Commented out because it's unimplemented
  ////uint16_t calculateOnlineId (uint16_t subDetectorId, uint16_t rodId);

  // set methods
  void setRodId (uint16_t rodId) {m_rodId = rodId;}
  void setSubDetectorId (uint16_t subDetectorId) {m_subDetectorId = subDetectorId;}
  void setOnlineId (uint16_t id) {m_id = id;}
  void setSamplingRate (uint16_t rate) {m_rate = rate;}
  void setNumSamples (uint16_t n) {m_numSamples = n;}
  void setLatency (uint16_t latency) {m_latency = latency;}
  void setNumDPU  (uint16_t numDPU)  {m_numDPU = numDPU;}
  void setOnlineId (uint16_t rodId, uint16_t subDetectorId) {m_id = rodId; m_subDetectorId = subDetectorId; }

/*  
  void		fillTransient(CscRawDataCollection* transObj, MsgStream &log) const  ;
  void		fillPersistent(const CscRawDataCollection* transObj, MsgStream &log) ;

  CscRawDataCollection*		createTransient(MsgStream &log) const;  
  static CscRawDataCollection_p1* createPersistent(const CscRawDataCollection* transObj, MsgStream &log);
*/

friend class  CscRawDataCollectionCnv_p1;
  
private:

  // Identifier of this collection
  uint32_t m_id;

  // online ROD Identifier
  uint16_t m_rodId;
  uint16_t m_subDetectorId;

  // number of samples
  uint16_t m_numSamples;

  // latency
  uint16_t m_latency;

  // number of DPU
  uint16_t m_numDPU;  

  // sampling rate
  uint16_t m_rate;

};


#endif




