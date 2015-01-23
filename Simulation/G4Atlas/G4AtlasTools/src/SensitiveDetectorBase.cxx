/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// Geant4 includes used in functions
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

SensitiveDetectorBase::SensitiveDetectorBase(const std::string& type, const std::string& name, const IInterface* parent)
  : G4VSensitiveDetector(name)
  , AthAlgTool(type,name,parent)
{
  declareProperty("LogicalVolumeNames" , m_volumeNames );
}

// Athena method, used to get out the G4 geometry and set up the SDs
StatusCode SensitiveDetectorBase::initialize(){
  // Add the sensitive detector to the SD manager in G4
  G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  SDmanager->AddNewDetector(this);
  ATH_MSG_DEBUG( "Initialized and added SD " << name() );

  // Go through the logical volumes and hook the SDs up
  bool gotOne = false;
  G4LogicalVolumeStore * logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
  for (auto myvol : m_volumeNames){
    int found = 0;
    for (auto ilv : *logicalVolumeStore ){
      if (ilv->GetName() == myvol.data()){
        ++found; // Do not break on found to protect against multiple volumes with the same name
        ilv->SetSensitiveDetector(this);
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
  if (!gotOne){
    ATH_MSG_ERROR( "Failed to assign *any* volume to SD " << name() );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
  
// Just the end of a G4 event
void SensitiveDetectorBase::EndOfEvent(G4HCofThisEvent*){
  // Can be called - this may not do anything for many ATLAS SDs
}

// End of an athena event - store the hit collection in SG at this point
void SensitiveDetectorBase::Gather(){
  ATH_MSG_ERROR("Should not be called on base class!");
  throw "ShouldNotBeCalledOnBaseClass";
}

// Process hits - pure virtual, as all derived classes should (and do) override this
G4bool SensitiveDetectorBase::ProcessHits(G4Step*,G4TouchableHistory*){
  ATH_MSG_ERROR("Should not be called on base class!");
  throw "ShouldNotBeCalledOnBaseClass";
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
