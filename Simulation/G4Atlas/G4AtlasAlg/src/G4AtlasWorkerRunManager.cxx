/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Hide multi-threading classes from builds without G4MT
#ifdef G4MULTITHREADED

#include "G4AtlasWorkerRunManager.h"

#include "G4StateManager.hh"
#include "G4Event.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4MTRunManager.hh"
#include "G4TransportationManager.hh"
#include "G4VUserDetectorConstruction.hh"

#include "FadsKinematics/GeneratorCenter.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/GaudiException.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
//#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"

#include <mutex>

static std::mutex _workerActionMutex;

G4AtlasWorkerRunManager::G4AtlasWorkerRunManager()
  : G4WorkerRunManager(),
    m_msg("G4AtlasWorkerRunManager"),
    // TODO: what if we need to make these configurable?
    m_detGeoSvc("DetectorGeometrySvc", "G4AtlasWorkerRunManager"),
    m_senDetTool("SensitiveDetectorMasterTool"),
    //m_fastSimTool("FastSimulationMasterTool"),
    m_userActionSvc("G4UA::UserActionSvc", "G4AtlasWorkerRunManager")
{}


G4AtlasWorkerRunManager* G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager()
{
  // Grab thread-local pointer from base class
  auto wrm = G4RunManager::GetRunManager();
  if(wrm) return static_cast<G4AtlasWorkerRunManager*>(wrm);
  else return new G4AtlasWorkerRunManager;
}


void G4AtlasWorkerRunManager::Initialize()
{
  ATH_MSG_INFO("Initialize");

  // Setup the user actions for current worker thread.
  ATH_MSG_INFO("Initializing user actions for worker thread");
  if(m_userActionSvc.retrieve().isFailure()) {
    throw GaudiException("Could not retrieve UserActionSvc for worker thread",
                         "CouldNotRetrieveUASvc", StatusCode::FAILURE);
  }
  if(m_userActionSvc->initializeActions().isFailure()) {
    throw GaudiException("Failed to initialize actions for worker thread",
                         "WorkerThreadInitError", StatusCode::FAILURE);
  }

  // Setup geometry and physics via the base class
  G4RunManager::Initialize();
  // Does some extra setup that we need.
  ConstructScoringWorlds();
  // Run initialization in G4RunManager.
  // Normally done in BeamOn.
  RunInitialization();
  ATH_MSG_INFO("Done initializing");
}

void G4AtlasWorkerRunManager::InitializeGeometry()
{
  ATH_MSG_INFO("InitializeGeometry");

  // I don't think this does anything
  if(fGeometryHasBeenDestroyed) {
    G4TransportationManager::GetTransportationManager()->ClearParallelWorlds();
  }

  // Get the world volume and give it to the kernel
  G4RunManagerKernel* masterKernel = G4MTRunManager::GetMasterRunManagerKernel();
  G4VPhysicalVolume* worldVol = masterKernel->GetCurrentWorld();
  kernel->WorkerDefineWorldVolume(worldVol, false);
  // We don't currently use parallel worlds in ATLAS, but someday we might
  kernel->SetNumberOfParallelWorld(masterKernel->GetNumberOfParallelWorld());

  // Setup the sensitive detectors on each worker.
  ATH_MSG_INFO("Initializing the SDs for this thread");
  if(m_senDetTool.retrieve().isFailure()){
    throw GaudiException("Could not retrieve SD master tool",
                         "CouldNotRetrieveSDMaster", StatusCode::FAILURE);
  }
  if(m_senDetTool->initializeSDs().isFailure()){
    throw GaudiException("Failed to initialize SDs for worker thread",
                         "WorkerThreadInitError", StatusCode::FAILURE);
  }

  // Set up the detector magnetic field
  if(m_detGeoSvc.retrieve().isFailure()) {
    throw GaudiException("Could not retrieve det geo svc",
                         "CouldNotRetrieveDetGeoSvc", StatusCode::FAILURE);
  }
  if(m_detGeoSvc->initializeFields().isFailure()) {
    throw GaudiException("Failed to initialize mag field for worker thread",
                         "WorkerThreadInitError", StatusCode::FAILURE);
  }

  // These currently do nothing because we don't override
  userDetector->ConstructSDandField();
  userDetector->ConstructParallelSD();

  // Mark g4 geo as initialized
  geometryInitialized = true;
}


G4Event* G4AtlasWorkerRunManager::GenerateEvent(G4int iEvent)
{
  ATH_MSG_DEBUG("GenerateEvent");
  static FADS::GeneratorCenter* generatorCenter =
    FADS::GeneratorCenter::GetGeneratorCenter();
  currentEvent = new G4Event(iEvent);
  generatorCenter->GenerateAnEvent(currentEvent);
  return currentEvent;
}


bool G4AtlasWorkerRunManager::SimulateFADSEvent()
{
  ATH_MSG_INFO("SimulateFADSEvent");
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);

  GenerateEvent(1);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4AtlasWorkerRunManager::SimulateFADSEvent: " <<
                     "Event Aborted at Generator level" );
    currentEvent=0;
    return true;
  }

  //eventManager->SetVerboseLevel(3);
  //eventManager->GetTrackingManager()->SetVerboseLevel(3);
  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4AtlasWorkerRunManager::SimulateFADSEvent: " <<
                     "Event Aborted at Detector Simulation level" );
    currentEvent=0;
    return true;
  }

  AnalyzeEvent(currentEvent);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4AtlasWorkerRunManager::SimulateFADSEvent: " <<
                     "Event Aborted at Analysis level" );
    currentEvent=0;
    return true;
  }

  // Register all of the collections if there are any new-style SDs
  // TODO: re-enable this
  //if (m_senDetTool) (*m_senDetTool)->EndOfAthenaEvent();
  //if (m_senDetSvc) m_senDetSvc->EndOfAthenaEvent();

  StackPreviousEvent(currentEvent);
  bool abort = currentEvent->IsAborted();
  currentEvent = 0;
  return abort;
}


void G4AtlasWorkerRunManager::RunTermination()
{
  // Not sure what I should put here...
  // Maybe I can just use the base class?
  G4WorkerRunManager::RunTermination();
}

#endif // G4MULTITHREADED
