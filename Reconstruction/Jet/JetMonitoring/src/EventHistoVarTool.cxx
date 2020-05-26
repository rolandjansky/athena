/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/EventHistoVarTool.h"


EventHistoVarTool::EventHistoVarTool(const std::string & name ,const IInterface* parent):
  AthAlgTool( "float", name, parent )

{
  declareInterface<IEventHistoVarTool>(this);
}

StatusCode EventHistoVarTool::initialize() {

  if(m_varName=="") m_varName = name();


  /*if (! bool(m_v) ) {
      ATH_MSG_ERROR("Could not access EventInfo variable "<< m_varName );
      return StatusCode::FAILURE;      
  }*/
  return StatusCode::SUCCESS;
  
}
