/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArHVData.h"
#include "Identifier/Identifier.h"

const std::vector<LArHVData::HV_t> & LArHVData::getHV(const Identifier& id) const {
  const auto& it=m_voltage.find(id);
  if (it==m_voltage.end()) 
    return m_empty; //return empty vector
  else {
    return it->second;
  }
}
  

