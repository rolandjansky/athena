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

#include <mutex>

static std::mutex _workerInitMutex;

G4AtlasWorkerRunManager::G4AtlasWorkerRunManager()
  : G4WorkerRunManager(),
    m_msg("G4AtlasWorkerRunManager"),
    // TODO: what if we need to make these configurable?
    m_detGeoSvc("DetectorGeometrySvc", "G4AtlasWorkerRunManager"),
    m_senDetTool("SensitiveDetectorMasterTool"),
    m_fastSimTool("FastSimulationMasterTool"),
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
  // Locking this initialization to protect currently thread-unsafe services
  std::lock_guard<std::mutex> lock(_workerInitMutex);

  const std::string methodName = "G4AtlasWorkerRunManager::Initialize";

  // Setup the user actions for current worker thread.
  if(m_userActionSvc.retrieve().isFailure()) {
    throw GaudiException("Could not retrieve UserActionSvc for worker thread",
                         methodName, StatusCode::FAILURE);
  }
  if(m_userActionSvc->initializeActions().isFailure()) {
    throw GaudiException("Failed to initialize actions for worker thread",
                         methodName, StatusCode::FAILURE);
  }

  // Setup geometry and physics via the base class
  G4RunManager::Initialize();

  // Does some extra setup that we need.
  ConstructScoringWorlds();
  // Run initialization in G4RunManager.
  // Normally done in BeamOn.
  RunInitialization();
}

void G4AtlasWorkerRunManager::InitializeGeometry()
{
  const std::string methodName = "G4AtlasWorkerRunManager::InitializeGeometry";

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
  if(m_senDetTool.retrieve().isFailure()){
    throw GaudiException("Could not retrieve SD master tool",
                         methodName, StatusCode::FAILURE);
  }
  if(m_senDetTool->initializeSDs().isFailure()){
    throw GaudiException("Failed to initialize SDs for worker thread",
                         methodName, StatusCode::FAILURE);
  }

  // Set up the detector magnetic field
  if(m_detGeoSvc.retrieve().isFailure()) {
    throw GaudiException("Could not retrieve det geo svc",
                         methodName, StatusCode::FAILURE);
  }
  if(m_detGeoSvc->initializeFields().isFailure()) {
    throw GaudiException("Failed to initialize mag field for worker thread",
                         methodName, StatusCode::FAILURE);
  }

  // These currently do nothing because we don't override
  userDetector->ConstructSDandField();
  userDetector->ConstructParallelSD();

  // Mark g4 geo as initialized
  geometryInitialized = true;
}

void G4AtlasWorkerRunManager::InitializePhysics()
{
  const std::string methodName = "G4AtlasWorkerRunManager::InitializePhysics";

  // Call the base class
  G4RunManager::InitializePhysics();

  // Setup the fast simulations
  if(m_fastSimTool.retrieve().isFailure()){
    throw GaudiException("Could not retrieve FastSims master tool",
                         methodName, StatusCode::FAILURE);
  }
  if(m_fastSimTool->initializeFastSims().isFailure()){
    throw GaudiException("Failed to initialize FastSims for worker thread",
                         methodName, StatusCode::FAILURE);
  }
}


G4Event* G4AtlasWorkerRunManager::GenerateEvent(G4int iEvent)
{
  static FADS::GeneratorCenter* generatorCenter =
    FADS::GeneratorCenter::GetGeneratorCenter();
  currentEvent = new G4Event(iEvent);
  generatorCenter->GenerateAnEvent(currentEvent);
  return currentEvent;
}


bool G4AtlasWorkerRunManager::SimulateFADSEvent()
{
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);

  // Invoke SDs for any begin-event requirements
  const std::string methodName = "G4AtlasWorkerRunManager::SimulateFADSEvent";
  if (m_senDetTool->BeginOfAthenaEvent().isFailure()) {
    throw GaudiException("Failure in SD BeginOfAthenaEvent",
                         methodName, StatusCode::FAILURE);
  }

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

  // Invoke SDs for end-event requirements, like finalizing hit collections
  if (m_senDetTool->EndOfAthenaEvent().isFailure()) {
    throw GaudiException("Failure in SD EndOfAthenaEvent",
                         methodName, StatusCode::FAILURE);
  }

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
