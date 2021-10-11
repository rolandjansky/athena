/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_VAR_H
#define TRIGCONF_VAR_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief This class provides map between id of TrigMonVar 
  and string. It stores list of id, string pairs which 
  can be saved in TrigMonConfig at end of a job.
*/

// Framework
#include "AthenaKernel/CLASS_DEF.h"
#include "CxxUtils/checker_macros.h"

// C/C++
#include <iostream>
#include <string>
#include <vector>

// Local
#include "TrigMonitoringEvent/TrigMonVar.h"

class TrigConfVar {
 public:
  
  TrigConfVar();
  TrigConfVar(const std::string &name, uint32_t id);
  ~TrigConfVar() {}
  
  const std::string&    name() const { return m_name; }
  const std::string& getName() const { return m_name; }

  uint32_t    id() const { return m_id; }
  uint32_t getId() const { return m_id; }   

  void print(std::ostream &os) const;
   
 private:
  
  uint32_t     m_id;   // Variable id
  std::string  m_name; // Variable name
};

std::string str(const TrigConfVar &);

//
// Helper functions: these use static TrigConfVar vector
//
namespace Trig
{
  uint16_t ReserveVarId ATLAS_NOT_THREAD_SAFE (const std::string &name);
  uint16_t ReserveVarId ATLAS_NOT_THREAD_SAFE (const std::string &name, uint16_t id);
  
  bool FindVarId ATLAS_NOT_THREAD_SAFE (const std::string &name, uint16_t &id);
  bool FindVarName ATLAS_NOT_THREAD_SAFE (const uint16_t id, std::string &name);
  
  std::vector<TrigConfVar> GetCurrentTrigConfVarVector ATLAS_NOT_THREAD_SAFE ();
}

//
// Inlined comparison operators
//
inline bool operator==(const TrigConfVar &lhs, const TrigConfVar &rhs) {
  return lhs.getId() == rhs.getId() || lhs.getName() == rhs.getName();
}
inline bool operator==(const TrigConfVar &key, const std::string &str) {
  return key.getName() == str;
}
inline bool operator==(const std::string &str, const TrigConfVar &key) {
  return str == key.getName();
}
inline bool operator==(const TrigConfVar &key, const unsigned int id) {
  return key.getId() == id;
}
inline bool operator==(const unsigned int id, const TrigConfVar &key) {
  return id == key.getId();
}

inline bool operator==(const TrigConfVar &key, const TrigMonVar &var) {
  return key.getId() == var.getId();
}
inline bool operator==(const TrigMonVar &var, const TrigConfVar &key) {
  return key.getId() == var.getId();
}

CLASS_DEF( TrigConfVar , 168672416 , 1 )

#endif
