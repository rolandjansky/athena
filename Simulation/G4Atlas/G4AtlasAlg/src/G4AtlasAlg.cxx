/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4AtlasAlg.h"

// Can we safely include all of these?
#include "G4AtlasMTRunManager.h"
#include "G4AtlasWorkerRunManager.h"
#include "G4AtlasUserWorkerThreadInitialization.h"
#include "G4AtlasRunManager.h"

// FADS includes
#include "FadsKinematics/GeneratorCenter.h"

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

// CLHEP includes
#include "CLHEP/Random/RandomEngine.h"

// EDM includes
#include "EventInfo/EventInfo.h"

#include "MCTruthBase/TruthStrategyManager.h"

// call_once mutexes
#include <mutex>
static std::once_flag initializeOnceFlag;
static std::once_flag finalizeOnceFlag;

/////////////////////////////////////////////////////////////////////////////


G4AtlasAlg::G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_rndmGenSvc("AtDSFMTGenSvc", name),
    m_userActionSvc("G4UA::UserActionSvc", name), // new user action design
    m_physListTool("PhysicsListToolBase"),
    m_truthRecordSvc("ISF_TruthRecordSvc", name),
    m_geoIDSvc("ISF_GeoIDSvc", name)
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
  declareProperty("UserActionSvc", m_userActionSvc);
  declareProperty("PhysicsListTool", m_physListTool);
  declareProperty("TruthRecordService", m_truthRecordSvc);
  declareProperty("GeoIDSvc", m_geoIDSvc);
  
  // Verbosities
  declareProperty("Verbosities", m_verbosities);

  // Multi-threading specific settings
  declareProperty("MultiThreading", m_useMT=false);

  // Commands to send to the G4UI
  declareProperty("G4Commands", m_g4commands);
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

  ATH_CHECK( m_userActionSvc.retrieve() );
  
  if(m_useMT) {
    // Retrieve the python service to trigger its initialization. This is done
    // here just to make sure things are initialized in the proper order.
    // Hopefully we can drop this at some point.
    ServiceHandle<IService> pyG4Svc("PyAthena::Svc/PyG4AtlasSvc", name());
    ATH_CHECK( pyG4Svc.retrieve() );
  }

  ATH_CHECK( m_truthRecordSvc.retrieve() );
  ATH_MSG_INFO( "- Using ISF TruthRecordSvc : " << m_truthRecordSvc.typeAndName() );
  ATH_CHECK( m_geoIDSvc.retrieve() );
  ATH_MSG_INFO( "- Using ISF GeoIDSvc       : " << m_geoIDSvc.typeAndName() );

  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);

  TruthStrategyManager* sManager = TruthStrategyManager::GetStrategyManager();
  sManager->SetISFTruthSvc( &(*m_truthRecordSvc) );
  sManager->SetISFGeoIDSvc( &(*m_geoIDSvc) );

  ATH_MSG_INFO("++++++++++++  G4AtlasAlg initialized  ++++++++++++" << std::endl << std::endl);
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void G4AtlasAlg::initializeOnce()
{
  // Assign physics list
  if(m_physListTool.retrieve().isFailure())
    {
      throw std::runtime_error("Could not initialize ATLAS PhysicsListTool!");
    }

  // Create the (master) run manager
  if(m_useMT) {
#ifdef G4MULTITHREADED
    auto runMgr = G4AtlasMTRunManager::GetG4AtlasMTRunManager();
    m_physListTool->SetPhysicsList();
    // Worker Thread initialization used to create worker run manager on demand.
    // @TODO use this class to pass any configuration to worker run manager.
    runMgr->SetUserInitialization( new G4AtlasUserWorkerThreadInitialization );
#else
    throw std::runtime_error("Trying to use multi-threading in non-MT build!");
#endif
  }
  // Single-threaded run manager
  else {
    auto runMgr = G4AtlasRunManager::GetG4AtlasRunManager();
    m_physListTool->SetPhysicsList();
    runMgr->SetReleaseGeo( m_releaseGeoModel );
    runMgr->SetRecordFlux( m_recordFlux );
    runMgr->SetLogLevel( int(msg().level()) ); // Synch log levels
    runMgr->SetUserActionSvc( m_userActionSvc.typeAndName() );
  }

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
      // We can only return void from here. Let's assume that eventually
      // all this initialization code will be moved elsewhere (like a svc) for
      // better control. Then, for now, let's just throw.
      throw std::runtime_error("Could not initialize ATLAS Random Generator Service");
    }
    CLHEP::HepRandomEngine* engine = m_rndmGenSvc->GetEngine("AtlasG4");
    CLHEP::HepRandom::setTheEngine(engine);
    ATH_MSG_INFO("Random nr. generator is set to Athena");
  }
  else if (rndmGen=="geant4" || rndmGen.empty()) {
    ATH_MSG_INFO("Random nr. generator is set to Geant4");
  }

  // Send UI commands
  for (auto g4command : m_g4commands){
    ui->ApplyCommand( g4command );
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
  auto runMgr = G4RunManager::GetRunManager();
  runMgr->RunTermination();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::execute()
{
  static int n_Event=0;
  ATH_MSG_DEBUG(std::endl<<std::endl<<std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg execute  ++++++++++++" <<std::endl<<std::endl);


  n_Event += 1;

  if (n_Event<=10 || (n_Event%100) == 0) {
    ATH_MSG_ALWAYS("G4AtlasAlg: Event num. "  << n_Event << " start processing");
  }

  // tell TruthService we're starting a new event
  ATH_CHECK( m_truthRecordSvc->initializeTruthCollection() ); //FIXME POINTLESS - THIS METHOD IS EMPTY IN MASTER

  ATH_MSG_DEBUG("Calling SimulateG4Event");


  // Worker run manager
  // Custom class has custom method call: SimulateFADSEvent.
  // So, grab custom singleton class directly, rather than base.
  // Maybe that should be changed! Then we can use a base pointer.
  bool abort = false;
  if(m_useMT) {
#ifdef G4MULTITHREADED
    auto workerRM = G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager();
    abort = workerRM->SimulateFADSEvent();
#else
    ATH_MSG_ERROR("Trying to use multi-threading in non-MT build!");
    return StatusCode::FAILURE;
#endif
  }
  else {
    auto workerRM = G4AtlasRunManager::GetG4AtlasRunManager();
    abort = workerRM->SimulateFADSEvent();
  }

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

  ATH_CHECK( m_truthRecordSvc->releaseEvent() );

  return StatusCode::SUCCESS;
}
