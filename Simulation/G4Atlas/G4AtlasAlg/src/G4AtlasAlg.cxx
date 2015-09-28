/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4AtlasAlg/G4AtlasAlg.h"
#include "G4AtlasAlg/PreEventActionManager.h"

#ifdef ATHENAHIVE
#include "G4AtlasAlg/G4AtlasMTRunManager.h"
#include "G4AtlasAlg/G4AtlasWorkerRunManager.h"
#include "G4AtlasAlg/G4AtlasUserWorkerThreadInitialization.h"
#else
#include "G4AtlasAlg/G4AtlasRunManager.h"
#endif

// Framework includes
#include "GaudiKernel/AlgFactory.h"

// FADS includes
#include "FadsKinematics/GeneratorCenter.h"
#include "FadsPhysics/PhysicsListCatalog.h"
#include "FadsPhysics/PhysicsListSteering.h"
#include "FadsGeometry/FadsDetectorConstruction.h"

// Geant4 includes
#include "G4TransportationManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4TrackingManager.hh"
#include "G4StackManager.hh"
#include "G4UImanager.hh"
#include "G4ScoringManager.hh"

// EDM includes
#include "EventInfo/EventInfo.h"

// call_once mutexes
#include <mutex>
static std::once_flag initializeOnceFlag;
static std::once_flag finalizeOnceFlag;

/////////////////////////////////////////////////////////////////////////////


