/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_RPCSECTORLOGIC_H
#define MUONRDO_RPCSECTORLOGIC_H

#include<stdint.h>
#include<vector>

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"

#include "MuonRDO/RpcSLTriggerHit.h"

//
// S. Rosati Jun 2006
//

class RpcSectorLogic : public DataVector<RpcSLTriggerHit> {

 public:

  /** Default constructor  */
  RpcSectorLogic() :
    m_sectorId(0), 
    m_fel1Id(0),
    m_bcid(0),
    m_errorCode(0), m_crc(0), m_hasMoreThan2TriggerCand(false) { };

  /** Constructor with arguments */
  RpcSectorLogic(uint16_t sectorId, uint16_t fel1Id, uint16_t bcid, uint16_t errorCode) :
    m_sectorId(sectorId),
    m_fel1Id(fel1Id),
    m_bcid(bcid),
    m_errorCode(errorCode), m_crc(0), m_hasMoreThan2TriggerCand(false) { }

  RpcSectorLogic(uint16_t sectorId, uint16_t fel1Id, uint16_t bcid, uint16_t errorCode, uint16_t crc) :
    m_sectorId(sectorId),
    m_fel1Id(fel1Id),
    m_bcid(bcid),
    m_errorCode(errorCode), m_crc(crc), m_hasMoreThan2TriggerCand(false) { }

  /** Destructor */
  ~RpcSectorLogic() { };
  
  /** Set methods */
  void addCounter(const uint16_t counter);
  void addTriggerRate(const double trig) {m_triggerRates.push_back(trig);}
  void setHasMoreThan2TriggerCand(const bool a) {m_hasMoreThan2TriggerCand=a;}

  /** Access methods */
  uint16_t sectorId() const {return m_sectorId;}
  uint16_t fel1Id() const {return m_fel1Id;}
  uint16_t bcid() const {return m_bcid;}
  uint16_t errorCode() const {return m_errorCode;}
  uint16_t crc() const {return m_crc;}
  bool    hasMoreThan2TriggerCand() const {return m_hasMoreThan2TriggerCand;}

  const std::vector<uint16_t>& counters() const {return m_counters;}

  const std::vector<double>& triggerRates() const {return m_triggerRates;}

 private:

  uint16_t m_sectorId;
  uint16_t m_fel1Id;
  uint16_t m_bcid;
  uint16_t m_errorCode;
  uint16_t m_crc;
  bool m_hasMoreThan2TriggerCand;

  std::vector<double> m_triggerRates;
  std::vector<uint16_t>    m_counters;

};

CLASS_DEF(RpcSectorLogic,125408109,1)

#endif




