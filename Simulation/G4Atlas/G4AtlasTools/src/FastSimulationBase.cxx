/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasTools/FastSimulationBase.h"

#include "G4RegionStore.hh"
#include "G4FastSimulationManager.hh"

FastSimulationBase::FastSimulationBase(const std::string& type, const std::string& name, const IInterface* parent)
  : G4VFastSimulationModel(name)
  , AthAlgTool(type,name,parent)
{
  declareProperty("RegionNames" , m_regionNames );
}

// Athena method, used to get out the G4 geometry and set up the SDs
StatusCode FastSimulationBase::initialize(){

  // Go through the regions and hook the fast simulation up
  G4RegionStore* regionStore = G4RegionStore::GetInstance();
  bool missedOne = false;
  for (auto myreg : m_regionNames){
    int found=0; // Regions with more than one name...
    for (auto areg : *regionStore){
      if (myreg.data()==areg->GetName()){
        ++found;
        G4FastSimulationManager* theFastSimulationManager = areg->GetFastSimulationManager();
  
        // Build a new fast sim manager if necessary
        if ( theFastSimulationManager == 0 ) theFastSimulationManager = new G4FastSimulationManager(areg);
        theFastSimulationManager->AddFastSimulationModel(this);
        theFastSimulationManager->ActivateFastSimulationModel(name());
      }
    } // Loop over regions
    if (0==found){
      ATH_MSG_ERROR( "Region " << myreg << " not found." );
      missedOne=true;
    } else {
      ATH_MSG_VERBOSE( found << " copies of region " << myreg << " found; fast simulation " << name() << " assigned." );
    }
  } // Loop over regions I want

  // Crash out if we have failed to assign a volume - this is bad news!
  if (missedOne){
    ATH_MSG_ERROR( "Failed to assign at least one volume to SD " << name() );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

