/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Main header
#include "G4AtlasTools/DetectorFieldManagerTool.h"

// From this package
#include "TightMuonElseNoFieldManager.h"

// Geant4 includes
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"

//-----------------------------------------------------------------------------
// Tool constructor
//-----------------------------------------------------------------------------
DetectorFieldManagerTool::DetectorFieldManagerTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
  : G4FieldManagerToolBase(type, name, parent)
{
  declareProperty("LogicalVolumes", m_volumeList,
                  "List of volumes to which the field will be applied");
  declareProperty("MuonOnlyField", m_muonOnlyField,
                  "Only muons experience the magnetic field");
}

//-----------------------------------------------------------------------------
// Initialize a field manager
//-----------------------------------------------------------------------------
StatusCode DetectorFieldManagerTool::initializeField()
{
  ATH_MSG_DEBUG("initializeField");

  if (m_fieldOn) {

    // If field manager already exists for current thread, error.
    // There is no foreseen use-case for this situation.
    if(m_fieldMgrHolder.get()) {
      ATH_MSG_ERROR("DetectorFieldManagerTool::initializeField() - " <<
                    "Field manager already exists!");
      return StatusCode::FAILURE;
    }

    // Create a new field manager
    G4FieldManager * fieldMgr = nullptr;
    if (m_muonOnlyField){
      fieldMgr = new TightMuonElseNoFieldManager();
    } else {
      fieldMgr = new G4FieldManager();
    }

    // Save it in the TL holder
    m_fieldMgrHolder.set(fieldMgr);

    // Retrieve the G4MagneticField
    G4MagneticField* field = m_fieldSvc->getField();

    // Configure the field manager
    fieldMgr->SetDetectorField(field);
    fieldMgr->CreateChordFinder(field);
    ATH_CHECK( setFieldParameters(fieldMgr) );

    // Construct the stepper
    auto stepper = getStepper(m_integratorStepper, field);
    fieldMgr->GetChordFinder()->GetIntegrationDriver()->RenewStepperAndAdjust(stepper);

    // Assign the field manager to volumes
    auto logVolStore = G4LogicalVolumeStore::GetInstance();
    for (const auto& volume: m_volumeList) {
      G4LogicalVolume* logicalVolume = logVolStore->GetVolume(volume);
      if (logicalVolume) logicalVolume->SetFieldManager(fieldMgr, true);
      else {
        ATH_MSG_WARNING("No volume called " << volume <<
                        " was found in the G4LogicalVolumeStore! Skipping this volume.");
      }
    }

  }

  return StatusCode::SUCCESS;
}
