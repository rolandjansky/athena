/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetHistoVarTool.h"


JetHistoVarTool::JetHistoVarTool( const std::string& type,  const std::string & name ,const IInterface* parent):
  AthAlgTool( type, name, parent )

{
  declareInterface<IJetHistoVarTool>(this);

  
}

StatusCode JetHistoVarTool::initialize() {

  if(m_name=="") m_name = name();
  m_v = JetVar::Variable::create( m_name, m_type, m_index);
  m_v->setScale(m_scale);

  if (! bool(m_v) ) {
      ATH_MSG_ERROR(" could not create Jet Variable "<< m_name << " type: "<< m_type << "  index="<< m_index );
      return StatusCode::FAILURE;      
  }
  return StatusCode::SUCCESS;
  
}


float JetHistoVarTool::value(const xAOD::Jet &j) const {
  return m_v->value(j) ;
}
