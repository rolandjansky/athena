/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "G4LegacyTransportTool.h"

//package includes
#include "G4AtlasAlg/G4AtlasRunManager.h"
#include "ISFFluxRecorder.h"

#include "AthenaKernel/RNGWrapper.h"

// ISF classes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ISFParticleVector.h"

// Athena classes
#include "GeneratorObjects/McEventCollection.h"

#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/AtlasG4EventUserInfo.h"

// HepMC classes
#include "AtlasHepMC/GenParticle.h"

// Geant4 classes
#include "G4LorentzVector.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Trajectory.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4ParticleTable.hh"
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"
#include "G4ScoringManager.hh"
#include "G4Timer.hh"
#include "G4SDManager.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "G4ParallelWorldPhysics.hh"

#include "AtlasDetDescr/AtlasRegionHelper.h"

// call_once mutexes
#include <mutex>
static std::once_flag initializeOnceFlag;
static std::once_flag finalizeOnceFlag;

//________________________________________________________________________
iGeant4::G4LegacyTransportTool::G4LegacyTransportTool(const std::string& type,
                                          const std::string& name,
                                          const IInterface*  parent )
  : ISF::BaseSimulatorTool(type, name, parent)
{
  //declareProperty("KillAllNeutrinos",      m_KillAllNeutrinos=true);
  //declareProperty("KillLowEPhotons",       m_KillLowEPhotons=-1.);
}

