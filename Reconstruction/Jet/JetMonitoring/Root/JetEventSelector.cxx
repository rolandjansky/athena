/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetEventSelector.h"
#include <limits>


JetEventSelector::JetEventSelector(const std::string &t) 
  : asg::AsgTool(t)
  , m_min(-std::numeric_limits<float>::max())
  , m_max( std::numeric_limits<float>::max())
  , m_var(this)
{

  declareProperty("CutMin", m_min );
  declareProperty("CutMax", m_max );
  declareProperty("Var", m_var);
}

JetEventSelector:: ~JetEventSelector(){
}

StatusCode JetEventSelector::initialize() {

  ATH_CHECK(m_var.retrieve() );
  ATH_MSG_INFO( "Selecting on var ("<< m_var->varName() << ") in ["<< m_min << " , "<< m_max<< "]");

  return StatusCode::SUCCESS;
}

int JetEventSelector::keep(const xAOD::EventInfo& e, const xAOD::JetContainer & jets) const {
  float v = m_var->value(e, jets);
  return (m_min < v ) && (v<m_max);
}
