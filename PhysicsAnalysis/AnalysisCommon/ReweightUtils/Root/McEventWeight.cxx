/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "ReweightUtils/McEventWeight.h"

#include "xAODParticleEvent/CompositeParticle.h"

#include "xAODTruth/TruthEventContainer.h"

McEventWeight::McEventWeight( const std::string& name )
  : WeightToolBase( name ),
    m_McEventWeightIndex(0),
    m_eventInfoName("EventInfo") {
  declareProperty( "McEventWeightIndex", m_McEventWeightIndex = 0);
  declareProperty("EventInfoName", m_eventInfoName = "EventInfo");
  declareProperty("UseTruthEvents",m_useTruthEvents=false,"To solve ATLASSIM-2989 we have to switch to using TruthEvents for weights");
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
  
  
  if(m_useTruthEvents) {
    //access TruthEvents container instead
    const xAOD::TruthEventContainer* te = 0;
    if( evtStore()->retrieve( te, "TruthEvents" ).isFailure() ) {
      ATH_MSG_ERROR("TruthEvents could not be retrieved. Throwing exception");
      throw std::runtime_error("McEventWeight ASG Tool: TruthEvents could not be retrieved");
    }
    if( te->size() == 0 ) {
      ATH_MSG_ERROR("TruthEvents has no events in it. Throwing exception");
      throw std::runtime_error("McEventWeight ASG Tool: TruthEvents has no events in it");
    }
    if( te->at(0)->weights().size() <= m_McEventWeightIndex) {
      ATH_MSG_ERROR("TruthEvents first event has " << te->at(0)->weights().size() << " weights, but " 
            << m_McEventWeightIndex << " weight requested. Throwing exception");
      throw std::runtime_error("McEventWeight ASG Tool: TruthEvents has too few weights.");
    }
    const double weight = te->at(0)->weights()[m_McEventWeightIndex];
    ATH_MSG_DEBUG ("Got weight in McEventWeight::computeWeight() = " << weight << ".");
    return weight;
  }
  
  const double weight = evtInfo->mcEventWeight(m_McEventWeightIndex);
  ATH_MSG_DEBUG ("Got weight in McEventWeight::computeWeight() = " << weight << ".");
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
  ATH_MSG_VERBOSE ("     " << name() << " returning weight= " << weight << ".");
  return weight;
}
