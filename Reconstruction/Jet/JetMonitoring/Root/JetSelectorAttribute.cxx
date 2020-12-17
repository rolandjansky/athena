/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetSelectorAttribute.h"
#include "JetMonitoring/JetVariable.h"
#include <limits>


JetSelectorAttribute::JetSelectorAttribute(const std::string &t) 
  : asg::AsgTool(t)
  //, m_min(-std::numeric_limits<float>::max())
  //, m_max(std::numeric_limits<float>::max())
  , m_min(10)
  , m_max(10)
  , m_var(this)
{
  declareProperty("CutMin", m_min );
  declareProperty("CutMax", m_max );
  declareProperty("Var", m_var);
}

JetSelectorAttribute:: ~JetSelectorAttribute(){
}

StatusCode JetSelectorAttribute::initialize() {

  unsigned int it=0;
  for (auto var : m_var) {
    ATH_CHECK(var.retrieve() );
    ATH_MSG_INFO( "Selecting on var ("<< var->describe() << ") in ["<< m_min.at(it) << " , "<< m_max.at(it)<< "]");
    it++;
  }
  return StatusCode::SUCCESS;
}

int JetSelectorAttribute::keep(const xAOD::Jet& jet) const {
  unsigned int it=0;
  for (auto var : m_var) {
    float v = var->value(jet);
    if ((m_min.at(it) > v) || (m_max.at(it) < v)) return false;
    it++;
  }
  return true;
}
