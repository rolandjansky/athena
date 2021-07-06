/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetSelectorAttribute.h"
#include "JetMonitoring/JetVariable.h"


JetSelectorAttribute::JetSelectorAttribute(const std::string &t) 
  : asg::AsgTool(t)
  , m_var(this)
{
  declareProperty("CutMin", m_min );
  declareProperty("CutMax", m_max );
  declareProperty("Var", m_var);
}

JetSelectorAttribute:: ~JetSelectorAttribute(){
}

StatusCode JetSelectorAttribute::initialize() {

  for (unsigned int it = 0; it < m_var.size(); it++) {
    ATH_CHECK(m_var[it].retrieve());
    ATH_MSG_INFO( "Selecting on var ("<< m_var[it]->describe() << ") in ["<< m_min.at(it) << " , "<< m_max.at(it)<< "]");
  }
  return StatusCode::SUCCESS;
}

int JetSelectorAttribute::keep(const xAOD::Jet& jet) const {

  for (unsigned int it = 0; it < m_var.size(); it++) {
    float v = m_var[it]->value(jet);
    if ((m_min.at(it) > v) || (m_max.at(it) < v)) return false;
  }
  return true;
}
