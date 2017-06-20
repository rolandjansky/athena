/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasRunManager.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "FadsKinematics/GeneratorCenter.h" // FIXME: to remove

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "G4Event.hh"
#include "G4GeometryManager.hh"
#include "G4ios.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParallelWorldScoringProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4ProductionCutsTable.hh"
#include "G4RegionStore.hh"
#include "G4Run.hh"
#include "G4ScoringManager.hh"
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4Timer.hh"
#include "G4UImanager.hh"
#include "G4UserRunAction.hh"
#include "G4Version.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

#include <string>
#include <vector>



G4AtlasRunManager::G4AtlasRunManager()
  : G4RunManager()
  , m_msg("G4AtlasRunManager")
  , m_releaseGeo(false)
  , m_recordFlux(false)
  , m_senDetTool("SensitiveDetectorMasterTool")
  , m_fastSimTool("FastSimulationMasterTool")
  , m_physListTool("PhysicsListToolBase")
  , m_userActionSvc("G4UA::UserActionSvc", "G4AtlasRunManager")
  , m_detGeoSvc("DetectorGeometrySvc", "G4AtlasRunManager")
{  }


G4AtlasRunManager* G4AtlasRunManager::GetG4AtlasRunManager()
{
  static G4AtlasRunManager* thisManager = nullptr;
  if (!thisManager)
    {
      thisManager=new G4AtlasRunManager; // Leaked
    }
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
  // Call the base class
  G4RunManager::Initialize();
}


