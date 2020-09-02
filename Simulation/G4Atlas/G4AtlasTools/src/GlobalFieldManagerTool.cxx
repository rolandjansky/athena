/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Main header
#include "G4AtlasTools/GlobalFieldManagerTool.h"

// Local package include
#include "TightMuonSteppingFieldManager.h"

// Geant4 includes
#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagneticField.hh"
#include "G4PropagatorInField.hh"
#include "G4TransportationManager.hh"
#include "G4Version.hh"
#include "G4VIntegrationDriver.hh"

//-----------------------------------------------------------------------------
// Tool constructor
//-----------------------------------------------------------------------------
GlobalFieldManagerTool::GlobalFieldManagerTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)
  : G4FieldManagerToolBase(type, name, parent)
{
}

//-----------------------------------------------------------------------------
// Initialize a field manager
//-----------------------------------------------------------------------------
StatusCode GlobalFieldManagerTool::initializeField()
{
  ATH_MSG_DEBUG("initializeField");

  if (m_fieldOn) {

    // Retrieve the G4MagneticField
    G4MagneticField* field = m_fieldSvc->getField();

    // Retrieve the global field manager
    auto transpManager = G4TransportationManager::GetTransportationManager();
    G4FieldManager* fieldMgr(nullptr);
    if (m_useTightMuonStepping){
      // In the case of tight stepping we need to make our own global field manager
      // If field manager already exists for current thread, error.
      // There is no foreseen use-case for this situation.
      if(m_fieldMgrHolder.get()) {
        ATH_MSG_ERROR("GlobalFieldManagerTool::initializeField() - " <<
                      "Field manager already exists!");
        return StatusCode::FAILURE;
      }
      // Create a new field manager
      fieldMgr = new TightMuonSteppingFieldManager();

      // Save it in the TL holder
      m_fieldMgrHolder.set(fieldMgr);

      // Assign it to the global field manager
      transpManager->SetFieldManager(fieldMgr);
    } else {
      // Otherwise get the default from the transportation manager
      fieldMgr = transpManager->GetFieldManager();
    }

    // Configure the field manager
    fieldMgr->SetDetectorField(field);
    ATH_CHECK( setFieldParameters(fieldMgr) );

    // Create and configure the ChordFinder
    fieldMgr->CreateChordFinder(field);

#if G4VERSION_NUMBER < 1040
    ATH_MSG_DEBUG("Old style stepper setting");
    G4MagIntegratorStepper* stepper = getStepper(m_integratorStepper, field);
    G4MagInt_Driver* magDriver = fieldMgr->GetChordFinder()->GetIntegrationDriver();
    magDriver->RenewStepperAndAdjust(stepper);
#else
    ATH_MSG_DEBUG("New style stepper setting");
    G4VIntegrationDriver* driver = createDriverAndStepper(m_integratorStepper, field);
    G4ChordFinder* chordFinder = fieldMgr->GetChordFinder();
    chordFinder->SetIntegrationDriver(driver);
#endif

    // Configure the propagator
    G4PropagatorInField* propagator = transpManager->GetPropagatorInField();
    if (m_maxStep>0) propagator->SetLargestAcceptableStep(m_maxStep);

  }

  return StatusCode::SUCCESS;
}
