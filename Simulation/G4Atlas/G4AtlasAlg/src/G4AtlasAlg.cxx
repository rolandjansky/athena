/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4AtlasAlg.h"
#include "G4AtlasFluxRecorder.h"

#include "AthenaKernel/RNGWrapper.h"

// Can we safely include all of these?
#include "G4AtlasAlg/G4AtlasMTRunManager.h"
#include "G4AtlasAlg/G4AtlasWorkerRunManager.h"
#include "G4AtlasAlg/G4AtlasUserWorkerThreadInitialization.h"
#include "G4AtlasAlg/G4AtlasRunManager.h"

// Geant4 includes
#include "G4StateManager.hh"
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

// Athena includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "EventInfo/EventInfo.h"
#include "MCTruthBase/TruthStrategyManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/IThreadInitTool.h"
#include "GeneratorObjects/HepMcParticleLink.h"

// call_once mutexes
#include <mutex>
static std::once_flag initializeOnceFlag;
static std::once_flag finalizeOnceFlag;
static std::once_flag releaseGeoModelOnceFlag;

/////////////////////////////////////////////////////////////////////////////


G4AtlasAlg::G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty( "Dll", m_libList);
  declareProperty( "Physics", m_physList);
  declareProperty( "FieldMap", m_fieldMap);
  declareProperty( "RandomGenerator", m_rndmGen);
  declareProperty( "ReleaseGeoModel", m_releaseGeoModel);
  declareProperty( "RecordFlux", m_recordFlux);
  declareProperty( "KillAbortedEvents", m_killAbortedEvents);
  declareProperty( "FlagAbortedEvents", m_flagAbortedEvents);
  declareProperty("G4Commands", m_g4commands, "Commands to send to the G4UI");
  // Multi-threading specific settings
  declareProperty("MultiThreading", m_useMT, "Multi-threading specific settings");

  // Verbosities
  declareProperty("Verbosities", m_verbosities);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::initialize()
{
  ATH_MSG_DEBUG("Start of initialize()");

  // Input/Ouput Keys
  ATH_CHECK( m_inputTruthCollectionKey.initialize());
  ATH_CHECK( m_outputTruthCollectionKey.initialize());

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

  ATH_CHECK( m_rndmGenSvc.retrieve() );
  ATH_CHECK( m_userActionSvc.retrieve() );

  // FIXME TOO EARLY???
  ATH_CHECK(m_g4atlasSvc.retrieve());

  ATH_CHECK( m_truthRecordSvc.retrieve() );
  ATH_MSG_INFO( "- Using ISF TruthRecordSvc : " << m_truthRecordSvc.typeAndName() );
  ATH_CHECK( m_geoIDSvc.retrieve() );
  ATH_MSG_INFO( "- Using ISF GeoIDSvc       : " << m_geoIDSvc.typeAndName() );

  ATH_CHECK(m_inputConverter.retrieve());

  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);


  TruthStrategyManager* sManager = TruthStrategyManager::GetStrategyManager();
  sManager->SetISFTruthSvc( &(*m_truthRecordSvc) );
  sManager->SetISFGeoIDSvc( &(*m_geoIDSvc) );

  ATH_CHECK(m_senDetTool.retrieve());
  ATH_CHECK(m_fastSimTool.retrieve());

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
    runMgr->SetRecordFlux( m_recordFlux, std::make_unique<G4AtlasFluxRecorder>() );
    runMgr->SetLogLevel( int(msg().level()) ); // Synch log levels
    runMgr->SetUserActionSvc( m_userActionSvc.typeAndName() );
    runMgr->SetDetGeoSvc( m_detGeoSvc.typeAndName() );
    runMgr->SetSDMasterTool(m_senDetTool.typeAndName() );
    runMgr->SetFastSimMasterTool(m_fastSimTool.typeAndName() );
    runMgr->SetPhysListTool(m_physListTool.typeAndName() );
  }

  // G4 user interface commands
  G4UImanager *ui = G4UImanager::GetUIpointer();

  // Load custom libraries
  if (!m_libList.empty()) {
    ATH_MSG_INFO("G4AtlasAlg specific libraries requested ");
    std::string temp="/load "+m_libList;
    ui->ApplyCommand(temp);
  }
  // Load custom physics
  if (!m_physList.empty()) {
    ATH_MSG_INFO("requesting a specific physics list "<< m_physList);
    std::string temp="/Physics/GetPhysicsList "+m_physList;
    ui->ApplyCommand(temp);
  }
  // Load custom magnetic field
  if (!m_fieldMap.empty()) {
    ATH_MSG_INFO("requesting a specific field map "<< m_fieldMap);
    ATH_MSG_INFO("the field is initialized straight away");
    std::string temp="/MagneticField/Select "+m_fieldMap;
    ui->ApplyCommand(temp);
    ui->ApplyCommand("/MagneticField/Initialize");
  }

  // Send UI commands
  ATH_MSG_DEBUG("G4 Command: Trying at the end of initializeOnce()");
  for (auto g4command : m_g4commands) {
    int returnCode = ui->ApplyCommand( g4command );
    commandLog(returnCode, g4command);
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
  auto runMgr = G4RunManager::GetRunManager();
  runMgr->RunTermination();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::execute()
{
  static int n_Event=0;
  ATH_MSG_DEBUG("++++++++++++  G4AtlasAlg execute  ++++++++++++");

#ifdef G4MULTITHREADED
  // In some rare cases, TBB may create more physical worker threads than
  // were requested via the pool size.  This can happen at any time.
  // In that case, those extra threads will not have had the thread-local
  // initialization done, leading to a crash.  Try to detect that and do
  // the initialization now if needed.
  if (G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume() == nullptr)
  {
    ToolHandle<IThreadInitTool> ti ("G4ThreadInitTool", nullptr);
    ATH_CHECK( ti.retrieve() );
    ti->initThread();
  }
#endif

  n_Event += 1;

  if (n_Event<=10 || (n_Event%100) == 0) {
    ATH_MSG_ALWAYS("G4AtlasAlg: Event num. "  << n_Event << " start processing");
  }

  // tell TruthService we're starting a new event
  ATH_CHECK( m_truthRecordSvc->initializeTruthCollection() );

  // Release GeoModel Geometry if necessary
  if (m_releaseGeoModel) {
    try {
      std::call_once(releaseGeoModelOnceFlag, &G4AtlasAlg::releaseGeoModel, this);
    }
    catch(const std::exception& e) {
      ATH_MSG_ERROR("Failure in G4AtlasAlg::releaseGeoModel: " << e.what());
      return StatusCode::FAILURE;
    }
  }

  // Set the RNG to use for this event. We need to reset it for MT jobs
  // because of the mismatch between Gaudi slot-local and G4 thread-local RNG.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  G4Random::setTheEngine(*rngWrapper);

  ATH_MSG_DEBUG("Calling SimulateG4Event");

  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

  SG::ReadHandle<McEventCollection> inputTruthCollection(m_inputTruthCollectionKey);
  if (!inputTruthCollection.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection " << inputTruthCollection.name() << " in store " << inputTruthCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input GenEvent collection " << inputTruthCollection.name() << " in store " << inputTruthCollection.store());
  // create copy
  SG::WriteHandle<McEventCollection> outputTruthCollection(m_outputTruthCollectionKey);
  ATH_CHECK(outputTruthCollection.record(std::make_unique<McEventCollection>(*inputTruthCollection)));
  if (!outputTruthCollection.isValid()) {
    ATH_MSG_FATAL("Unable to record output GenEvent collection " << outputTruthCollection.name() << " in store " << outputTruthCollection.store());
    return StatusCode::FAILURE;

  }
  ATH_MSG_DEBUG("Recorded output GenEvent collection " << outputTruthCollection.name() << " in store " << outputTruthCollection.store());
  G4Event *inputEvent{};
  ATH_CHECK( m_inputConverter->convertHepMCToG4Event(*outputTruthCollection, inputEvent, HepMcParticleLink::find_enumFromKey(outputTruthCollection.name())) );

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
  ATH_CHECK(m_fastSimTool->EndOfAthenaEvent());

  ATH_CHECK( m_truthRecordSvc->releaseEvent() );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void G4AtlasAlg::releaseGeoModel()
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
  return;
}

void G4AtlasAlg::commandLog(int returnCode, const std::string& commandString) const
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
