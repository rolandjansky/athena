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

// Geant4 includes
#include "G4TransportationManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4TrackingManager.hh"
#include "G4StackManager.hh"
#include "G4ScoringManager.hh"

// CLHEP includes
#include "CLHEP/Random/RandomEngine.h"

// Athena includes
#include "EventInfo/EventInfo.h"
#include "CxxUtils/make_unique.h"
#include "MCTruthBase/TruthStrategyManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeneratorObjects/HepMcParticleLink.h"

// call_once mutexes
#include <mutex>
static std::once_flag initializeOnceFlag;
static std::once_flag finalizeOnceFlag;

/////////////////////////////////////////////////////////////////////////////


G4AtlasAlg::G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_libList("")
  , m_physList("")
  , m_fieldMap("")
  , m_rndmGen("athena")
  , m_releaseGeoModel(true)
  , m_recordFlux(false)
  , m_killAbortedEvents(true)
  , m_flagAbortedEvents(false)
  , m_inputTruthCollection("BeamTruthEvent")
  , m_outputTruthCollection("TruthEvent")
  , m_useMT(false)
  , m_rndmGenSvc("AtDSFMTGenSvc", name)
  , m_g4atlasSvc("G4AtlasSvc", name)
  , m_userActionSvc("G4UA::UserActionSvc", name) // new user action design
  , m_detGeoSvc("DetectorGeometrySvc", name)
  , m_inputConverter("ISF_InputConverter",name)
  , m_truthRecordSvc("ISF_TruthRecordSvc", name)
  , m_geoIDSvc("ISF_GeoIDSvc", name)
  , m_physListTool("PhysicsListToolBase")
  , m_senDetTool("SensitiveDetectorMasterTool")
  , m_fastSimTool("FastSimulationMasterTool")
{
  declareProperty( "Dll", m_libList);
  declareProperty( "Physics", m_physList);
  declareProperty( "FieldMap", m_fieldMap);
  declareProperty( "RandomGenerator", m_rndmGen);
  declareProperty( "ReleaseGeoModel", m_releaseGeoModel);
  declareProperty( "RecordFlux", m_recordFlux);
  declareProperty( "KillAbortedEvents", m_killAbortedEvents);
  declareProperty( "FlagAbortedEvents", m_flagAbortedEvents);
  declareProperty( "InputTruthCollection", m_inputTruthCollection);
  declareProperty( "OutputTruthCollection", m_outputTruthCollection);

  // Verbosities
  declareProperty("Verbosities", m_verbosities);
  // Commands to send to the G4UI
  declareProperty("G4Commands", m_g4commands);
  // Multi-threading specific settings
  declareProperty("MultiThreading", m_useMT=false);
  // ServiceHandle properties
  declareProperty("AtRndmGenSvc", m_rndmGenSvc);
  declareProperty("G4AtlasSvc", m_g4atlasSvc );
  declareProperty("UserActionSvc", m_userActionSvc);
  declareProperty("GeoIDSvc", m_geoIDSvc);
  declareProperty("InputConverter",        m_inputConverter);
  declareProperty("TruthRecordService", m_truthRecordSvc);
  declareProperty("DetGeoSvc", m_detGeoSvc);
  // ToolHandle properties
  declareProperty("SenDetMasterTool", m_senDetTool);
  declareProperty("FastSimMasterTool", m_fastSimTool);
  declareProperty("PhysicsListTool", m_physListTool);

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::initialize()
{
  ATH_MSG_DEBUG("Start of initialize()");
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

  // FIXME TOO EARLY???
  ATH_CHECK(m_g4atlasSvc.retrieve());

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

  ATH_CHECK(m_inputConverter.retrieve());

  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);


  TruthStrategyManager* sManager = TruthStrategyManager::GetStrategyManager();
  sManager->SetISFTruthSvc( &(*m_truthRecordSvc) );
  sManager->SetISFGeoIDSvc( &(*m_geoIDSvc) );

  ATH_MSG_DEBUG("End of initialize()");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void G4AtlasAlg::initializeOnce()
{
  // Assign physics list
  if(m_physListTool.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize ATLAS PhysicsListTool!");
  }

  std::vector<std::string> localCommands;
  // Load custom libraries
  if (!m_libList.empty()) {
    ATH_MSG_INFO("G4AtlasAlg specific libraries requested ");
    std::string g4command="/load "+m_libList;
    localCommands.push_back(g4command);
  }
  // Load custom physics
  if (!m_physList.empty()) {
    ATH_MSG_INFO("requesting a specific physics list "<< m_physList);
    std::string g4command="/Physics/GetPhysicsList "+m_physList;
    localCommands.push_back(g4command);
  }
  // Load custom magnetic field
  if (!m_fieldMap.empty()) {
    ATH_MSG_INFO("requesting a specific field map "<< m_fieldMap);
    ATH_MSG_INFO("the field is initialized straight away");
    std::string g4command="/MagneticField/Select "+m_fieldMap;
    localCommands.push_back(g4command);
    g4command = "/MagneticField/Initialize";
    localCommands.push_back(g4command);
  }

  localCommands.reserve(localCommands.size()+m_g4commands.size());
  localCommands.insert(std::end(localCommands), std::begin(m_g4commands), std::end(m_g4commands));

  // Create the (master) run manager
  if(m_useMT) {
#ifdef G4MULTITHREADED
    auto* runMgr = G4AtlasMTRunManager::GetG4AtlasMTRunManager();
    m_physListTool->SetPhysicsList();
    // Worker Thread initialization used to create worker run manager on demand.
    // @TODO use this class to pass any configuration to worker run manager.
    runMgr->SetUserInitialization( new G4AtlasUserWorkerThreadInitialization );
    // @TODO configure all tool and service handles as in single threaded case.
#else
    throw std::runtime_error("Trying to use multi-threading in non-MT build!");
#endif
  }
  // Single-threaded run manager
  else {
    auto* runMgr = G4AtlasRunManager::GetG4AtlasRunManager();
    m_physListTool->SetPhysicsList();
    runMgr->SetRecordFlux( m_recordFlux );
    runMgr->SetLogLevel( int(msg().level()) ); // Synch log levels
    runMgr->SetUserActionSvc( m_userActionSvc.typeAndName() );
    runMgr->SetDetGeoSvc( m_detGeoSvc.typeAndName() );
    runMgr->SetSDMasterTool(m_senDetTool.typeAndName() );
    runMgr->SetFastSimMasterTool(m_fastSimTool.typeAndName() );
    runMgr->SetPhysListTool(m_physListTool.typeAndName() );
    // Send UI commands
    for (auto g4command : localCommands) {
      runMgr->AddG4Command(g4command);
    }
  }

  if (m_rndmGen=="athena" || m_rndmGen=="ranecu") {
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
  else if (m_rndmGen=="geant4" || m_rndmGen.empty()) {
    ATH_MSG_INFO("Random nr. generator is set to Geant4");
  }

  // G4 init moved to PyG4AtlasAlg / G4AtlasEngine
  /// @todo Reinstate or delete?! This can't actually be called from the Py algs
  //ATH_MSG_INFO("Firing initialization of G4!!!");
  //initializeG4();
  return;
}

void G4AtlasAlg::initializeG4()
{
  if (m_verbosities.size()>0) {
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

StatusCode G4AtlasAlg::finalize()
{
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
void G4AtlasAlg::finalizeOnce()
{
  ATH_MSG_DEBUG("\t terminating the current G4 run");
  // TODO: could probably just use G4RunManager base class generically.
  //#ifdef ATHENAHIVE
  //  auto runMgr = G4AtlasMTRunManager::GetG4AtlasMTRunManager();
  //#else
  //  auto runMgr = G4AtlasRunManager::GetG4AtlasRunManager();
  //#endif
  auto* runMgr = G4RunManager::GetRunManager();
  runMgr->RunTermination();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::execute()
{
  static int n_Event=0;
  ATH_MSG_DEBUG("++++++++++++  G4AtlasAlg execute  ++++++++++++");

  TruthStrategyManager* sManager = TruthStrategyManager::GetStrategyManager();
  ATH_CHECK( sManager->InitializeWorldVolume() );


  n_Event += 1;

  if (n_Event<=10 || (n_Event%100) == 0) {
    ATH_MSG_ALWAYS("G4AtlasAlg: Event num. "  << n_Event << " start processing");
  }

  // tell TruthService we're starting a new event
  ATH_CHECK( m_truthRecordSvc->initializeTruthCollection() );

  // Release GeoModel Geometry if necessary
  if (m_releaseGeoModel) {
    ATH_CHECK(this->releaseGeoModel());
  }

  ATH_MSG_DEBUG("Calling SimulateG4Event");

  if(!m_senDetTool) {
    // FIXME temporary lazy init. To be (re)moved
    ATH_CHECK(m_senDetTool.retrieve());
  }
  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

  if (!m_inputTruthCollection.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection '" << m_inputTruthCollection.key() << "'");
    return StatusCode::FAILURE;
  }
  // create copy
  m_outputTruthCollection = CxxUtils::make_unique<McEventCollection>(*m_inputTruthCollection);
  G4Event *inputEvent(nullptr);
  ATH_CHECK( m_inputConverter->convertHepMCToG4Event(*m_outputTruthCollection, inputEvent, HepMcParticleLink::find_enumFromKey(m_outputTruthCollection.name())) );

  bool abort = false;
  // Worker run manager
  // Custom class has custom method call: ProcessEvent.
  // So, grab custom singleton class directly, rather than base.
  // Maybe that should be changed! Then we can use a base pointer.
  if(m_useMT) {
#ifdef G4MULTITHREADED
    auto* workerRM = G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager();
    abort = workerRM->ProcessEvent(inputEvent);
#else
    ATH_MSG_ERROR("Trying to use multi-threading in non-MT build!");
    return StatusCode::FAILURE;
#endif
  }
  else {
    auto* workerRM = G4AtlasRunManager::GetG4AtlasRunManager();
    abort = workerRM->ProcessEvent(inputEvent);
  }
  if (abort) {
    ATH_MSG_WARNING("Event was aborted !! ");
    ATH_MSG_WARNING("Simulation will now go on to the next event ");
    if (m_killAbortedEvents) {
      ATH_MSG_WARNING("setFilterPassed is now False");
      setFilterPassed(false);
    }
    if (m_flagAbortedEvents) {
      // FIXME This code is updating an object which is already in
      // StoreGate, which is not really allowed. The long term
      // solution is to switch Simulation to use xAOD::EventInfo, then
      // use an SG::WriteDecorHandle (when available) to set the error
      // state.
      const DataHandle<EventInfo> eic = 0;
      if ( sgSvc()->retrieve( eic ).isFailure() || !eic ) {
        ATH_MSG_WARNING( "Failed to retrieve EventInfo" );
      }
      else {
        // Gotta cast away the const... sadface
        EventInfo *ei = const_cast< EventInfo * > (&(*eic));
        ei->setErrorState(EventInfo::Core,EventInfo::Error);
        ATH_MSG_WARNING( "Set error state in event info!" );
      }
    }
  }

  // Register all of the collections if there are any new-style SDs
  ATH_CHECK(m_senDetTool->EndOfAthenaEvent());
  if(!m_fastSimTool) {
    // FIXME temporary lazy init. To be (re)moved
    ATH_CHECK(m_fastSimTool.retrieve());
  }
  ATH_CHECK(m_fastSimTool->EndOfAthenaEvent());

  ATH_CHECK( m_truthRecordSvc->releaseEvent() );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::releaseGeoModel()
{
  ISvcLocator *svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IGeoModelSvc *geoModel(nullptr);
  if(svcLocator->service("GeoModelSvc",geoModel).isFailure()) {
    ATH_MSG_WARNING( " ----> Unable to retrieve GeoModelSvc" );
  }
  else {
    if(geoModel->clear().isFailure()) {
      ATH_MSG_WARNING( " ----> GeoModelSvc::clear() failed" );
    }
    else {
      ATH_MSG_INFO( " ----> GeoModelSvc::clear() succeeded " );
    }
  }
  m_releaseGeoModel=false; // Don't do that again...
  return StatusCode::SUCCESS;
}
