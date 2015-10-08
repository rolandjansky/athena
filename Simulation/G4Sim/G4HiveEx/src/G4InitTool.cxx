/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "unistd.h"
#include "sys/syscall.h"

#include "G4InitTool.h"

#include "G4AtlasAlg/G4AtlasWorkerRunManager.h"

#include "G4GeometryWorkspace.hh"
#include "G4GeometryWorkspacePool.hh"
#include "G4SolidsWorkspace.hh"
#include "G4SolidsWorkspacePool.hh"
#include "G4ParticlesWorkspace.hh"
#include "G4PhysicsListWorkspace.hh"
#include "G4UImanager.hh"

#include "G4MTRunManager.hh"
#include "G4WorkerThread.hh"
#include "G4UserWorkerThreadInitialization.hh"
#include "G4RunManager.hh"
#include "G4VUserActionInitialization.hh"
#include "G4UserWorkerInitialization.hh"
#include "G4AutoDelete.hh"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

G4InitTool::G4InitTool( const std::string& type, const std::string& name,
                        const IInterface* parent )
  : base_class(type, name, parent),
    m_nInitThreads(0)

{
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void
G4InitTool::initThread() {

  ATH_MSG_INFO ("==> tbb thread started with id: 0x" << std::hex
                << pthread_self() << std::dec);

  // The following code is modeled after G4MTRunManagerKernel::StartThread

  // Define the G4 worker thread context
  G4WorkerThread* wThreadContext = new G4WorkerThread;
  G4AutoDelete::Register(wThreadContext);

  // Assign the thread id
#ifdef SYS_gettid
  // WARNING: not portable!
  pid_t tid = syscall(SYS_gettid);
  wThreadContext->SetThreadId( tid );
#else
  #error "SYS_gettid unavailable on this system"
#endif
  //wThreadContext->SetThreadId( pthread_self() );
  G4int thisID = wThreadContext->GetThreadId();
  G4Threading::G4SetThreadId(thisID);

  // Setup up thread-local workspace
  wThreadContext->BuildGeometryAndPhysicsVector();

  // Retrieve the G4AtlasMTRunManager
  ATH_MSG_INFO("Creating worker RM");
  G4MTRunManager* masterRM = G4MTRunManager::GetMasterRunManager();

  const CLHEP::HepRandomEngine* masterEngine = masterRM->getMasterRandomEngine();
  masterRM->GetUserWorkerThreadInitialization()->SetupRNGEngine(masterEngine);

  // Create the thread-local worker run manager
  G4AtlasWorkerRunManager* wrm = dynamic_cast<G4AtlasWorkerRunManager*>
    (masterRM->GetUserWorkerThreadInitialization()->CreateWorkerRunManager());
  wrm->SetWorkerThread(wThreadContext);

  // Share detector from master with worker.
  ATH_MSG_INFO("Assigning detector construction");
  const G4VUserDetectorConstruction* detector =
    masterRM->GetUserDetectorConstruction();
  wrm->G4RunManager::SetUserInitialization
    (const_cast<G4VUserDetectorConstruction*>(detector));
  // Share physics list from master with worker.
  const G4VUserPhysicsList* physicslist = masterRM->GetUserPhysicsList();
  wrm->SetUserInitialization(const_cast<G4VUserPhysicsList*>(physicslist));

  // Build thread-local user actions - NOT CURRENTLY USED.
  //ATH_MSG_INFO("Building user actions");
  if(masterRM->GetUserActionInitialization())
  { masterRM->GetNonConstUserActionInitialization()->Build(); }
  // Start user worker initialization
  if(masterRM->GetUserWorkerInitialization())
  { masterRM->GetUserWorkerInitialization()->WorkerStart(); }

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

  ATH_MSG_INFO ("==> tbb thread end of initThread with id: 0x" << std::hex <<
                pthread_self() << std::dec );

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void
G4InitTool::terminateThread() {

  ATH_MSG_INFO ("terminateThread ==> tbb thread 0x"
                << std::hex << pthread_self() << std::dec );

  m_nInitThreads--;


}
