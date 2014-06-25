/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_SIG_H
#define TRIGCONF_SIG_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief This is a helper class used by TrigConfChain class.
   It stores signature configuration and helps to match 
   TrigConfSeq and TrigMonSeq to chains.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

class TrigConfSig
{
 public:
  
  TrigConfSig();
  TrigConfSig(uint32_t counter, int logic, const std::string &label);
  ~TrigConfSig() {}
  
  void clearStrings();

  void addOutputTE(uint32_t te)  { m_output_te.push_back(te); }
  
  uint32_t           getCounter() const { return m_counter; }
  int                getLogic()   const { return m_logic; }
  const std::string& getLabel()   const { return m_label; }
  
  const std::vector<uint32_t>& getOutputTEs() const { return m_output_te; }
  
  bool matchOutputTE(uint32_t te) const;
  
  void print(std::ostream &os = std::cout) const;
  
 private:
  
  uint32_t                 m_counter;     // Signature counter in chain
  int                      m_logic;       // Signature logic
  std::string              m_label;       // Signature label
  std::vector<uint32_t>    m_output_te;   // Id list of output TEs
};

std::string str(const TrigConfSig &);

//
// Inline global operators
//  
inline bool operator==(const TrigConfSig &lhs, const TrigConfSig &rhs) { 
  return lhs.getLabel() == rhs.getLabel();
}
inline bool operator!=(const TrigConfSig &lhs, const TrigConfSig &rhs) {
  return !(lhs == rhs);
}
inline bool operator <(const TrigConfSig &lhs, const TrigConfSig &rhs) { 
  return lhs.getLabel() < rhs.getLabel();
}

//
// Compare TrigConfSig with other types
//
inline bool operator==(const TrigConfSig &seq, const std::string &name) {
  return seq.getLabel() == name;
}
inline bool operator==(const std::string &name, const TrigConfSig &seq) {
  return name == seq.getLabel();
}

inline bool operator <(const TrigConfSig &seq, const std::string &name) {
  return seq.getLabel() < name;
}
inline bool operator <(const std::string &name, const TrigConfSig &seq) {
  return name < seq.getLabel();
}

CLASS_DEF( TrigConfSig , 34893644 , 1 )

#endif