void G4AtlasRunManager::InitializeGeometry()
{
  if (m_detGeoSvc.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the DetectorGeometrySvc" );
      G4ExceptionDescription description;
      description << "InitializeGeometry: Failed to retrieve IDetectorGeometrySvc.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveDetGeoSvc", FatalException, description);
      abort(); // to keep Coverity happy
    }

  G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
  for (unsigned int i=0;i<lvs->size();++i)
    {
      if ( (*lvs)[i]->GetName() == "Muon::MuonSys" )
        {
          (*lvs)[i]->SetSmartless( 0.1 );
          ATH_MSG_INFO( "Set smartlessness for Muon::MuonSys to 0.1" );
        }
      else if ( (*lvs)[i]->GetName() == "LArMgr::LAr::EMB::STAC")
        {
          (*lvs)[i]->SetSmartless( 0.5 );
          ATH_MSG_INFO( "Set smartlessness for LArMgr::LAr::EMB::STAC to 0.5" );
        }
    }

  // Create/assign detector construction
  G4RunManager::SetUserInitialization(m_detGeoSvc->GetDetectorConstruction());

  if (userDetector)
    {
      G4RunManager::InitializeGeometry();
    }
  else
    {
      ATH_MSG_WARNING( " User Detector not set!!! Geometry NOT initialized!!!" );
    }

  // Geometry has been initialized.  Now get services to add some stuff to the geometry.
  if (m_senDetTool.retrieve().isFailure()) //svcLocator->service("SensitiveDetector",m_senDetSvc).isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the SD master tool" );
      G4ExceptionDescription description;
      description << "InitializeGeometry: Failed to retrieve ISensitiveDetectorMasterTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveSDMaster", FatalException, description);
      abort(); // to keep Coverity happy
    }
  if(m_senDetTool->initializeSDs().isFailure())
    {
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
  kernel->InitializePhysics();
  physicsInitialized = true;

  // Grab the physics list tool and set the extra options
  if (m_physListTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the physics list tool" );
      G4ExceptionDescription description;
      description << "InitializePhysics: Failed to retrieve IPhysicsListTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrievePLTool", FatalException, description);
      abort(); // to keep Coverity happy
    }
  m_physListTool->SetPhysicsOptions();

  // Fast simulations last
  if (m_fastSimTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the FastSim master tool" );
      G4ExceptionDescription description;
      description << "InitializePhysics: Failed to retrieve IFastSimulationMasterTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveFastSimMaster", FatalException, description);
      abort(); // to keep Coverity happy
    }
  if(m_fastSimTool->initializeFastSims().isFailure())
    {
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

void G4AtlasRunManager::InitializeFluxRecording() {
  // @TODO move this block into a separate function.
  G4UImanager *ui = G4UImanager::GetUIpointer();
  ui->ApplyCommand("/run/setCutForAGivenParticle proton 0 mm");

  G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();

  if(!ScM) return;

  ui->ApplyCommand("/score/create/cylinderMesh cylMesh_1");
  //                        R  Z(-24 to 24)
  ui->ApplyCommand("/score/mesh/cylinderSize 12. 24. m");
  //                iR iZ
  //ui->ApplyCommand("/score/mesh/nBin 1 1 1");
  ui->ApplyCommand("/score/mesh/nBin 120 480 1");

  ui->ApplyCommand("/score/quantity/energyDeposit eDep");

  ui->ApplyCommand("/score/quantity/cellFlux CF_photon");
  ui->ApplyCommand("/score/filter/particle photonFilter gamma");
  // above 2 line crete tally for cell flux for gamma

  ui->ApplyCommand("/score/quantity/cellFlux CF_neutron");
  ui->ApplyCommand("/score/filter/particle neutronFilter neutron");

  ui->ApplyCommand("/score/quantity/cellFlux CF_HEneutron");
  ui->ApplyCommand("/score/filter/particleWithKineticEnergy HEneutronFilter 20 7000000 MeV neutron");

  ui->ApplyCommand("/score/quantity/doseDeposit dose");

  ui->ApplyCommand("/score/close");
  ui->ApplyCommand("/score/list");

  G4int nPar = ScM->GetNumberOfMesh();

  if(nPar<1) return;

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


G4Event* G4AtlasRunManager::GenerateEvent(G4int i_event)
{
  static FADS::GeneratorCenter* generatorCenter=FADS::GeneratorCenter::GetGeneratorCenter();

  SetCurrentG4Event(i_event);

  generatorCenter->GenerateAnEvent(currentEvent);

  return currentEvent;
}


bool G4AtlasRunManager::SimulateFADSEvent()
{

  // std::cout<<" SimulateFADSEvent : start simulating one event "<<std::endl;

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);
  // stateManager->SetNewState(G4State_EventProc);

  // Release GeoModel Geometry if necessary
  if (m_releaseGeo)
    {
      ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
      StoreGateSvc* detStore;
      if (svcLocator->service("DetectorStore",detStore).isFailure())
        {
          ATH_MSG_ERROR( "G4AtlasRunManager could not access the detector store - PANIC!!!!" );
          G4ExceptionDescription description;
          description << "SimulateFADSEvent: Attempt to access DetectorStore failed.";
          G4Exception("G4AtlasRunManager", "CouldNotAccessDetStore", FatalException, description);
          abort(); // to keep Coverity happy
        }

      IGeoModelSvc* geoModel = nullptr;
      if(svcLocator->service("GeoModelSvc",geoModel).isFailure())
        {
          ATH_MSG_WARNING( " ----> Unable to retrieve GeoModelSvc" );
        }
      else
        {
          if(geoModel->clear().isFailure())
            {
              ATH_MSG_WARNING( " ----> GeoModelSvc::clear() failed" );
            }
          else
            {
              ATH_MSG_INFO( " ----> GeoModelSvc::clear() succeeded " );
            }
        }
      m_releaseGeo=false; // Don't do that again...
    }

  if (m_senDetTool)
    {
      if(m_senDetTool->BeginOfAthenaEvent().isFailure())
        {
          G4ExceptionDescription description;
          description << "SimulateFADSEvent: Call to ISensitiveDetectorMasterTool::BeginOfAthenaEvent failed.";
          G4Exception("G4AtlasRunManager", "SDMasterBoAthEvtFailed", FatalException, description);
          abort(); // to keep Coverity happy
        }
    }

  currentEvent = GenerateEvent(1);
  if (currentEvent->IsAborted())
    {
      ATH_MSG_WARNING( "G4AtlasRunManager::SimulateFADSEvent: Event Aborted at Generator level" );
      currentEvent = nullptr;
      return true;
    }

  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      ATH_MSG_WARNING( "G4AtlasRunManager::SimulateFADSEvent: Event Aborted at Detector Simulation level" );
      currentEvent = nullptr;
      return true;
    }

  AnalyzeEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      ATH_MSG_WARNING( "G4AtlasRunManager::SimulateFADSEvent: Event Aborted at Analysis level" );
      currentEvent = nullptr;
      return true;
    }

  if (m_recordFlux) {
    this->RecordFlux();
  }

  //      stateManager->SetNewState(G4State_GeomClosed);
  // Register all of the collections if there are any new-style SDs
  if (m_senDetTool)
    {
      if(m_senDetTool->EndOfAthenaEvent().isFailure())
        {
          G4ExceptionDescription description;
          description << "SimulateFADSEvent: Call to ISensitiveDetectorMasterTool::EndOfAthenaEvent failed.";
          G4Exception("G4AtlasRunManager", "SDMasterEoAthEvtFailed", FatalException, description);
          abort(); // to keep Coverity happy
        }
    }
  if (m_fastSimTool)
    {
      if(m_fastSimTool->EndOfAthenaEvent().isFailure())
        {
          G4ExceptionDescription description;
          description << "SimulateFADSEvent: Call to IFastSimulationMasterTool::EndOfAthenaEvent failed.";
          G4Exception("G4AtlasRunManager", "FSMasterEoAthEvtFailed", FatalException, description);
          abort(); // to keep Coverity happy
        }
    }
  StackPreviousEvent(currentEvent);
  bool abort=currentEvent->IsAborted();
  currentEvent = nullptr;
  // std::cout<<" SimulateFADSEvent : done simulating one event "<<std::endl;
  return abort;
}

