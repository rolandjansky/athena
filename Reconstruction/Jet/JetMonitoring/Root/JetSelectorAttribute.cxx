/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetSelectorAttribute.h"
#include "JetMonitoring/JetVariable.h"
#include <limits>


JetSelectorAttribute::JetSelectorAttribute(const std::string &t) 
  : asg::AsgTool(t)
  , m_min(-std::numeric_limits<float>::max())
  , m_max( std::numeric_limits<float>::max())
  , m_var(this)
{

  declareProperty("CutMin", m_min );
  declareProperty("CutMax", m_max );
  declareProperty("Var", m_var);
}

JetSelectorAttribute:: ~JetSelectorAttribute(){
}

StatusCode JetSelectorAttribute::initialize() {

  ATH_CHECK(m_var.retrieve() );
  ATH_MSG_INFO( "Selecting on var ("<< m_var->describe() << ") in ["<< m_min << " , "<< m_max<< "]");

  return StatusCode::SUCCESS;
}

int JetSelectorAttribute::keep(const xAOD::Jet& jet) const {
  float v = m_var->value(jet);
  return (m_min < v ) && (v<m_max);
}
