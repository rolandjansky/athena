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
