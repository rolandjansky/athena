/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkSUSY/SUSYIDWeight.h"

#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODTruth/TruthEventContainer.h"

//namespace DerivationFramework {
  
  static SG::AuxElement::Accessor<int> acc_procID("SUSY_procID");
  
  SUSYIDWeight::SUSYIDWeight( const std::string& name )
    : WeightToolBase( name ),
      m_SUSYProcID(0),
      m_eventInfoName("EventInfo") {
    declareProperty( "SUSYProcID", m_SUSYProcID = 0);
    declareProperty("EventInfoName", m_eventInfoName = "EventInfo");
    declareProperty("UseTruthEvents" ,m_useTruthEvents=false, "To solve ATLASSIM-2989 we have to switch to using TruthEvents for weights");
  }
  
  // returns: the value that was calculated from the xAOD::IParticle composite built event
  // This is the implementation of the interface
  double SUSYIDWeight::evaluate( const xAOD::IParticle* /*part*/ ) const
  {
    ATH_MSG_DEBUG ("Evaluating " << name() << "...");
    
    // Get the proper weight (from EventInfo)
    return this->getWeight();
  }
  
  double SUSYIDWeight::computeWeight(const xAOD::EventInfo* evtInfo) const {
    // don't do anything for data
    if(!evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){
      ATH_MSG_DEBUG (" Returning weight=1. for data.");
      return 1.;
    }
    
    //Check for requested Process ID, otherwise return 0.
    if (acc_procID.isAvailable(*(evtInfo))) {
      if( acc_procID(*(evtInfo)) != m_SUSYProcID )
	return 0.;
    }
    else{
      ATH_MSG_WARNING("The SUSY_procID decoration requested is not available! You need to run SUSYSignalTagger beforehand.");
      return 0.;
    }
    
    if(m_useTruthEvents) {
      //access TruthEvents container instead
      const xAOD::TruthEventContainer* te = 0;
      if( evtStore()->retrieve( te, "TruthEvents" ).isFailure() ) {
	ATH_MSG_ERROR("TruthEvents could not be retrieved. Throwing exception");
	throw std::runtime_error("SUSYIDWeight ASG Tool: TruthEvents could not be retrieved");
      }
      if( te->size() == 0 ) {
	ATH_MSG_ERROR("TruthEvents has no events in it. Throwing exception");
	throw std::runtime_error("SUSYIDWeight ASG Tool: TruthEvents has no events in it");
      }
      const double weight = te->at(0)->weights()[0];
      ATH_MSG_DEBUG ("Got weight in SUSYIDWeight::computeWeight() = " << weight << ".");
      return weight;
    }
    
    const double weight = evtInfo->mcEventWeight(0);
    ATH_MSG_DEBUG ("Got weight in SUSYIDWeight::computeWeight() = " << weight << ".");
    return weight;
  }
  
  double SUSYIDWeight::getWeight() const {
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
//}