void G4AtlasRunManager::RecordFlux() {
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

void  G4AtlasRunManager::RunTermination()
{
  if (m_recordFlux) {
    this->WriteFluxInformation();
  }
  // std::cout<<" this is G4AtlasRunManager::RunTermination() "<<std::endl;
#if G4VERSION_NUMBER < 1010
  for(size_t itr=0;itr<previousEvents->size();itr++)
    {
      delete (*previousEvents)[itr];
    }
#else
  this->CleanUpPreviousEvents();
#endif
  previousEvents->clear();

  if(userRunAction)
    {
      userRunAction->EndOfRunAction(currentRun);
    }

  delete currentRun;
  currentRun = nullptr;
  runIDCounter++;

  ATH_MSG_INFO( "Changing the state..." );
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);

  ATH_MSG_INFO( "Opening the geometry back up" );
  G4GeometryManager::GetInstance()->OpenGeometry();

  ATH_MSG_INFO( "Terminating the run...  State is " << stateManager->GetStateString( stateManager->GetCurrentState() ) );
  kernel->RunTermination();
  ATH_MSG_INFO( "All done..." );

  // std::cout<<" setting all pointers in G4AtlasRunManager to nullptr"<<std::endl;
  userRunAction=nullptr;
  userEventAction=nullptr;
  userSteppingAction=nullptr;
  userStackingAction=nullptr;
  userTrackingAction=nullptr;
  // physicsList=nullptr;
  userDetector=nullptr;
  userPrimaryGeneratorAction=nullptr;

  // std::cout<<" this is G4AtlasRunManager::RunTermination(): done "<<std::endl;
}

void G4AtlasRunManager::WriteFluxInformation() {
  G4UImanager *ui=G4UImanager::GetUIpointer();
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 eDep edep.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_neutron neutron.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_HEneutron HEneutron.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 CF_photon photon.txt");
  ui->ApplyCommand("/score/dumpQuantityToFile cylMesh_1 dose dose.txt");
  return;
}

void G4AtlasRunManager::SetCurrentG4Event(int iEvent)
{
  currentEvent=new G4Event(iEvent);
}
