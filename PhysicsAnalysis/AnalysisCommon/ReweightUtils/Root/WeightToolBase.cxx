/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "ReweightUtils/WeightToolBase.h"

#include "xAODParticleEvent/CompositeParticle.h"

// #include <TLorentzVector.h>


WeightToolBase::WeightToolBase( const std::string& name )
  : asg::AsgTool( name ),
    m_prefixWeightDecoration("weight_"){
    declareProperty("PrefixWeightDecoration", m_prefixWeightDecoration);
}


StatusCode WeightToolBase::initialize() {
  // Default implementation, such that users don't have to implement this any
  // more for the simplest use cases.
  ATH_MSG_DEBUG ("Initializing " << name() << "...");
  //setting weight name for decoration
  m_weightName = m_prefixWeightDecoration + this->name();
  m_weightName.replace(m_weightName.find("ToolSvc."), std::string("ToolSvc.").length(), "");

  return StatusCode::SUCCESS;
}


StatusCode WeightToolBase::finalize()
{
  // Default implementation, such that users don't have to implement this any
  // more for the simplest use cases.
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


bool WeightToolBase::addDecoration(const xAOD::EventInfo* evtInfo, float weight) const {
  if (!evtInfo) {
    ATH_MSG_ERROR("Cannot decorate empty EventInfo in "<< this->name());
    return false;
  }
  if (! evtInfo->isAvailable< float >( m_weightName ))
    evtInfo->auxdecor< float > (m_weightName) = weight;
  return true;
}
