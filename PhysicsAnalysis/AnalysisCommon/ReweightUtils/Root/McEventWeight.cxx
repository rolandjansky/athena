/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "ReweightUtils/McEventWeight.h"

#include "xAODParticleEvent/CompositeParticle.h"


McEventWeight::McEventWeight( const std::string& name )
  : WeightToolBase( name ),
    m_McEventWeightIndex(0),
    m_eventInfoName("EventInfo") {
  declareProperty( "McEventWeightIndex", m_McEventWeightIndex);
  declareProperty("EventInfoName", m_eventInfoName);
}


// returns: the value that was calculated from the xAOD::IParticle composite built event
// This is the implementation of the interface
double McEventWeight::evaluate( const xAOD::IParticle* /*part*/ ) const
{
  ATH_MSG_DEBUG ("Evaluating " << name() << "...");

  // Get the proper weight (from EventInfo)
  return this->getWeight();
}

double McEventWeight::computeWeight(const xAOD::EventInfo* evtInfo) const {
  // don't do anything for data
  if(!evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){
    ATH_MSG_DEBUG (" Returning weight=1. for data.");
    return 1.;
  }
  const double weight = evtInfo->mcEventWeight(m_McEventWeightIndex);
  return weight;
}

double McEventWeight::getWeight() const {
 //Retrieveing eventInfo
  const xAOD::EventInfo* evtInfo;
  StatusCode sc = evtStore()->retrieve( evtInfo, m_eventInfoName );
  if(sc.isFailure() || !evtInfo) {
    ATH_MSG_ERROR (" EventInfo could not be retrieved !!");
    return 0.;
  }

  double weight = this->computeWeight(evtInfo);
  ATH_MSG_DEBUG ("     " << name() << " returning weight= " << weight << ".");
  return weight;
}
