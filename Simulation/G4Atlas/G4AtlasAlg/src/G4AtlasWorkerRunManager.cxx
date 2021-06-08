/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Hide multi-threading classes from builds without G4MT
#include "G4Types.hh"
#ifdef G4MULTITHREADED

#include "G4AtlasAlg/G4AtlasWorkerRunManager.h"

#include "G4StateManager.hh"
#include "G4Event.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4MTRunManager.hh"
#include "G4TransportationManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4UImanager.hh"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/GaudiException.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <mutex>

static std::mutex workerInitMutex;

G4AtlasWorkerRunManager::G4AtlasWorkerRunManager()
  : G4WorkerRunManager()
  , m_msg("G4AtlasWorkerRunManager")
    // TODO: what if we need to make these configurable?
  , m_detGeoSvc("DetectorGeometrySvc", "G4AtlasWorkerRunManager")
  , m_fastSimTool("FastSimulationMasterTool")
{}


G4AtlasWorkerRunManager* G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager()
{
  // Grab thread-local pointer from base class
  auto* wrm = G4RunManager::GetRunManager();
  if(wrm) { return static_cast<G4AtlasWorkerRunManager*>(wrm); }
  else { return new G4AtlasWorkerRunManager; }
}


void G4AtlasWorkerRunManager::Initialize()
{
  // Locking this initialization to protect currently thread-unsafe services
  std::lock_guard<std::mutex> lock(workerInitMutex);

  // Setup geometry and physics via the base class
  G4RunManager::Initialize();

  /*
  ** The following fragment of code applies all UI commangs from the master command stack.
  ** It has been moved over here from G4InitTool::initThread() because in its previous
  ** location the code had no effect (invalid application state).
  **
  ** Having this code here is OK, but placing it here we are changing some assumptions
  ** implemented in the design of Geant4. This has to do with the handling of multiple
  ** (G4)Runs in the same job. If a second run is executed and no physics or geometry
  ** changes happened between the two runs [Worker]::Initialize() is not called. But UI
  ** commands need to be called anyway, which is not going to happen with our implementation.
  **
  ** If ATLAS ever decides to run multiple G4 runs in the same job, all the MT initialization
  ** will have to be thoroughly reviewed.
  */
  ATH_MSG_DEBUG("G4 Command: Trying at the end of Initialize()");
  const std::string methodName = "G4AtlasWorkerRunManager::Initialize";
  G4MTRunManager* masterRM = G4MTRunManager::GetMasterRunManager();
  std::vector<G4String> cmds = masterRM->GetCommandStack();
  G4UImanager* uimgr = G4UImanager::GetUIpointer();
  for(const auto& it : cmds) {
    int retVal = uimgr->ApplyCommand(it);
    CommandLog(retVal, it);
    if(retVal!=fCommandSucceeded) {
       std::string errMsg{"Failed to apply command <"};
       errMsg += (it + ">. Return value " + std::to_string(retVal));
       throw GaudiException(errMsg,methodName,StatusCode::FAILURE);
    }
  }

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

  // Construct sensitive detectors and magnetic field
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
  if(m_fastSimTool.retrieve().isFailure()) {
    throw GaudiException("Could not retrieve FastSims master tool",
                         methodName, StatusCode::FAILURE);
  }
  if(m_fastSimTool->initializeFastSims().isFailure()) {
    throw GaudiException("Failed to initialize FastSims for worker thread",
                         methodName, StatusCode::FAILURE);
  }
}


bool G4AtlasWorkerRunManager::ProcessEvent(G4Event* event)
{

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);

  currentEvent = event;

  //eventManager->SetVerboseLevel(3);
  //eventManager->GetTrackingManager()->SetVerboseLevel(3);
  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4AtlasWorkerRunManager::SimulateFADSEvent: " <<
                     "Event Aborted at Detector Simulation level" );
    currentEvent = nullptr;
    return true;
  }

  this->AnalyzeEvent(currentEvent);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4AtlasWorkerRunManager::SimulateFADSEvent: " <<
                     "Event Aborted at Analysis level" );
    currentEvent = nullptr;
    return true;
  }

  this->StackPreviousEvent(currentEvent);
  bool abort = currentEvent->IsAborted();
  currentEvent = nullptr;

  return abort;
}


void G4AtlasWorkerRunManager::RunTermination()
{
  // Not sure what I should put here...
  // Maybe I can just use the base class?
  G4WorkerRunManager::RunTermination();
}

void G4AtlasWorkerRunManager::CommandLog(int returnCode, const std::string& commandString) const
{
  switch(returnCode) {
  case 0: { ATH_MSG_DEBUG("G4 Command: " << commandString << " - Command Succeeded"); } break;
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

#endif // G4MULTITHREADED