G4AtlasAlg::G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_rndmGenSvc("AtDSFMTGenSvc", name),
    m_UASvc("UserActionSvc", name),              // current user action design
    m_userActionSvc("G4UA::UserActionSvc", name) // new user action design
{
  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg created  ++++++++++++" << std::endl << std::endl);
  declareProperty( "Dll", libList="");
  declareProperty( "Physics", physList="");
  declareProperty( "Generator", generator="");
  declareProperty( "FieldMap", fieldMap="");
  declareProperty( "RandomGenerator", rndmGen="athena");
  declareProperty( "ReleaseGeoModel", m_releaseGeoModel=true);
  declareProperty( "RecordFlux", m_recordFlux=false);
  declareProperty( "IncludeParentsInG4Event", m_IncludeParentsInG4Event=false);
  declareProperty( "KillAbortedEvents", m_killAbortedEvents=true);
  declareProperty( "FlagAbortedEvents", m_flagAbortedEvents=false);

  // Service instantiation
  declareProperty("AtRndmGenSvc", m_rndmGenSvc);

  // Verbosities
  m_verbosities.clear();
  declareProperty("Verbosities", m_verbosities);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::initialize()
{
  // Create the scoring manager if requested
  if (m_recordFlux) G4ScoringManager::GetScoringManager();

  // One-time initialization
  try {
    std::call_once(initializeOnceFlag, &G4AtlasAlg::initializeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in G4AtlasAlg::initializeOnce: " << e.what());
    return StatusCode::FAILURE;
  }

#ifdef ATHENAHIVE
  // Current UA design doesn't work in hive
#else
  // Retrieve the user actions
  ATH_CHECK( m_UASvc.retrieve() );
#endif

  // New UA design should work in both worlds
  ATH_CHECK( m_userActionSvc.retrieve() );

#ifdef ATHENAHIVE
  // Retrieve the python service to trigger its initialization. This is done
  // here just to make sure things are initialized in the proper order.
  // TODO: Check if this is still needed.
  ServiceHandle<IService> pyG4Svc("PyAthena::Svc/PyG4AtlasSvc", name());
  ATH_CHECK( pyG4Svc.retrieve() );
#endif

  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg initialized  ++++++++++++" << std::endl << std::endl);

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void G4AtlasAlg::initializeOnce()
{
  // Create the (master) run manager
#ifdef ATHENAHIVE
  auto runMgr = G4AtlasMTRunManager::GetG4AtlasMTRunManager();
#else
  auto runMgr = G4AtlasRunManager::GetG4AtlasRunManager();
  runMgr->SetReleaseGeo( m_releaseGeoModel );
  runMgr->SetLogLevel( int(msg().level()) ); // Synch log levels
#endif

  // Assign physics list
  G4VUserPhysicsList* physicsList =
    FADS::PhysicsListCatalog::GetInstance()->GetMainPhysicsList();
  //runMgr->SetPhysicsList(physicsList); // Is this needed?
  runMgr->SetUserInitialization(physicsList);

  // Create/assign detector construction
  runMgr->SetUserInitialization(new FADS::FadsDetectorConstruction);

#ifdef ATHENAHIVE
  // Worker Thread initialization used to create worker run manager on demand
  runMgr->SetUserInitialization(new G4AtlasUserWorkerThreadInitialization);
#endif

  // G4 user interface commands
  G4UImanager *ui = G4UImanager::GetUIpointer();

  // Load custom libraries
  if (!libList.empty()) {
    ATH_MSG_INFO("G4AtlasAlg specific libraries requested ") ;
    std::string temp="/load "+libList;
    ui->ApplyCommand(temp);
  }
  // Load custom physics
  if (!physList.empty()) {
    ATH_MSG_INFO("requesting a specific physics list "<< physList) ;
    std::string temp="/Physics/GetPhysicsList "+physList;
    ui->ApplyCommand(temp);
  }
  // Setup generator
  FADS::GeneratorCenter* gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->SetIncludeParentsInG4Event( m_IncludeParentsInG4Event );
  if (!generator.empty()) {
    ATH_MSG_INFO("requesting a specific generator "<< generator) ;
    gc->SelectGenerator(generator);
  } else {
    // make sure that there is a default generator (i.e. HepMC interface)
    gc->SelectGenerator("AthenaHepMCInterface");
  }
  // Load custom magnetic field
  if (!fieldMap.empty()) {
    ATH_MSG_INFO("requesting a specific field map "<< fieldMap) ;
    ATH_MSG_INFO("the field is initialized straight away") ;
    std::string temp="/MagneticField/Select "+fieldMap;
    ui->ApplyCommand(temp);
    ui->ApplyCommand("/MagneticField/Initialize");
  }

  ui->ApplyCommand("/geometry/navigator/check_mode true");

  if (rndmGen=="athena" || rndmGen=="ranecu")	{
    // Set the random number generator to AtRndmGen
    if (m_rndmGenSvc.retrieve().isFailure()) {
      // We can only retrieve void from here. Let's assume that eventually
      // all this initialization code will be moved elsewhere (like a svc) for
      // better control. Then, for now, let's just throw.
      throw std::runtime_error("Could not initialize ATLAS Random Generator Service");
      //return StatusCode::FAILURE;
    }
    CLHEP::HepRandomEngine* engine = m_rndmGenSvc->GetEngine("AtlasG4");
    CLHEP::HepRandom::setTheEngine(engine);
    ATH_MSG_INFO("Random nr. generator is set to Athena");
  }
  else if (rndmGen=="geant4" || rndmGen.empty()) {
    ATH_MSG_INFO("Random nr. generator is set to Geant4");
  }

  // G4 init moved to PyG4AtlasAlg / G4AtlasEngine
  /// @todo Reinstate or delete?! This can't actually be called from the Py algs
  //ATH_MSG_INFO("Firing initialization of G4!!!");
  //initializeG4();
}

void G4AtlasAlg::initializeG4()
{

  if (m_verbosities.size()>0){
    G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    G4EventManager *em = G4EventManager::GetEventManager();

    auto itr = m_verbosities.end();
    if ((itr = m_verbosities.find("Navigator")) != m_verbosities.end()) {
      tm->GetNavigatorForTracking()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Propagator")) != m_verbosities.end()) {
      tm->GetPropagatorInField()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Tracking")) != m_verbosities.end()) {
      rmk->GetTrackingManager()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Stepping")) != m_verbosities.end()) {
      rmk->GetTrackingManager()->GetSteppingManager()->
        SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Stacking")) != m_verbosities.end()) {
      rmk->GetStackManager()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Event")) != m_verbosities.end()) {
      em->SetVerboseLevel( atof(itr->second.data()) );
    }
  } // End of the setting of verbosities

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::finalize() {
  ATH_MSG_DEBUG(std::endl<<std::endl<<std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg finalized  ++++++++++++" <<std::endl<<std::endl);

  // One time finalization
  try {
    std::call_once(finalizeOnceFlag, &G4AtlasAlg::finalizeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in G4AtlasAlg::finalizeOnce: " << e.what());
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void G4AtlasAlg::finalizeOnce() {
  ATH_MSG_DEBUG("\t terminating the current G4 run");
  // TODO: could probably just use G4RunManager base class generically.
#ifdef ATHENAHIVE
  auto runMgr = G4AtlasMTRunManager::GetG4AtlasMTRunManager();
#else
  auto runMgr = G4AtlasRunManager::GetG4AtlasRunManager();
#endif
  runMgr->RunTermination();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::execute()
{
  static int n_Event=0;
  static PreEventActionManager *preEvent=PreEventActionManager::
    GetPreEventActionManager();
  ATH_MSG_DEBUG(std::endl<<std::endl<<std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg execute  ++++++++++++" <<std::endl<<std::endl);

  n_Event += 1;

  if (n_Event<=10 || (n_Event%100) == 0) {
    ATH_MSG_ALWAYS("G4AtlasAlg: Event num. "  << n_Event << " start processing");
  }

  // TODO: is this thread safe??
  preEvent->Execute();

  ATH_MSG_DEBUG("Calling SimulateG4Event");

  // Worker run manager
  // Custom class has custom method call: SimulateFADSEvent.
  // So, grab custom singleton class directly, rather than base.
  // Maybe that should be changed! Then we can use a base pointer.
#ifdef ATHENAHIVE
  auto workerRM = G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager();
#else
  auto workerRM = G4AtlasRunManager::GetG4AtlasRunManager();
#endif
  bool abort = workerRM->SimulateFADSEvent();

  if (abort) {
    ATH_MSG_WARNING("Event was aborted !! ");
    ATH_MSG_WARNING("Simulation will now go on to the next event ");
    if (m_killAbortedEvents){
      ATH_MSG_WARNING("setFilterPassed is now False");
      setFilterPassed(false);
    }
    if (m_flagAbortedEvents){
      // TODO: update to VarHandle
      const DataHandle<EventInfo> eic = 0;
      if ( sgSvc()->retrieve( eic ).isFailure() || !eic ){
	ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
      } else {
	// Gotta cast away the const... sadface
	EventInfo *ei = const_cast< EventInfo * > (&(*eic));
	ei->setErrorState(EventInfo::Core,EventInfo::Error);
	ATH_MSG_WARNING( "Set error state in event info!" );
      }
    }
  }

  return StatusCode::SUCCESS;
}
