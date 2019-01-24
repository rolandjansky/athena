/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasRunManager.h"

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
  , m_physListTool("PhysicsListToolBase")
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
  // Call the base class Initialize method. This will call
  // InitializeGeometry and InitializePhysics.
  G4RunManager::Initialize();
  ATH_MSG_INFO("G4 Command: Trying at the end of Initialize()");
  // Try to execute G4Commands (Should be in G4State_Idle at this point.)
  AttemptG4Commands(m_g4commands);
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
  const G4String muonSys("Muon::MuonSys");
  const G4String embSTAC("LArMgr::LAr::EMB::STAC");
  for (auto* ilv : *logicalVolumeStore ) {
    if ( ilv->GetName() == muonSys ) {
      ilv->SetSmartless( 0.1 );
      ATH_MSG_INFO( "Set smartlessness for Muon::MuonSys to 0.1" );
    }
    else if ( ilv->GetName() == embSTAC ) {
      ilv->SetSmartless( 0.5 );
      ATH_MSG_INFO( "Set smartlessness for LArMgr::LAr::EMB::STAC to 0.5" );
    }
  }

  // Create/assign detector construction
  G4RunManager::SetUserInitialization(m_detGeoSvc->GetDetectorConstruction());
  if (userDetector) {
    G4RunManager::InitializeGeometry();
  }
  else {
    ATH_MSG_WARNING( " User Detector not set!!! Geometry NOT initialized!!!" );
  }
  ATH_MSG_INFO("G4 Command: Trying at the end of InitializeGeometry()");
  // Try to execute G4Commands (Should be in G4State_PreInit at this point.)
  AttemptG4Commands(m_g4commands);

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
  if (m_physListTool.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve the physics list tool" );
    G4ExceptionDescription description;
    description << "InitializePhysics: Failed to retrieve IPhysicsListTool.";
    G4Exception("G4AtlasRunManager", "CouldNotRetrievePLTool", FatalException, description);
    abort(); // to keep Coverity happy
  }
  m_physListTool->SetPhysicsOptions();

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
    this->InitializeFluxRecording();
  }

  return;
}

void G4AtlasRunManager::InitializeFluxRecording()
{
  std::vector<std::string> initFluxCommands;
  initFluxCommands.push_back("/run/setCutForAGivenParticle proton 0 mm");
  AttemptG4Commands(initFluxCommands);
  if(initFluxCommands.size()) {
    ATH_MSG_WARNING("The following commands failed to be executed via the G4UImanager:");
    for (auto g4command : initFluxCommands) {
      ATH_MSG_WARNING(g4command);
    }
  }
  initFluxCommands.clear();
  G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();

  if(!ScM) { return; }

  initFluxCommands.push_back("/score/create/cylinderMesh cylMesh_1");
  //                        R  Z(-24 to 24)
  initFluxCommands.push_back("/score/mesh/cylinderSize 12. 24. m");
  //                iR iZ
  //initFluxCommands.push_back("/score/mesh/nBin 1 1 1");
  initFluxCommands.push_back("/score/mesh/nBin 120 480 1");

  initFluxCommands.push_back("/score/quantity/energyDeposit eDep");

  initFluxCommands.push_back("/score/quantity/cellFlux CF_photon");
  initFluxCommands.push_back("/score/filter/particle photonFilter gamma");
  // above 2 line crete tally for cell flux for gamma

  initFluxCommands.push_back("/score/quantity/cellFlux CF_neutron");
  initFluxCommands.push_back("/score/filter/particle neutronFilter neutron");

  initFluxCommands.push_back("/score/quantity/cellFlux CF_HEneutron");
  initFluxCommands.push_back("/score/filter/particleWithKineticEnergy HEneutronFilter 20 7000000 MeV neutron");

  initFluxCommands.push_back("/score/quantity/doseDeposit dose");

  initFluxCommands.push_back("/score/close");
  initFluxCommands.push_back("/score/list");
  AttemptG4Commands(initFluxCommands);
  if(initFluxCommands.size()) {
    ATH_MSG_WARNING("The following commands failed to be executed via the G4UImanager:");
    for (auto g4command : initFluxCommands) {
      ATH_MSG_WARNING(g4command);
    }
  }

  G4int nPar = ScM->GetNumberOfMesh();

  if(nPar<1) { return; }

  G4ParticleTable::G4PTblDicIterator* particleIterator
    = G4ParticleTable::GetParticleTable()->GetIterator();

  for(G4int iw=0;iw<nPar;iw++) {
    G4VScoringMesh* mesh = ScM->GetMesh(iw);
    G4VPhysicalVolume* pWorld
      = G4TransportationManager::GetTransportationManager()
      ->IsWorldExisting(ScM->GetWorldName(iw));
    if(!pWorld) {
      pWorld = G4TransportationManager::GetTransportationManager()
        ->GetParallelWorld(ScM->GetWorldName(iw));
      pWorld->SetName(ScM->GetWorldName(iw));

      G4ParallelWorldScoringProcess* theParallelWorldScoringProcess
        = new G4ParallelWorldScoringProcess(ScM->GetWorldName(iw));
      theParallelWorldScoringProcess->SetParallelWorld(ScM->GetWorldName(iw));

      particleIterator->reset();
      while( (*particleIterator)() ) {
        G4ParticleDefinition* particle = particleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        if(pmanager) {
          pmanager->AddProcess(theParallelWorldScoringProcess);
          pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxAtRest);
          pmanager->SetProcessOrderingToSecond(theParallelWorldScoringProcess, idxAlongStep);
          pmanager->SetProcessOrderingToLast(theParallelWorldScoringProcess, idxPostStep);
        }
      }
    }

    mesh->Construct(pWorld);
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

  this->AnalyzeEvent(currentEvent);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4AtlasRunManager::ProcessEvent: Event Aborted at Analysis level" );
    currentEvent = nullptr;
    return true;
  }

  if (m_recordFlux) { this->RecordFlux(); }

  this->StackPreviousEvent(currentEvent);
  bool abort = currentEvent->IsAborted();
  currentEvent = nullptr;

  return abort;
}

