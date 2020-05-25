/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/EventHistoVarTool.h"


EventHistoVarTool::EventHistoVarTool( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )

{
  declareInterface<IEventHistoVarTool>(this);

  
}

StatusCode EventHistoVarTool::initialize() {

  if(m_name=="") m_name = name();
  m_v = EventVar::Variable::create( m_name, m_type, m_index);
  m_v->setScale(m_scale);

  if (! bool(m_v) ) {
      ATH_MSG_ERROR(" could not create Event Variable "<< m_name << " type: "<< m_type << "  index="<< m_index );
      return StatusCode::FAILURE;      
  }
  return StatusCode::SUCCESS;
  
}


float EventHistoVarTool::value(const xAOD::EventInfo &e) const {
  return m_v->value(e) ;
}
