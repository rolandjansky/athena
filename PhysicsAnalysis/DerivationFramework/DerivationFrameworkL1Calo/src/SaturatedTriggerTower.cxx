/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SaturatedTriggerTower.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Ben Allen (benjamin.william.allen@cern.ch)
// Adapted heavily from code originally written by
// James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkL1Calo/SaturatedTriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {
    
    SaturatedTriggerTower::SaturatedTriggerTower(const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p) :
    AthAlgTool(t,n,p),
    m_adcThreshold(0),
    m_collName("xAODTriggerTowers")
    {
        declareInterface<DerivationFramework::ISkimmingTool>(this);
	declareProperty("TriggerTowerContainer",m_collName);
	declareProperty("adcThreshold",m_adcThreshold);
    }
    
    StatusCode SaturatedTriggerTower::initialize()
    {
      ATH_MSG_VERBOSE("initialize() ...");
      return StatusCode::SUCCESS;
    }
    
    StatusCode SaturatedTriggerTower::finalize()
    {
        return StatusCode::SUCCESS;
    }
    
    bool SaturatedTriggerTower::eventPassesFilter() const
    {
	bool acceptEvent(false);
        
        // check for triggertowers
        if (evtStore()->contains<xAOD::TriggerTowerContainer>( m_collName )) {
        
          // Retrieve data
          const xAOD::TriggerTowerContainer* triggertowers(nullptr);
          CHECK( evtStore()->retrieve( triggertowers , m_collName ) );
          
          if( !triggertowers ) {
            ATH_MSG_ERROR("Couldn't retrieve triggertower container with key: " << m_collName);
            return StatusCode::FAILURE;
          }
          // LOOP OVER TRIGGERTOWERS
          int adcvalue(0);
          for (xAOD::TriggerTowerContainer::const_iterator eIt = triggertowers->begin(); eIt!=triggertowers->end(); ++eIt) {
            for(int i=0; i<5; i++){
              adcvalue = (*eIt)->adc()[i];
              if(adcvalue > m_adcThreshold){
                acceptEvent = true;
                return acceptEvent;
              }
            }
          }
    
        } // check for trigger towers
       
      return acceptEvent;
    }
    
} // end of namespace

