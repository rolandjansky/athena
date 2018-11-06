/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArHVData.h"
#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

StatusCode LArHVData::getHV(const Identifier& id, std::vector< HV_t > & v  ) const {

   v.clear();
   if(m_voltage.find(id) != m_voltage.end()) { // fill output
         for(auto iptr : m_voltage.at(id)) v.push_back(iptr);
         return StatusCode::SUCCESS;
   } else { // do not have such data ?
         return StatusCode::FAILURE;
   }
}

StatusCode LArHVData::getCurrent(const Identifier& id, std::vector< CURRENT_t > & v  ) const {

   v.clear();
   if(m_current.find(id) != m_current.end()) { // fill output
         for(auto iptr : m_current.at(id)) v.push_back(iptr);
         return StatusCode::SUCCESS;
   } else { // do not have such data ?
         return StatusCode::FAILURE;
   }
}