//________________________________________________________________________
StatusCode iGeant4::G4LegacyTransportTool::initialize()
{
  ATH_MSG_VERBOSE("initialize");

  ATH_CHECK( ISF::BaseSimulatorTool::initialize() );

  // create G4Timers if enabled
  if (m_doTiming) {
    m_runTimer   = new G4Timer();
    m_eventTimer = new G4Timer();
    m_runTimer->Start();
  }

  // Create the scoring manager if requested
  if (m_recordFlux) G4ScoringManager::GetScoringManager();

  // One-time initialization
  try {
    std::call_once(initializeOnceFlag, &iGeant4::G4LegacyTransportTool::initializeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in iGeant4::G4LegacyTransportTool::initializeOnce: " << e.what());
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_rndmGenSvc.retrieve() );
  ATH_CHECK( m_userActionSvc.retrieve() );

  ATH_CHECK(m_senDetTool.retrieve());
  ATH_CHECK(m_fastSimTool.retrieve());

  ATH_CHECK(m_inputConverter.retrieve());

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void iGeant4::G4LegacyTransportTool::initializeOnce()
{
  // get G4AtlasRunManager
  ATH_MSG_DEBUG("initialize G4AtlasRunManager");

  if (m_g4RunManagerHelper.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize G4RunManagerHelper!");
  }
  ATH_MSG_DEBUG("retrieved "<<m_g4RunManagerHelper);
  m_pRunMgr = m_g4RunManagerHelper ? m_g4RunManagerHelper->g4RunManager() : nullptr;
  if (!m_pRunMgr) {
    throw std::runtime_error("G4RunManagerHelper::g4RunManager() returned nullptr.");
  }

  if(m_physListSvc.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize ATLAS PhysicsListSvc!");
  }
  m_physListSvc->SetPhysicsList();

  m_pRunMgr->SetRecordFlux( m_recordFlux, std::make_unique<ISFFluxRecorder>() );
  m_pRunMgr->SetLogLevel( int(msg().level()) ); // Synch log levels
  m_pRunMgr->SetUserActionSvc( m_userActionSvc.typeAndName() );
  m_pRunMgr->SetDetGeoSvc( m_detGeoSvc.typeAndName() );
  m_pRunMgr->SetSDMasterTool(m_senDetTool.typeAndName() );
  m_pRunMgr->SetFastSimMasterTool(m_fastSimTool.typeAndName() );
  m_pRunMgr->SetPhysListSvc(m_physListSvc.typeAndName() );

  G4UImanager *ui = G4UImanager::GetUIpointer();

  if (!m_libList.empty()) {
    ATH_MSG_INFO("G4AtlasAlg specific libraries requested ") ;
    std::string temp="/load "+m_libList;
    ui->ApplyCommand(temp);
  }

  if (!m_physList.empty()) {
    ATH_MSG_INFO("requesting a specific physics list "<< m_physList) ;
    std::string temp="/Physics/GetPhysicsList "+m_physList;
    ui->ApplyCommand(temp);
  }

  if (!m_fieldMap.empty()) {
    ATH_MSG_INFO("requesting a specific field map "<< m_fieldMap) ;
    ATH_MSG_INFO("the field is initialized straight away") ;
    std::string temp="/MagneticField/Select "+m_fieldMap;
    ui->ApplyCommand(temp);
    ui->ApplyCommand("/MagneticField/Initialize");
  }

  // Send UI commands
  ATH_MSG_DEBUG("G4 Command: Trying at the end of initializeOnce()");
  for (const auto& g4command : m_g4commands) {
    int returnCode = ui->ApplyCommand( g4command );
    commandLog(returnCode, g4command);
  }

  // Code from G4AtlasSvc
  auto* rm = G4RunManager::GetRunManager();
  if(!rm) {
    throw std::runtime_error("Run manager retrieval has failed");
  }
  rm->Initialize();     // Initialization differs slightly in multi-threading.
  // TODO: add more details about why this is here.
  if(!m_useMT && rm->ConfirmBeamOnCondition()) {
    rm->RunInitialization();
  }

  ATH_MSG_INFO( "retireving the Detector Geometry Service" );
  if(m_detGeoSvc.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize ATLAS DetectorGeometrySvc!");
  }

  if(m_userLimitsSvc.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize ATLAS UserLimitsSvc!");
  }

  if (m_activateParallelGeometries) {
    G4VModularPhysicsList* thePhysicsList=dynamic_cast<G4VModularPhysicsList*>(m_physListSvc->GetPhysicsList());
    if (!thePhysicsList) {
      throw std::runtime_error("Failed dynamic_cast!! this is not a G4VModularPhysicsList!");
    }
#if G4VERSION_NUMBER >= 1010
    std::vector<std::string>& parallelWorldNames=m_detGeoSvc->GetParallelWorldNames();
    for (auto& it: parallelWorldNames) {
      thePhysicsList->RegisterPhysics(new G4ParallelWorldPhysics(it,true));
    }
#endif
  }

  return;
}

//________________________________________________________________________
StatusCode iGeant4::G4LegacyTransportTool::finalize()
{
  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4LegacyTransportTool finalized  ++++++++++++");

  // One time finalization
  try {
    std::call_once(finalizeOnceFlag, &iGeant4::G4LegacyTransportTool::finalizeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in iGeant4::G4LegacyTransportTool::finalizeOnce: " << e.what());
    return StatusCode::FAILURE;
  }

  if (m_doTiming) {
    m_runTimer->Stop();
    float runTime=m_runTimer->GetUserElapsed()+m_runTimer->GetSystemElapsed();
    float avgTimePerEvent=(m_nrOfEntries>1) ? m_accumulatedEventTime/(m_nrOfEntries-1.) : runTime;
    float sigma=( m_nrOfEntries>2) ? std::sqrt((m_accumulatedEventTimeSq/float(m_nrOfEntries-1)-
                                                avgTimePerEvent*avgTimePerEvent)/float(m_nrOfEntries-2)) : 0;
    ATH_MSG_INFO("*****************************************"<<endmsg<<
                 "**                                     **"<<endmsg<<
                 "    End of run - time spent is "<<std::setprecision(4) <<
                 runTime<<endmsg<<
                 "    Average time per event was "<<std::setprecision(4) <<
                 avgTimePerEvent <<" +- "<< std::setprecision(4) << sigma<<endmsg<<
                 "**                                     **"<<endmsg<<
                 "*****************************************");
  }

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void iGeant4::G4LegacyTransportTool::finalizeOnce()
{
  ATH_MSG_DEBUG("\t terminating the current G4 run");

  m_pRunMgr->RunTermination();

  return;
}

//________________________________________________________________________
StatusCode iGeant4::G4LegacyTransportTool::simulate( const ISF::ISFParticle& isp, ISF::ISFParticleContainer& secondaries, McEventCollection* mcEventCollection) const {

  // give a screen output that you entered Geant4SimSvc
  ATH_MSG_VERBOSE( "Particle " << isp << " received for simulation." );

  /** Process ParticleState from particle stack */
  // wrap the given ISFParticle into a STL vector of ISFParticles with length 1
  // (minimizing code duplication)
  const ISF::ConstISFParticleVector ispVector(1, &isp);
  StatusCode success = this->simulateVector(ispVector, secondaries, mcEventCollection);
  ATH_MSG_VERBOSE( "Simulation done" );

  // Geant4 call done
  return success;
}

//________________________________________________________________________
StatusCode iGeant4::G4LegacyTransportTool::simulateVector( const ISF::ConstISFParticleVector& particles, ISF::ISFParticleContainer& secondaries, McEventCollection* mcEventCollection) const {

  ATH_MSG_DEBUG (name() << ".simulateVector(...) : Received a vector of " << particles.size() << " particles for simulation.");
  /** Process ParticleState from particle stack */
  G4Event* inputEvent = m_inputConverter->ISF_to_G4Event(particles, genEvent(mcEventCollection));
  if (!inputEvent) {
    ATH_MSG_ERROR("ISF Event conversion failed ");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Calling ISF_Geant4 ProcessEvent");
  bool abort = m_pRunMgr->ProcessEvent(inputEvent);

  if (abort) {
    ATH_MSG_WARNING("Event was aborted !! ");
    //ATH_MSG_WARNING("Simulation will now go on to the next event ");
    //ATH_MSG_WARNING("setFilterPassed is now False");
    //setFilterPassed(false);
    return StatusCode::FAILURE;
  }


  // const DataHandle <TrackRecordCollection> tracks;

  // StatusCode sc = evtStore()->retrieve(tracks,m_trackCollName);

  // if (sc.isFailure()) {
  //   ATH_MSG_WARNING(" Cannot retrieve TrackRecordCollection " << m_trackCollName);
  // }

  // not implemented yet... need to get particle stack from Geant4 and convert to ISFParticle
  ATH_MSG_VERBOSE( "Simulation done" );

  Slot& slot = *m_slots;
  Slot::lock_t lock (slot.m_mutex);
  
  for (auto* cisp : particles) {
    // return any secondaries associated with this particle
    auto searchResult = slot.m_secondariesMap.find( cisp );
    if ( searchResult == slot.m_secondariesMap.end() ) {

      ATH_MSG_VERBOSE( "Found no secondaries" );

    } else {

      ATH_MSG_VERBOSE( "Found secondaries: " << searchResult->second.size() );
      secondaries.splice( end(secondaries), std::move(searchResult->second) ); //append vector
      slot.m_secondariesMap.erase( searchResult );
    }
  }
  // Geant4 call done
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4LegacyTransportTool::setupEvent()
{
  ATH_MSG_DEBUG ( "setup Event" );

  // Set the RNG to use for this event. We need to reset it for MT jobs
  // because of the mismatch between Gaudi slot-local and G4 thread-local RNG.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeed( m_randomStreamName, Gaudi::Hive::currentContext() );
  G4Random::setTheEngine(*rngWrapper);

  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

  m_nrOfEntries++;
  if (m_doTiming) m_eventTimer->Start();

  // make sure SD collections are properly initialized in every Athena event
  G4SDManager::GetSDMpointer()->PrepareNewEvent();

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4LegacyTransportTool::releaseEvent()
{
  ATH_MSG_DEBUG ( "release Event" );
  /** @todo : strip hits of the tracks ... */

  /* todo: ELLI: the following is copied in from the PyG4AtlasAlg:
     -> this somehow needs to be moved into C++
     and put into releaseEvent() ( or setupEvent() ?)

     from ISF_Geant4Example import AtlasG4Eng
     from ISF_Geant4Example.ISF_SimFlags import simFlags
     if self.doFirstEventG4SeedsCheck :
     if simFlags.SeedsG4.statusOn:
     rnd = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
     rnd.set_Seed(simFlags.SeedsG4.get_Value())
     self.doFirstEventG4SeedsCheck = False
     if self.RndG4Menu.SaveStatus:
     self.RndG4Menu.Menu.saveStatus('G4Seeds.txt')
  */

  // print per-event timing info if enabled
  if (m_doTiming) {
    m_eventTimer->Stop();

    double eventTime=m_eventTimer->GetUserElapsed()+m_eventTimer->GetSystemElapsed();
    if (m_nrOfEntries>1) {
      m_accumulatedEventTime  +=eventTime;
      m_accumulatedEventTimeSq+=eventTime*eventTime;
    }

    float avgTimePerEvent=(m_nrOfEntries>1) ? m_accumulatedEventTime/(m_nrOfEntries-1.) : eventTime;
    float sigma=(m_nrOfEntries>2) ? std::sqrt((m_accumulatedEventTimeSq/float(m_nrOfEntries-1)-
                                               avgTimePerEvent*avgTimePerEvent)/float(m_nrOfEntries-2)) : 0.;

    ATH_MSG_INFO("\t Event nr. "<<m_nrOfEntries<<" took " << std::setprecision(4) <<
                 eventTime << " s. New average " << std::setprecision(4) <<
                 avgTimePerEvent<<" +- "<<std::setprecision(4) << sigma);
  }

  ATH_CHECK(m_senDetTool->EndOfAthenaEvent());
  ATH_CHECK(m_fastSimTool->EndOfAthenaEvent());

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
HepMC::GenEvent* iGeant4::G4LegacyTransportTool::genEvent(McEventCollection* mcEventCollection) const
{

  if(!mcEventCollection) {
    // retrieve McEventCollection from storegate
    if (evtStore()->contains<McEventCollection>(m_mcEventCollectionName)) {
      if (evtStore()->retrieve( mcEventCollection, m_mcEventCollectionName).isFailure()) {
        ATH_MSG_ERROR( "Unable to retrieve McEventCollection with name=" << m_mcEventCollectionName
                         << ".");
        return nullptr;
      }
      else {
        ATH_MSG_WARNING( "Fallback. Sucessfully retrieved McEventCollection with name=" << m_mcEventCollectionName);
      }
    }
    else { return nullptr; }
  }
  // collect last GenEvent from McEventCollection
  return mcEventCollection->back();
}

//________________________________________________________________________
void iGeant4::G4LegacyTransportTool::commandLog(int returnCode, const std::string& commandString) const
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

