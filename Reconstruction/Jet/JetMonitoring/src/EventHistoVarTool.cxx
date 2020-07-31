/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/EventHistoVarTool.h"


EventHistoVarTool::EventHistoVarTool(const std::string & type, const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )

{
  declareInterface<IEventHistoVarTool>(this);
}

StatusCode EventHistoVarTool::initialize() {

  if(m_varName=="") m_varName = name();

  return StatusCode::SUCCESS;
  
}

float EventHistoVarTool::value(const xAOD::EventInfo & e, const xAOD::JetContainer & /*jets not used in this implementation*/) const {

  if (! e.isAvailable<float>(m_attName) ) {
    ATH_MSG_WARNING("Could not access EventInfo variable "<< m_attName << ", returning default value " << m_defaultValue );
    return m_defaultValue;
  }

  return e.auxdata<float>(m_attName);

}
