/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/G4AtlasRunManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "G4GeometryManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParallelWorldScoringProcess.hh"
#include "G4RegionStore.hh"
#include "G4Run.hh"
#include "G4ScoringManager.hh"
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"
#include "G4UserRunAction.hh"
#include "G4Version.hh"

#include <string>

G4AtlasRunManager::G4AtlasRunManager()
  : G4RunManager()
  , m_msg("G4AtlasRunManager")
  , m_recordFlux(false)
  , m_senDetTool("SensitiveDetectorMasterTool")
  , m_fastSimTool("FastSimulationMasterTool")
  , m_physListSvc("PhysicsListSvc", "G4AtlasRunManager")
  , m_userActionSvc("G4UA::UserActionSvc", "G4AtlasRunManager")
  , m_detGeoSvc("DetectorGeometrySvc", "G4AtlasRunManager")
{  }


G4AtlasRunManager* G4AtlasRunManager::GetG4AtlasRunManager()
{
  static G4AtlasRunManager *thisManager = nullptr;
  if (!thisManager) { thisManager = new G4AtlasRunManager; } // Leaked
  return thisManager;
}


void G4AtlasRunManager::Initialize()
{
  const std::string methodName = "G4AtlasRunManager::Initialize";
  
  // ADA 11/28.2018: switch initialization order to meet ISF requirements
  // Call the base class Initialize method. This will call
  // InitializeGeometry and InitializePhysics.
  G4RunManager::Initialize();
  
  // Setup the user actions now.
  if( !m_userActionSvc.name().empty() ) {
    ATH_MSG_INFO("Creating user actions now");
    if(m_userActionSvc.retrieve().isFailure()) {
      throw GaudiException("Could not retrieve UserActionSvc",
                           methodName, StatusCode::FAILURE);
    }
    if(m_userActionSvc->initializeActions().isFailure()) {
      throw GaudiException("Failed to initialize actions",
                           methodName, StatusCode::FAILURE);
    }
  }

}


void G4AtlasRunManager::InitializeGeometry()
{
  ATH_MSG_DEBUG( "InitializeGeometry()" );
  if (m_detGeoSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve the DetectorGeometrySvc" );
    G4ExceptionDescription description;
    description << "InitializeGeometry: Failed to retrieve IDetectorGeometrySvc.";
    G4Exception("G4AtlasRunManager", "CouldNotRetrieveDetGeoSvc", FatalException, description);
    abort(); // to keep Coverity happy
  }

  // Set smartlessness
  G4LogicalVolumeStore *logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
  if (logicalVolumeStore->size() == 0) {
      ATH_MSG_ERROR( "G4 logical volume store is empty." );
  }
  const G4String muonSys("Muon::MuonSys");
  const G4String embSTAC("LArMgr::LAr::EMB::STAC");
  bool ilvMuonSys = false, ilvEmbSTAC = false;
  for (auto* ilv : *logicalVolumeStore ) {
    if ( ilv->GetName() == muonSys ) {
      ilv->SetSmartless( 0.1 );
      ATH_MSG_INFO( "Set smartlessness for Muon::MuonSys to 0.1" );
      ilvMuonSys = true;
    }
    else if ( ilv->GetName() == embSTAC ) {
      ilv->SetSmartless( 0.5 );
      ATH_MSG_INFO( "Set smartlessness for LArMgr::LAr::EMB::STAC to 0.5" );
      ilvEmbSTAC = true;
    }
  }
  if (ilvMuonSys == false) {
      ATH_MSG_INFO( "Muon::MuonSys not in G4 logical volume store. Smartlessness not set." );
  }
  if (ilvEmbSTAC == false) {
      ATH_MSG_INFO( "LArMgr::LAr::EMB::STAC not in G4 logical volume store. Smartlessness not set." );
  }

  // Create/assign detector construction
  G4RunManager::SetUserInitialization(m_detGeoSvc->GetDetectorConstruction());
  if (userDetector) {
    G4RunManager::InitializeGeometry();
  }
  else {
    ATH_MSG_WARNING( " User Detector not set!!! Geometry NOT initialized!!!" );
  }

  // Geometry has been initialized.
  if (m_senDetTool.retrieve().isFailure()) { //svcLocator->service("SensitiveDetector",m_senDetSvc).isFailure())
    ATH_MSG_ERROR ( "Could not retrieve the SD master tool" );
    G4ExceptionDescription description;
    description << "InitializeGeometry: Failed to retrieve ISensitiveDetectorMasterTool.";
    G4Exception("G4AtlasRunManager", "CouldNotRetrieveSDMaster", FatalException, description);
    abort(); // to keep Coverity happy
  }
  if(m_senDetTool->initializeSDs().isFailure()) {
    G4ExceptionDescription description;
    description << "InitializeGeometry: Call to ISensitiveDetectorMasterTool::initializeSDs failed.";
    G4Exception("G4AtlasRunManager", "FailedToInitializeSDs", FatalException, description);
    abort(); // to keep Coverity happy
  }
  return;
}


