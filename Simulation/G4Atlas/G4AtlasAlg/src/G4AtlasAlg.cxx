/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/G4AtlasAlg.h"

#include "G4AtlasAlg/PreEventActionManager.h"
#include "G4AtlasAlg/G4AtlasRunManager.h"

#include "GaudiKernel/AlgFactory.h" //FIXME safe to remove?

#include "FadsKinematics/GeneratorCenter.h"

#include "G4TransportationManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4TrackingManager.hh"
#include "G4StackManager.hh"
#include "G4UImanager.hh"
#include "G4ScoringManager.hh"

#include "EventInfo/EventInfo.h"

/////////////////////////////////////////////////////////////////////////////


G4AtlasAlg::G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_rndmGenSvc("AtDSFMTGenSvc", name),m_UASvc("UserActionSvc",name), m_physListTool("PhysicsListToolBase")
{
  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg created  ++++++++++++" << std::endl << std::endl);
  declareProperty( "Dll", m_libList="");
  declareProperty( "Physics", m_physList="");
  declareProperty( "Generator", m_generator="");
  declareProperty( "FieldMap", m_fieldMap="");
  declareProperty( "RandomGenerator", m_rndmNumberSource="athena");
  declareProperty( "ReleaseGeoModel",m_releaseGeoModel=true);
  declareProperty( "RecordFlux",m_recordFlux=false);
  declareProperty( "IncludeParentsInG4Event",m_IncludeParentsInG4Event=false);
  declareProperty( "KillAbortedEvents", m_killAbortedEvents=true);
  declareProperty( "FlagAbortedEvents", m_flagAbortedEvents=false);

  // Tool and Service instantiation
  declareProperty("AtRndmGenSvc", m_rndmGenSvc);
  declareProperty("UserActionSvc", m_UASvc);
  declareProperty("PhysicsListTool", m_physListTool);


  p_runMgr = G4AtlasRunManager::GetG4AtlasRunManager();
  // if (m_noGeomInit) p_runMgr->NoGeomInit();

  // Verbosities
  m_verbosities.clear();
  declareProperty( "Verbosities" , m_verbosities );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::initialize()
{
  if (m_recordFlux) G4ScoringManager::GetScoringManager();

  CHECK(m_physListTool.retrieve());
  m_physListTool->SetPhysicsList();

  CHECK(m_UASvc.retrieve());

  G4UImanager *ui = G4UImanager::GetUIpointer();

  if (!m_libList.empty())
    {
      ATH_MSG_INFO("G4AtlasAlg specific libraries requested ") ;
      std::string temp="/load "+m_libList;
      ui->ApplyCommand(temp);
    }
  if (!m_physList.empty())
    {
      ATH_MSG_INFO("requesting a specific physics list "<< m_physList) ;
      std::string temp="/Physics/GetPhysicsList "+m_physList;
      ui->ApplyCommand(temp);
    }
  FADS::GeneratorCenter * gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->SetIncludeParentsInG4Event( m_IncludeParentsInG4Event );
  if (!m_generator.empty())
    {
      ATH_MSG_INFO("requesting a specific generator "<< m_generator) ;
      gc->SelectGenerator(m_generator);
    } else {
    // make sure that there is a default generator (i.e. HepMC interface)
    gc->SelectGenerator("AthenaHepMCInterface");
  }
  if (!m_fieldMap.empty())
    {
      ATH_MSG_INFO("requesting a specific field map "<< m_fieldMap) ;
      ATH_MSG_INFO("the field is initialized straight away") ;
      std::string temp="/MagneticField/Select "+m_fieldMap;
      ui->ApplyCommand(temp);
      ui->ApplyCommand("/MagneticField/Initialize");
    }

  p_runMgr->SetReleaseGeo( m_releaseGeoModel );
  p_runMgr->SetLogLevel( int(msg().level()) ); // Synch log levels

  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg initialized  ++++++++++++" << std::endl << std::endl);

  ATH_MSG_INFO("Setting checkmode to true");
  ui->ApplyCommand("/geometry/navigator/check_mode true");



  if (m_rndmNumberSource=="athena" || m_rndmNumberSource=="ranecu") {
    // Set the random number generator to AtRndmGen
    if (m_rndmGenSvc.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not initialize ATLAS Random Generator Service");
      return StatusCode::FAILURE;
    }
    CLHEP::HepRandomEngine* engine = m_rndmGenSvc->GetEngine("AtlasG4");
    CLHEP::HepRandom::setTheEngine(engine);
    ATH_MSG_INFO("Random nr. generator is set to Athena");
  }
  else if (m_rndmNumberSource=="geant4" || m_rndmNumberSource.empty()) {
    ATH_MSG_INFO("Random nr. generator is set to Geant4");
  }

  // G4 init moved to PyG4AtlasAlg / G4AtlasEngine
  /// @todo Reinstate or delete?! This can't actually be called from the Py algs
  //ATH_MSG_INFO("Firing initialization of G4!!!");
  //initializeG4();

  return StatusCode::SUCCESS;

}

void G4AtlasAlg::initializeG4()
{

  if (m_verbosities.size()>0){
    G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    G4EventManager *em = G4EventManager::GetEventManager();

    if (m_verbosities.find("Navigator") != m_verbosities.end()){
      tm->GetNavigatorForTracking()->SetVerboseLevel( atof(m_verbosities.find("Navigator")->second.data()) );
    }
    if (m_verbosities.find("Propagator") != m_verbosities.end()){
      tm->GetPropagatorInField()->SetVerboseLevel( atof(m_verbosities.find("Propagator")->second.data()) );
    }
    if (m_verbosities.find("Tracking") != m_verbosities.end()){
      rmk->GetTrackingManager()->SetVerboseLevel( atof(m_verbosities.find("Tracking")->second.data()) );
    }
    if (m_verbosities.find("Stepping") != m_verbosities.end()){
      rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel( atof(m_verbosities.find("Stepping")->second.data()) );
    }
    if (m_verbosities.find("Stacking") != m_verbosities.end()){
      rmk->GetStackManager()->SetVerboseLevel( atof(m_verbosities.find("Stacking")->second.data()) );
    }
    if (m_verbosities.find("Event") != m_verbosities.end()){
      em->SetVerboseLevel( atof(m_verbosities.find("Event")->second.data()) );
    }
  } // End of the setting of verbosities

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::finalize() {
  ATH_MSG_DEBUG(std::endl<<std::endl<<std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg finalized  ++++++++++++" <<std::endl<<std::endl);

  ATH_MSG_DEBUG("\t terminating the current G4 run");

  p_runMgr->RunTermination();

  return StatusCode::SUCCESS;
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

  preEvent->Execute();

  ATH_MSG_DEBUG("Calling SimulateG4Event");

  bool abort = p_runMgr->SimulateFADSEvent();

  if (abort) {
    ATH_MSG_WARNING("Event was aborted !! ");
    ATH_MSG_WARNING("Simulation will now go on to the next event ");
    if (m_killAbortedEvents){
      ATH_MSG_WARNING("setFilterPassed is now False");
      setFilterPassed(false);
    }
    if (m_flagAbortedEvents){
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
