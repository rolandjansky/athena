/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_SEQ_H
#define TRIGMON_SEQ_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Summary of sequence execution.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigMonAlg.h"
#include "TrigMonitoringEvent/TrigMonVar.h"
#include "TrigMonitoringEvent/TrigConfChain.h"
#include "TrigMonitoringEvent/TrigConfSeq.h"

class TrigMonSeq
{
 public:
  
  enum State {
    kUnknown         = 0x0,       // Default state
    kInitial         = 0x010000,  // Initialization of sequence execution
    kStart           = 0x020000,  // Start running algorithms
    kAlreadyExecuted = 0x040000,  // Sequence (TE) was cached
    kPrevious        = 0x080000   // Sequence was executed by other sequence
  };
  
  TrigMonSeq();
  explicit TrigMonSeq(uint32_t encoded);
  TrigMonSeq(const TrigConfChain &chn, const TrigConfSeq &seq);
  ~TrigMonSeq() {}
  
  void addAlg  (const TrigMonAlg &alg) { m_alg.push_back(alg); }
  void addVar  (const TrigMonVar &var);
  void addTimer(float timer);
  void addState(State value);
  
  bool isInitial()         const;
  bool isExecuted()        const;
  bool isAlreadyExecuted() const;
  bool isPrevious()        const;

  uint16_t getLevel()        const;
  uint16_t getChnCounter()   const;
  uint16_t getChnEncodedId() const;
  uint16_t getSeqIndex()     const;
  uint32_t getEncoded()      const { return m_encoded; }
  
  float  getSeqTimer() const;
  double getAlgTimer() const;

  std::vector<TrigMonAlg>& getAlg() { return m_alg; }
  const std::vector<TrigMonAlg>& getAlg() const { return m_alg; }
  const std::vector<TrigMonVar>  getVar() const;

  const std::vector<uint16_t>&   getVarKey() const { return m_var_key; }
  const std::vector<float>&      getVarVal() const { return m_var_val; }
  
  void print(const TrigConfSeq &confg, std::ostream &os = std::cout) const;
  
 private:
  
  uint32_t                m_encoded;     // Encoded data (see below)
  std::vector<TrigMonAlg> m_alg;         // Algorithm execution and/or caching records
  std::vector<uint16_t>   m_var_key;     // Variable key
  std::vector<float>      m_var_val;     // Variable value
};

//
// m_encoded stores chain encoded id, seq index and seq state
// 
// m_encoded low 16 bits:
//   16 bits chain encoded id (see TrigConfChain.h)

// m_encoded top 16 bits:
//
//   seq state      [s]  4 bits (0:4)      State num bits
//   seq index      [i] 12 bits (0:4,096)  Number of configured sequences
//

inline bool operator==(const TrigMonSeq &lhs, const TrigMonSeq &rhs) {
  return (lhs.getSeqIndex() == rhs.getSeqIndex());
}
inline bool operator!=(const TrigMonSeq &lhs, const TrigMonSeq &rhs) {
  return !(lhs == rhs);
}
inline bool operator<(const TrigMonSeq &lhs, const TrigMonSeq &rhs) {
  return lhs.getSeqIndex() < rhs.getSeqIndex();
}

inline bool operator==(const TrigMonSeq &entry, const TrigConfSeq &confg) {
  return entry.getSeqIndex() == confg.getIndex();
}
inline bool operator==(const TrigConfSeq &confg, const TrigMonSeq &entry) {
  return entry.getSeqIndex() == confg.getIndex();
}
inline bool operator==(const TrigMonSeq &entry, const TrigConfChain &confg) {
  return entry.getChnEncodedId() == confg.getEncodedId();
}
inline bool operator==(const TrigConfChain &confg, const TrigMonSeq &entry) {
  return entry.getChnEncodedId() == confg.getEncodedId();
}

CLASS_DEF( TrigMonSeq , 81476271 , 1 )

#endif
