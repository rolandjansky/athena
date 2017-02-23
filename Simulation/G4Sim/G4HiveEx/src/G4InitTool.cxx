#include "unistd.h"
#include "sys/syscall.h"

#include "G4InitTool.h"

#include "G4WorkerRunManager.hh"

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
  auto wThreadContext = new G4WorkerThread;
  G4AutoDelete::Register(wThreadContext);

  // Assign the thread id
  // TODO: implement a better solution.
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
  auto masterRM = G4MTRunManager::GetMasterRunManager();
  // Worker thread initialization
  auto workerInitializer = masterRM->GetUserWorkerThreadInitialization();

  const CLHEP::HepRandomEngine* masterEngine = masterRM->getMasterRandomEngine();
  workerInitializer->SetupRNGEngine(masterEngine);

  // Create the thread-local worker run manager (G4AtlasWorkerRunManager)
  ATH_MSG_INFO("Creating worker RM");
  auto wrm = workerInitializer->CreateWorkerRunManager();
  wrm->SetWorkerThread(wThreadContext);

  // Share detector from master with worker.
  ATH_MSG_INFO("Assigning detector construction");
  const auto detector = masterRM->GetUserDetectorConstruction();
  wrm->G4RunManager::SetUserInitialization
    (const_cast<G4VUserDetectorConstruction*>(detector));
  // Share physics list from master with worker.
  const auto physicslist = masterRM->GetUserPhysicsList();
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

  ATH_MSG_INFO ("==> tbb thread end of initThread with id: 0x" << std::hex <<
                pthread_self() << std::dec );

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void
G4InitTool::terminateThread() {

  ATH_MSG_INFO ("terminateThread ==> tbb thread 0x"
                << std::hex << pthread_self() << std::dec );

   // call the run termination method on the G4 side
  G4RunManager::GetRunManager()->RunTermination();

  m_nInitThreads--;

}