void G4AtlasRunManager::EndEvent()
{
  ATH_MSG_DEBUG( "G4AtlasRunManager::EndEvent" );
}


void G4AtlasRunManager::InitializePhysics()
{
  ATH_MSG_INFO( "InitializePhysics()" );
  kernel->InitializePhysics();
  physicsInitialized = true;

  // Grab the physics list tool and set the extra options
  if (m_physListSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve the physics list tool" );
    G4ExceptionDescription description;
    description << "InitializePhysics: Failed to retrieve IPhysicsListSvc.";
    G4Exception("G4AtlasRunManager", "CouldNotRetrievePLTool", FatalException, description);
    abort(); // to keep Coverity happy
  }
  m_physListSvc->SetPhysicsOptions();

  // Fast simulations last
  if (m_fastSimTool.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve the FastSim master tool" );
    G4ExceptionDescription description;
    description << "InitializePhysics: Failed to retrieve IFastSimulationMasterTool.";
    G4Exception("G4AtlasRunManager", "CouldNotRetrieveFastSimMaster", FatalException, description);
    abort(); // to keep Coverity happy
  }
  if(m_fastSimTool->initializeFastSims().isFailure()) {
    G4ExceptionDescription description;
    description << "InitializePhysics: Call to IFastSimulationMasterTool::initializeFastSims failed.";
    G4Exception("G4AtlasRunManager", "FailedToInitializeFastSims", FatalException, description);
    abort(); // to keep Coverity happy
  }

  if (m_recordFlux) {
    m_fluxRecorder->InitializeFluxRecording();
  }

  return;
}

bool G4AtlasRunManager::ProcessEvent(G4Event* event)
{

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);

  currentEvent = event;

  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4AtlasRunManager::ProcessEvent: Event Aborted at Detector Simulation level" );
    currentEvent = nullptr;
    return true;
  }

  if (m_recordFlux) { m_fluxRecorder->RecordFlux(currentEvent); }

  this->StackPreviousEvent(currentEvent);
  bool abort = currentEvent->IsAborted();
  currentEvent = nullptr;

  return abort;
}

void G4AtlasRunManager::RunTermination()
{
  ATH_MSG_DEBUG( " G4AtlasRunManager::RunTermination() " );
  if (m_recordFlux) {
    m_fluxRecorder->WriteFluxInformation();
  }
  
  this->CleanUpPreviousEvents();
  previousEvents->clear();

  if (userRunAction) {
    userRunAction->EndOfRunAction(currentRun);
  }

  delete currentRun;
  currentRun = nullptr;
  runIDCounter++;

  ATH_MSG_VERBOSE( "Changing the state..." );
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);

  ATH_MSG_VERBOSE( "Opening the geometry back up" );
  G4GeometryManager::GetInstance()->OpenGeometry();

  ATH_MSG_VERBOSE( "Terminating the run...  State is " << stateManager->GetStateString( stateManager->GetCurrentState() ) );
  kernel->RunTermination();
  ATH_MSG_VERBOSE( "All done..." );

  userRunAction = nullptr;
  userEventAction = nullptr;
  userSteppingAction = nullptr;
  userStackingAction = nullptr;
  userTrackingAction = nullptr;
  userDetector = nullptr;
  userPrimaryGeneratorAction = nullptr;

  return;
}

