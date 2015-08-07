/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// Geant4 includes used in functions
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

SensitiveDetectorBase::SensitiveDetectorBase(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent)
#ifndef ATHENAHIVE
    , m_SD(nullptr)
#endif
{
  declareProperty("LogicalVolumeNames" , m_volumeNames );
  declareProperty("NoVolumes", m_noVolumes=false );
  declareProperty("OutputCollectionNames", m_outputCollectionNames );
}

// Athena method, used to get out the G4 geometry and set up the SDs
StatusCode SensitiveDetectorBase::initializeSD()
{
  ATH_MSG_VERBOSE( name() << "::initializeSD()" );

  // Make sure SD isn't already registered
  if(getSD())
    {
      ATH_MSG_ERROR("Trying to create a SD which already exists!");
      return StatusCode::FAILURE;
    }

  // Make the SD stored by this tool
  auto* sd = makeSD();
  if(!sd)
    {
      ATH_MSG_ERROR("Failed to create SD!");
      return StatusCode::FAILURE;
    }
  setSD(sd);

  // Set the verbosity information on this thing
  if(msgLvl(MSG::VERBOSE)) getSD()->SetVerboseLevel(10);
  else if(msgLvl(MSG::DEBUG)) getSD()->SetVerboseLevel(5);

  // Add the sensitive detector to the SD manager in G4
  G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  SDmanager->AddNewDetector(getSD());
  ATH_MSG_DEBUG( "Initialized and added SD " << name() );

  // Go through the logical volumes and hook the SDs up
  bool gotOne = false;
  G4LogicalVolumeStore * logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
  for (auto myvol : m_volumeNames){
    int found = 0;
    for (auto ilv : *logicalVolumeStore ){
      if (ilv->GetName() == myvol.data()){
        ++found; // Do not break on found to protect against multiple volumes with the same name
        ilv->SetSensitiveDetector(getSD());
        gotOne = true;
      } // Found a volume!
    } // Loop over all the volumes in the geometry
    // Give notice if we have missed a volume in here
    if (0==found){
      ATH_MSG_WARNING( "Volume " << myvol << " not found in G4LogicalVolumeStore." );
    } else {
      ATH_MSG_VERBOSE( found << " copies of LV " << myvol << " found; SD " << name() << " assigned." );
    }
  } // Loop over my volumes

  // Crash out if we have failed to assign a volume - this is bad news!
  if (!gotOne && !m_noVolumes){
    ATH_MSG_ERROR( "Failed to assign *any* volume to SD " << name() << " and expected at least one." );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode
SensitiveDetectorBase::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == ISensitiveDetector::interfaceID() ) {
    *ppvIf = (ISensitiveDetector*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface( riid, ppvIf );
}

G4VSensitiveDetector* SensitiveDetectorBase::getSD()
{
#ifdef ATHENAHIVE
  // Get current thread-ID
  const auto tid = std::this_thread::get_id();
  // Retrieve it from the SD map
  auto sdPair = m_sdThreadMap.find(tid);
  if(sdPair == m_sdThreadMap.end()) return nullptr;
  return sdPair->second;
#else
  return m_SD;
#endif
}

void SensitiveDetectorBase::setSD(G4VSensitiveDetector* sd)
{
#ifdef ATHENAHIVE
  const auto tid = std::this_thread::get_id();
  ATH_MSG_DEBUG("Creating and registering SD " << sd << " in thread " << tid);
  m_sdThreadMap.insert( std::make_pair(tid, sd) );
#else
  m_SD = sd;
#endif
}
