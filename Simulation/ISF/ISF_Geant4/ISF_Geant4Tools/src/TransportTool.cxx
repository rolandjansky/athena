/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "TransportTool.h"

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
#include "MCTruth/EventInformation.h"

// HepMC classes
#include "HepMC/GenParticle.h"

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

#include "AtlasDetDescr/AtlasRegionHelper.h"

// call_once mutexes
#include <mutex>
static std::once_flag initializeOnceFlag;
static std::once_flag finalizeOnceFlag;

//________________________________________________________________________
iGeant4::G4TransportTool::G4TransportTool(const std::string& type,
                                          const std::string& name,
                                          const IInterface*  parent )
  : ISF::BaseSimulatorTool(type, name, parent)
{
  declareProperty("Dll",                   m_libList);
  declareProperty("Physics",               m_physList);
  declareProperty("FieldMap",              m_fieldMap);
  declareProperty("ReleaseGeoModel",       m_releaseGeoModel);
  declareProperty("RecordFlux",            m_recordFlux);
  declareProperty("McEventCollection",     m_mcEventCollectionName);
  declareProperty("G4Commands",            m_g4commands, "Commands to send to the G4UI");
  declareProperty("MultiThreading",        m_useMT, "Multi-threading specific settings");
  //declareProperty("KillAllNeutrinos",      m_KillAllNeutrinos=true);
  //declareProperty("KillLowEPhotons",       m_KillLowEPhotons=-1.);
  declareProperty("PrintTimingInfo",      m_doTiming       );

}

//________________________________________________________________________
iGeant4::G4TransportTool::~G4TransportTool()
{}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::initialize()
{
  ATH_MSG_VERBOSE("initialize");

  ATH_CHECK( ISF::BaseSimulatorTool::initialize() );

  // create G4Timers if enabled
  if (m_doTiming) {
    m_runTimer   = new G4Timer();
    m_eventTimer = new G4Timer();
    m_runTimer->Start();
  }

  ATH_CHECK(m_inputConverter.retrieve());

 // One-time initialization
  try {
    std::call_once(initializeOnceFlag, &iGeant4::G4TransportTool::initializeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in iGeant4::G4TransportTool::initializeOnce: " << e.what());
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_rndmGenSvc.retrieve() );
  ATH_CHECK( m_userActionSvc.retrieve() );

  ATH_CHECK(m_g4atlasSvc.retrieve());

  if (m_recordFlux) G4ScoringManager::GetScoringManager();

  ATH_CHECK (m_detGeoSvc.retrieve());

  ATH_CHECK(m_senDetTool.retrieve());
  ATH_CHECK(m_fastSimTool.retrieve());

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void iGeant4::G4TransportTool::initializeOnce()
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

  if(m_physListTool.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize ATLAS PhysicsListTool!");
  }
  m_physListTool->SetPhysicsList();

  m_pRunMgr->SetRecordFlux( m_recordFlux, std::make_unique<ISFFluxRecorder>() );
  m_pRunMgr->SetLogLevel( int(msg().level()) ); // Synch log levels
  m_pRunMgr->SetUserActionSvc( m_userActionSvc.typeAndName() );
  m_pRunMgr->SetDetGeoSvc( m_detGeoSvc.typeAndName() );
  m_pRunMgr->SetSDMasterTool(m_senDetTool.typeAndName() );
  m_pRunMgr->SetFastSimMasterTool(m_fastSimTool.typeAndName() );
  m_pRunMgr->SetPhysListTool(m_physListTool.typeAndName() );

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
  for (auto g4command : m_g4commands) {
    int returnCode = ui->ApplyCommand( g4command );
    commandLog(returnCode, g4command);
  }

  return;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::finalize()
{
  ATH_MSG_VERBOSE("++++++++++++  ISF G4 G4TransportTool finalized  ++++++++++++");

  // One time finalization
  try {
    std::call_once(finalizeOnceFlag, &iGeant4::G4TransportTool::finalizeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in iGeant4::G4TransportTool::finalizeOnce: " << e.what());
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
void iGeant4::G4TransportTool::finalizeOnce()
{
  ATH_MSG_DEBUG("\t terminating the current G4 run");

  m_pRunMgr->RunTermination();

  return;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::simulate( const ISF::ISFParticle& isp, ISF::ISFParticleContainer& secondaries, McEventCollection* mcEventCollection) {

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
StatusCode iGeant4::G4TransportTool::simulateVector( const ISF::ConstISFParticleVector& particles, ISF::ISFParticleContainer& secondaries, McEventCollection* mcEventCollection) {

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

  for (auto* cisp : particles) {
    auto* isp = const_cast<ISF::ISFParticle*>(cisp);
    // return any secondaries associated with this particle
    auto searchResult = m_secondariesMap.find( isp );
    if ( searchResult == m_secondariesMap.end() ) {

      ATH_MSG_VERBOSE( "Found no secondaries" );

    } else {

      ATH_MSG_VERBOSE( "Found secondaries: " << searchResult->second.size() );
      secondaries.splice( end(secondaries), std::move(searchResult->second) ); //append vector
      m_secondariesMap.erase( searchResult );
    }
  }
  // Geant4 call done
  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::setupEvent()
{
  ATH_MSG_DEBUG ( "setup Event" );

  // Set the RNG to use for this event. We need to reset it for MT jobs
  // because of the mismatch between Gaudi slot-local and G4 thread-local RNG.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  G4Random::setTheEngine(*rngWrapper);

  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

  m_nrOfEntries++;
  if (m_doTiming) m_eventTimer->Start();

  // make sure SD collections are properly initialized in every Athena event
  G4SDManager::GetSDMpointer()->PrepareNewEvent();

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::setupEventST()
{
  return setupEvent();
}

//________________________________________________________________________
StatusCode iGeant4::G4TransportTool::releaseEvent()
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
StatusCode iGeant4::G4TransportTool::releaseEventST()
{
  return releaseEvent();
}

//________________________________________________________________________
// Act as particle broker for G4 secondaries
void iGeant4::G4TransportTool::push( ISF::ISFParticle *particle, const ISF::ISFParticle *parent )
{
  ATH_MSG_VERBOSE( "Caught secondary particle push() from Geant4" );

  m_secondariesMap[ parent ].push_back( particle );
}

//________________________________________________________________________
HepMC::GenEvent* iGeant4::G4TransportTool::genEvent(McEventCollection* mcEventCollection) const
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
void iGeant4::G4TransportTool::commandLog(int returnCode, const std::string& commandString) const
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

