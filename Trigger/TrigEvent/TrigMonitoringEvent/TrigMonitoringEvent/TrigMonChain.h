/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CHAIN_H
#define TRIGMON_CHAIN_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Summary of chain decisions.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <stdint.h>

// Local
#include "TrigMonitoringEvent/TrigConfChain.h"
#include "TrigMonitoringEvent/TrigMonVar.h"
  
class TrigMonChain
{
 public:
  
  enum Decision {
    kReset            = 0x0,
    kPassed           = 0x010000,
    kPassedRaw        = 0x020000,
    kPassedThrough    = 0x040000, 
    kResurrected      = 0x080000,
    kPrescaled        = 0x100000,
    kL1AfterVeto      = 0x200000,
    kL1BeforePrescale = 0x400000,
    kL1AfterPrescale  = 0x800000,
    kExpressStream    = 0x1000000
  };
  
  TrigMonChain(unsigned int encoded = 0);
  TrigMonChain(unsigned int level, unsigned int counter);
  TrigMonChain(const std::string &level, unsigned int counter);
  ~TrigMonChain() {}
  
  void addDecision(Decision value);
  void addTimer(float timer);
  void addVar(const TrigMonVar &var);

  uint16_t getCounter()   const;
  uint16_t getLevel()     const;
  uint16_t getEncodedId() const;
  uint32_t getEncoded()   const { return m_encoded; }

  float getTimer() const;

  bool isPassed(Decision value = kPassed) const;

  const std::vector<TrigMonVar> getVar()    const;
  const std::vector<uint16_t>&  getVarKey() const { return m_var_key; }
  const std::vector<float>&     getVarVal() const { return m_var_val; }

  void print(std::ostream &os = std::cout) const;
  
 private:
  
  uint32_t                m_encoded;     // Encoded chain word (see below)
  std::vector<uint16_t>   m_var_key;     // Variable key
  std::vector<float>      m_var_val;     // Variable value
};

std::string str(const TrigMonChain &);

//
// m_encoded stores encoded trigger id and decisions
//
// m_encoded 16 low bits:
//    chain level   [l]  2 bits (0:3)      = 1 (L1), 2 (L2), 3 (EF)
//    chain counter [c] 14 bits (0:16384) 
//
// m_encoded 16 top bits:
//    chain decision   [d] 9 bits packed with Decision enums
//                     [f] 7 bits are free
//

//
// Inlined member and global functions
//
inline bool operator==(const TrigMonChain &lhs, const TrigMonChain &rhs) { 
  return lhs.getEncodedId() == rhs.getEncodedId();
}
inline bool operator <(const TrigMonChain &lhs, const TrigMonChain &rhs) { 
  return lhs.getEncodedId() < rhs.getEncodedId();
}

inline bool operator==(const TrigMonChain &d, const TrigConfChain &c) { 
  return d.getEncodedId() == c.getEncodedId();
}
inline bool operator==(const TrigConfChain &c, const TrigMonChain &d) { 
  return d.getEncodedId() == c.getEncodedId();
}

inline bool operator==(const TrigMonChain &d, unsigned int i) { 
  return d.getEncodedId() == i;
}
inline bool operator==(unsigned int i, const TrigMonChain &d) { 
  return i == d.getEncodedId();
}

CLASS_DEF( TrigMonChain , 261329711 , 1 )

#endif