void G4AtlasRunManager::RecordFlux()
{
  G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();
  if(ScM) {
    G4int nPar = ScM->GetNumberOfMesh();
    G4HCofThisEvent* HCE = currentEvent->GetHCofThisEvent();
    if(HCE && nPar>0) {
      G4int nColl = HCE->GetCapacity();
      for(G4int i=0;i<nColl;i++) {
        G4VHitsCollection* HC = HCE->GetHC(i);
        if(HC) { ScM->Accumulate(HC); }
      }
    }
  }
  return;
}

void G4AtlasRunManager::RunTermination()
{
  ATH_MSG_DEBUG( " G4AtlasRunManager::RunTermination() " );
  if (m_recordFlux) {
    this->WriteFluxInformation();
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

  if(m_g4commands.size()) {
    ATH_MSG_WARNING("The following commands failed to be executed via the G4UImanager:");
    for (auto g4command : m_g4commands) {
      ATH_MSG_WARNING(g4command);
    }
  }
  return;
}

void G4AtlasRunManager::WriteFluxInformation()
{
  std::vector<std::string> writeFluxCommands;
  writeFluxCommands.push_back("/score/dumpQuantityToFile cylMesh_1 eDep edep.txt");
  writeFluxCommands.push_back("/score/dumpQuantityToFile cylMesh_1 CF_neutron neutron.txt");
  writeFluxCommands.push_back("/score/dumpQuantityToFile cylMesh_1 CF_HEneutron HEneutron.txt");
  writeFluxCommands.push_back("/score/dumpQuantityToFile cylMesh_1 CF_photon photon.txt");
  writeFluxCommands.push_back("/score/dumpQuantityToFile cylMesh_1 dose dose.txt");
  AttemptG4Commands(writeFluxCommands);
  if(writeFluxCommands.size()) {
    ATH_MSG_WARNING("The following commands failed to be executed via the G4UImanager:");
    for (auto g4command : writeFluxCommands) {
      ATH_MSG_WARNING(g4command);
    }
  }
  return;
}

void G4AtlasRunManager::AddG4Command(const std::string& g4Command)
{
  m_g4commands.push_back(g4Command);
  return;
}

void G4AtlasRunManager::AttemptG4Commands(std::vector<std::string>& commands)
{
  // G4 user interface commands
  G4UImanager *ui = G4UImanager::GetUIpointer();
  std::vector<std::string> commandsToRetry;
  // Send UI commands
  for (auto g4command : commands) {
    const int the_return = ui->ApplyCommand( g4command );
    CommandLog(the_return, g4command);
    // Tried to execute the command at the wrong point in the
    // initialization - try again later
    if(the_return==200) {
      commandsToRetry.push_back(g4command);
    }
  }
  commands.swap(commandsToRetry);
  return;
}

void G4AtlasRunManager::CommandLog(int returnCode, const std::string& commandString) const
{
  switch(returnCode) {
  case 0: { ATH_MSG_INFO("G4 Command: " << commandString << " - Command Succeeded"); } break;
  case 100: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Command Not Found!"); } break;
  case 200: {
    auto* stateManager = G4StateManager::GetStateManager();
    ATH_MSG_DEBUG("G4 Command: " << commandString << " - Illegal Application State (" <<
                    stateManager->GetStateString(stateManager->GetCurrentState()) << ")!");
  } break;
  case 300: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Out of Range!"); } break;
  case 400: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Unreadable!"); } break;
  case 500: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Out of Candidates!"); } break;
  case 600: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Alias Not Found!"); } break;
  default: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Unknown Status!"); } break;
  }

}
