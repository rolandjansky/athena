/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetEventSelector.h"


JetEventSelector::JetEventSelector(const std::string &t) 
  : asg::AsgTool(t)
  , m_var(this)
{

  declareProperty("CutMin", m_min );
  declareProperty("CutMax", m_max );
  declareProperty("Var", m_var);
}

JetEventSelector:: ~JetEventSelector(){
}

StatusCode JetEventSelector::initialize() {

  unsigned int it=0;
  for (auto var : m_var) {
    ATH_CHECK(var.retrieve() );
    ATH_MSG_INFO( "Selecting on var ("<< var->varName() << ") in ["<< m_min.at(it) << " , "<< m_max.at(it)<< "]");
    it++;
  }
  return StatusCode::SUCCESS;
}

int JetEventSelector::keep(const xAOD::EventInfo& e, const xAOD::JetContainer & jets) const {
  unsigned int it=0;
  for (auto var : m_var) {
    float v = var->value(e, jets);
    if ((m_min.at(it) > v) || (m_max.at(it) < v)) return false;
    it++;
  }
  return true;
}
