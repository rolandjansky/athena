/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_VAR_H
#define TRIGMON_VAR_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Variable pair: 16 bits int id and float value.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>
#include <iostream>

namespace Trig
{
  // Dictionary of keys
  enum TrigCostMagicNumber_t {
    kRoIEta = 0,
    kRoIPhi = 1,
    kEventBufferSize = 9,
    kEventLumiBlockLength = 43,
    kEBWeight = 45,
    kEBBunchGroup = 46,
    kIsCostEvent = 47,
    kEBIsUnbiasedFlag = 48,
    kSMK = 66,
    kL1PSK = 67,
    kHLTPSK = 68,
    kTimeCostMonitoring = 100,
    kTimeExec = 101,
    kTimeProc = 102,
    kTimeRes = 103,
    kTimeMon = 104,
    // 200 - 299 reserved but not used in TrigNtVarsTool
    kRoIET = 300, // Main transverse energy/pt var
    kRoIIsTau = 301,
    kRoIIsolationBits = 302,
    kRoIETLarge = 304,
    kRoIMuonCharge = 306,
    kRoIEnergyVectorX = 307,
    kRoIEnergyVectorY = 308,
    kRoIEnergyOverflowX = 310,
    kRoIEnergyOverflowY = 311,
    kRoIEnergyOverflowT = 312,
    kEventNumber = 9999
    // 10,000 - 11,000 reserved for LB
  };
}  

class TrigMonVar
{
 public:
  
  TrigMonVar();
  TrigMonVar(unsigned int key, float value);
  ~TrigMonVar() {}
  
  uint16_t getId()  const { return m_key; }
  uint16_t getKey() const { return m_key; }
  uint16_t    key() const { return m_key; }

  float    data() const { return m_data; }
  float getData() const { return m_data; }
  
  void print(std::ostream &os) const;
  
 private:
  
  uint16_t m_key;     // variable key
  float    m_data;    // variable value
};

//
// Inlined comparison operators
//

inline bool operator==(const TrigMonVar &lhs, const TrigMonVar &rhs) { 
  return lhs.getKey() == rhs.getKey();
}
inline bool operator <(const TrigMonVar &lhs, const TrigMonVar &rhs) { 
  return lhs.getKey() < rhs.getKey();
}

inline bool operator==(const TrigMonVar &var, unsigned int key) { 
  return var.getKey() == key;
}
inline bool operator==(unsigned int key, const TrigMonVar &var) { 
  return var.getKey() == key; 
}

inline bool operator<(const TrigMonVar &var, unsigned int key) { 
  return var.getKey() < key; 
}
inline bool operator<(unsigned int key, const TrigMonVar &var) { 
  return key < var.getKey(); 
}

CLASS_DEF( TrigMonVar , 126820305 , 1 )

#endif
