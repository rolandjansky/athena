/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_SEQ_H
#define TRIGCONF_SEQ_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief This class provides map between monitoring
   object TrigMonSeq and sequence configuration.
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigConfAlg.h"

class TrigConfSeq
{
 public:
  
  TrigConfSeq();
  TrigConfSeq(unsigned int id, unsigned int index, const std::string &name);
  ~TrigConfSeq() {}

  void clearStrings();

  void addAlg    (const TrigConfAlg &alg) { m_alg.push_back(alg); }
  void addInputTE(uint32_t te_id)        { m_input_te.push_back(te_id); }
  void setTopoTE (uint32_t te_id)        { m_topo_te = te_id; }
  
  uint16_t           getIndex()  const { return m_output_te_index; }
  uint32_t           getId()     const { return m_output_te_id; }
  const std::string& getName()   const { return m_output_te_name; }
  uint32_t           getTopoTE() const { return m_topo_te; }

  const std::vector<uint32_t>& getInputTEs() const { return m_input_te; }
  
  const             TrigConfAlg & getAlg(unsigned int pos) const;
  const std::vector<TrigConfAlg>& getAlg()                 const { return m_alg; }
  
  unsigned int getNAlg() const { return m_alg.size(); }

  std::vector<TrigConfAlg>::const_iterator algBeg() const { return m_alg.begin(); }
  std::vector<TrigConfAlg>::const_iterator algEnd() const { return m_alg.end(); }
  
  std::vector<TrigConfAlg>::const_iterator findName(const std::string &aname) const;
  std::vector<TrigConfAlg>::const_iterator findType(const std::string &atype) const;
  
  bool matchAlgName(const std::string &aname) const;
  bool matchAlgType(const std::string &atype) const;
  
  void print(std::ostream &os = std::cout) const;
  
 private:
  
  std::string               m_output_te_name;   // Name  of output TE
  uint16_t                  m_output_te_index;  // Index of output TE (in current configuration)
  uint32_t                  m_output_te_id;     // Id    of output TE
  uint32_t                  m_topo_te;          // Id of topo start TE
  std::vector<TrigConfAlg>  m_alg;              // List of children algorithms
  std::vector<uint32_t>     m_input_te;         // Id list of input TEs
};

std::string str(const TrigConfSeq &);

//
// Inline global operators
//
inline bool operator==(const TrigConfSeq &lhs, const TrigConfSeq &rhs) { 
  return lhs.getName() == rhs.getName();
}
inline bool operator!=(const TrigConfSeq &lhs, const TrigConfSeq &rhs) { 
  return !(lhs == rhs);
}
inline bool operator <(const TrigConfSeq &lhs, const TrigConfSeq &rhs) { 
  return lhs.getName() < rhs.getName();
}

//
// Compare TrigConfSeq to other types
//
inline bool operator==(const TrigConfSeq &s, const std::string &n) { return s.getName() == n; }
inline bool operator==(const std::string &n, const TrigConfSeq &s) { return n == s.getName(); }

inline bool operator <(const TrigConfSeq &s, const std::string &n) { return s.getName() < n; }
inline bool operator <(const std::string &n, const TrigConfSeq &s) { return n < s.getName(); }

inline bool operator==(const TrigConfSeq &s, unsigned int i) { return s.getId() == i; }
inline bool operator==(unsigned int i, const TrigConfSeq &s) { return i == s.getId(); }

inline bool operator <(const TrigConfSeq &s, unsigned int i) { return s.getId() < i; }
inline bool operator <(unsigned int i, const TrigConfSeq &s) { return i < s.getId(); }

CLASS_DEF( TrigConfSeq , 46893630 , 1 )

#endif
