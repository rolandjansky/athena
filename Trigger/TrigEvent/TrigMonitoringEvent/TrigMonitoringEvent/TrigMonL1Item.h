/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_L1ITEM_H
#define TRIGMON_L1ITEM_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Encoded L1 decision for one item.
*/

// Framework
#include "AthenaKernel/CLASS_DEF.h"

// C/C++
#include <stdint.h>
  
// Local 
#include "TrigMonitoringEvent/TrigConfChain.h"

class TrigMonL1Item
{
 public:
  
  enum Decision {
    kReset            = 0x0,
    kPassed           = 0x6000, // not stored
    kPrescaled        = 0x5000, // not stored
    kL1AfterVeto      = 0x4000,
    kL1BeforePrescale = 0x2000,
    kL1AfterPrescale  = 0x1000
  };
  
  TrigMonL1Item();
  explicit TrigMonL1Item(uint16_t encoded);
  ~TrigMonL1Item() = default;
  
  void setCtpId(unsigned int ctpid);
  void addDecision(Decision value);

  uint16_t getCtpId()     const;
  uint16_t getEncoded()   const { return m_encoded; }
  uint16_t getCounter()   const { return getCtpId(); }
  uint16_t getEncodedId() const;
  
  bool isPassed(Decision value) const;

  bool isPassedBeforePrescale() const { return isPassed(kL1BeforePrescale); }
  bool isPassedAfterPrescale()  const { return isPassed(kL1AfterPrescale); }
  bool isPassedAfterVeto()      const { return isPassed(kL1AfterVeto); }
  bool isPassed()               const { return isPassedAfterVeto(); } 
  bool isPrescaled()            const { return isPassedBeforePrescale() && !isPassedAfterPrescale(); }
  bool isVeto()                 const { return isPassedAfterPrescale()  && !isPassedAfterVeto(); }

  void print(std::ostream &os = std::cout) const;
  
 private:
  
  uint16_t  m_encoded;     // Encoded ctp id and decisions
};

std::string str(const TrigMonL1Item &);

//
// m_encoded stores encoded ctp id and decisions
//
// m_encoded = adddcccccccccccc
//
// ctp id     [c]  low 12 bits
// decision   [d]  mid 3 bits
// available  [a]  top 1 bit
//

//
// Inlined member and global functions
//
inline uint16_t TrigMonL1Item::getEncodedId() const
{
  return Trig::getEncodedId(1, getCounter());
}

inline bool operator==(const TrigMonL1Item &lhs, const TrigMonL1Item &rhs) { 
  return lhs.getEncodedId() == rhs.getEncodedId();
}
inline bool operator <(const TrigMonL1Item &lhs, const TrigMonL1Item &rhs) { 
  return lhs.getEncodedId() < rhs.getEncodedId();
}

inline bool operator==(const TrigMonL1Item &d, const TrigConfChain &c) { 
  return d.getEncodedId() == c.getEncodedId();
}
inline bool operator==(const TrigConfChain &c, const TrigMonL1Item &d) { 
  return d.getEncodedId() == c.getEncodedId();
}

CLASS_DEF( TrigMonL1Item , 98029077 , 1 )

#endif
