/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_ALG_P1_H
#define TRIGCONF_ALG_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Converter class for TrigConfAlg.
*/

#include <stdint.h>
#include <string>

class TrigConfAlg_p1
{
 public:
  
  TrigConfAlg_p1() : m_index(0), m_position(0), m_name_id(0), m_type_id(0), m_name(), m_type() {}
  ~TrigConfAlg_p1() {}
  
  friend class TrigConfAlgCnv_p1;

 private:
  
  uint16_t    m_index;
  uint8_t     m_position;
  uint32_t    m_name_id;
  uint32_t    m_type_id;
  
  std::string m_name;
  std::string m_type;
};
	
#endif
