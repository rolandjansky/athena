/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Main header include
#include "G4ThreadInitTool.h"

// Geant4 includes
#include "G4WorkerRunManager.hh"
#include "G4UImanager.hh"
#include "G4MTRunManager.hh"
#include "G4WorkerThread.hh"
#include "G4UserWorkerThreadInitialization.hh"
#include "G4RunManager.hh"
#include "G4VUserActionInitialization.hh"
#include "G4UserWorkerInitialization.hh"
#include "G4AutoDelete.hh"

// System includes
#include <unistd.h>
#include <sys/syscall.h>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
G4ThreadInitTool::G4ThreadInitTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
  : base_class(type, name, parent),
    m_nInitThreads(0)
{}

//-----------------------------------------------------------------------------
// Worker thread initialization.
// This code is modeled after G4MTRunManagerKernel::StartThread.
//-----------------------------------------------------------------------------
void G4ThreadInitTool::initThread()
{
  ATH_MSG_INFO("==> tbb thread started with id: 0x" <<
               std::hex << pthread_self() << std::dec);

  // Define the G4 worker thread context and setup its cleanup mechanism.
  auto wThreadContext = new G4WorkerThread;
  G4AutoDelete::Register(wThreadContext);

  // Assign the thread ID
  static std::atomic_uint tid(0);
  wThreadContext->SetThreadId( tid++ );
  G4Threading::G4SetThreadId( wThreadContext->GetThreadId() );

  // Setup thread-local geometry and physics
  wThreadContext->BuildGeometryAndPhysicsVector();

  // Retrieve the master thread run manager
  G4MTRunManager* masterRM = G4MTRunManager::GetMasterRunManager();
  // Worker thread initialization object
  const G4UserWorkerThreadInitialization* workerInitializer =
    masterRM->GetUserWorkerThreadInitialization();

  // Random number setup.
  // TODO: revisit this once MT AthRNGSvc is available.
  const CLHEP::HepRandomEngine* masterEngine = masterRM->getMasterRandomEngine();
  workerInitializer->SetupRNGEngine(masterEngine);

  // Create the thread-local worker run manager (G4AtlasWorkerRunManager)
  ATH_MSG_INFO("Creating worker RM");
  G4WorkerRunManager* wrm = workerInitializer->CreateWorkerRunManager();
  wrm->SetWorkerThread(wThreadContext);

  // Share detector from master with worker.
  ATH_MSG_INFO("Assigning detector construction");
  const G4VUserDetectorConstruction* detector =
    masterRM->GetUserDetectorConstruction();
  // I don't want to const-cast here, but this is what they do in G4's
  // StartThread function, so there is likely no alternative.
  wrm->G4RunManager::SetUserInitialization
    (const_cast<G4VUserDetectorConstruction*>(detector));
  // Share physics list from master with worker.
  const G4VUserPhysicsList* physicslist = masterRM->GetUserPhysicsList();
  wrm->SetUserInitialization(const_cast<G4VUserPhysicsList*>(physicslist));

  // Build thread-local user actions - NOT CURRENTLY USED.
  if(masterRM->GetUserActionInitialization()) {
    masterRM->GetNonConstUserActionInitialization()->Build();
  }

  // Start user worker initialization
  if(masterRM->GetUserWorkerInitialization()) {
    masterRM->GetUserWorkerInitialization()->WorkerStart();
  }

  // Initialize the worker run manager
  ATH_MSG_INFO("Initializing worker RM");
  wrm->Initialize();

  // Copy the UI commands to the worker
  std::vector<G4String> cmds = masterRM->GetCommandStack();
  ATH_MSG_INFO (cmds.size() << " commands in UI stack");
  G4UImanager* uimgr = G4UImanager::GetUIpointer();
  for(const auto& it : cmds) {
    ATH_MSG_INFO ("Adding command to worker: " << it);
    uimgr->ApplyCommand(it);
  }

  // Atomic increment number of initialized threads
  m_nInitThreads++;

  ATH_MSG_INFO("==> tbb thread end of initThread with id: 0x" <<
               std::hex << pthread_self() << std::dec);
}

//-----------------------------------------------------------------------------
// Worker thread termination
//-----------------------------------------------------------------------------
void G4ThreadInitTool::terminateThread()
{
  ATH_MSG_DEBUG("terminateThread ==> tbb thread 0x" <<
               std::hex << pthread_self() << std::dec);

  // Geant4 worker finalization
  auto runMgr = G4RunManager::GetRunManager();
  ATH_MSG_DEBUG("G4RunManager ptr" << runMgr);
  if(runMgr) {
    runMgr->RunTermination();
    ATH_MSG_INFO("terminateThread ==> safely called G4AtlasWorkerRunManager::RunTermination for tbb thread 0x" <<
               std::hex << pthread_self() << std::dec);
    // Atomic decrement number of initialized threads
    m_nInitThreads--;
  }
  else {
    ATH_MSG_WARNING("skipping attempt to call terminateThread for tbb thread 0x" <<
                    std::hex << pthread_self() << std::dec <<
                    " without having first called initThread.");
    // Not decrementing m_nInitThreads as initThread was not called in this case.
  }
}
