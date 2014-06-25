/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_ALG_H
#define TRIGCONF_ALG_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief This class provides map between monitoring
   object TrigMonAlg and algorithm configuration. 
*/

// Framework
#include "CLIDSvc/CLASS_DEF.h"

// C/C++
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

class TrigConfAlg {
 public:
  
  TrigConfAlg();
  TrigConfAlg(const uint32_t index,
	     const uint32_t position,
	     const std::string name,
	     const std::string type,
	     const uint32_t name_id,
	     const uint32_t type_id);
  ~TrigConfAlg() {}

  void clearStrings();
  
  const std::string& getName() const { return m_name; }
  const std::string& getType() const { return m_type; }
  
  uint16_t getIndex()    const { return m_index; }
  uint8_t  getPosition() const { return m_position; }
  uint32_t getNameId()   const { return m_name_id; }
  uint32_t getTypeId()   const { return m_type_id; }
  
  void print(std::ostream &os = std::cout) const;
  
 private:
  
  uint16_t    m_index;     // Unique algorithm index in current configuration
  uint8_t     m_position;  // Algorithm position within sequence
  uint32_t    m_name_id;   // Hash id of name_alg
  uint32_t    m_type_id;   // Hash id of type_alg
  
  std::string m_name;      // Algorithm instance name
  std::string m_type;      // Algorithm base class name
};

std::string str(const TrigConfAlg &);

//
// Inline global operators
//
inline bool operator==(const TrigConfAlg &lhs, const TrigConfAlg &rhs) { 
  return lhs.getName() == rhs.getName();
}
inline bool operator!=(const TrigConfAlg &lhs, const TrigConfAlg &rhs) { 
  return !(lhs == rhs);
}

CLASS_DEF( TrigConfAlg , 93121329 , 1 )

#endif
