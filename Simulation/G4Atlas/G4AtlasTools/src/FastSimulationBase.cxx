/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasTools/FastSimulationBase.h"

// Geant4 includes used in functions
#include "G4RegionStore.hh"
#include "G4FastSimulationManager.hh"

FastSimulationBase::FastSimulationBase(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent)
#ifndef G4MULTITHREADED
  , m_FastSimModel(nullptr)
#endif
{
  declareProperty("RegionNames" , m_regionNames );
  declareProperty("NoRegions", m_noRegions=false );
}

// Athena method, used to get out the G4 geometry and set up the Fast Simulation Models
StatusCode FastSimulationBase::initializeFastSim(){
  ATH_MSG_VERBOSE( name() << "::initializeFastSim()" );

  // Make sure Fast Simulation Model isn't already registered
  if(getFastSimModel()){
    ATH_MSG_ERROR("Trying to create a Fast Simulation Model which already exists!");
    return StatusCode::FAILURE;
  }

  // Make the FastSimModel stored by this tool
  auto* fastsimmodel = makeFastSimModel();
  setFastSimModel(fastsimmodel);

  // Set the verbosity information on this thing - this will have to go into the makeFastSimModel methods...
  //if(msgLvl(MSG::VERBOSE)) m_FastSimModel->SetVerboseLevel(10);
  //else if(msgLvl(MSG::DEBUG)) m_FastSimModel->SetVerboseLevel(5);

  // Go through the regions and hook the fast simulation up
  G4RegionStore* regionStore = G4RegionStore::GetInstance();
  bool missedOne = false;
  for (const auto& myreg : m_regionNames){
    int found=0; // Regions with more than one name...
    for (auto* areg : *regionStore){
      if (myreg.data()==areg->GetName()){
        ++found;
        G4FastSimulationManager* theFastSimulationManager = areg->GetFastSimulationManager();

        // Build a new fast sim manager if necessary
        if ( theFastSimulationManager == 0 ) theFastSimulationManager = new G4FastSimulationManager(areg);
        theFastSimulationManager->AddFastSimulationModel(getFastSimModel());
        theFastSimulationManager->ActivateFastSimulationModel(getFastSimModel()->GetName());
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
  if (missedOne && !m_noRegions){
    ATH_MSG_ERROR( "Failed to assign at least one volume to Fast Simulation Model " << name() );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode FastSimulationBase::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IFastSimulation::interfaceID() ) {
    *ppvIf = (IFastSimulation*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface( riid, ppvIf );
}

G4VFastSimulationModel* FastSimulationBase::getFastSimModel()
{
#ifdef G4MULTITHREADED
  // Get current thread-ID
  const auto tid = std::this_thread::get_id();
  // Retrieve it from the FastSimModel map
  auto fastsimmodelPair = m_fastsimmodelThreadMap.find(tid);
  if(fastsimmodelPair == m_fastsimmodelThreadMap.end()) return nullptr;
  return fastsimmodelPair->second;
#else
  return m_FastSimModel;
#endif
}

void FastSimulationBase::setFastSimModel(G4VFastSimulationModel* fastsimmodel)
{
#ifdef G4MULTITHREADED
  // Make sure one isn't already assigned
  const auto tid = std::this_thread::get_id();
  ATH_MSG_DEBUG("Creating and registering FastSimModel " << fastsimmodel << " in thread " << tid);
  m_fastsimmodelThreadMap.insert( std::make_pair(tid, fastsimmodel) );
#else
  m_FastSimModel = fastsimmodel;
#endif
}
