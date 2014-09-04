/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/G4AtlasAlg.h"

#include "G4AtlasAlg/PreEventActionManager.h"
#include "G4AtlasAlg/AthenaTrackingAction.h"
#include "G4AtlasAlg/AthenaStackingAction.h"
#include "G4AtlasAlg/G4AtlasRunManager.h"

#include "GaudiKernel/AlgFactory.h"

#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsSteppingAction.h"
#include "FadsActions/FadsRunAction.h"
#include "FadsActions/FadsTrackingAction.h"
#include "FadsActions/FadsStackingAction.h"
#include "FadsKinematics/GeneratorCenter.h"
#include "FadsPhysics/PhysicsListCatalog.h"
#include "FadsPhysics/PhysicsListSteering.h"
#include "FadsGeometry/FadsDetectorConstruction.h"

#include "G4TransportationManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4TrackingManager.hh"
#include "G4StackManager.hh"
#include "G4UImanager.hh"

#include "EventInfo/EventInfo.h" 

/////////////////////////////////////////////////////////////////////////////


G4AtlasAlg::G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_rndmGenSvc("AtDSFMTGenSvc", name)
{
  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg created  ++++++++++++" << std::endl << std::endl);
  declareProperty( "Dll", libList="");
  declareProperty( "Physics", physList="");
  declareProperty( "Generator", generator="");
  declareProperty( "FieldMap", fieldMap="");
  declareProperty( "RandomGenerator", rndmGen="athena");
  declareProperty( "KillAllNeutrinos",m_KillAllNeutrinos=true);
  declareProperty( "KillLowEPhotons",m_KillLowEPhotons=-1.);
  declareProperty( "ReleaseGeoModel",m_releaseGeoModel=true);
  declareProperty( "IncludeParentsInG4Event",m_IncludeParentsInG4Event=false);
  declareProperty( "KillAbortedEvents", m_killAbortedEvents=true); 
  declareProperty( "FlagAbortedEvents", m_flagAbortedEvents=false); 

  // Service instantiation
  declareProperty("AtRndmGenSvc", m_rndmGenSvc);

  p_runMgr = G4AtlasRunManager::GetG4AtlasRunManager();
  // if (m_noGeomInit) p_runMgr->NoGeomInit();

  G4VUserPhysicsList *thePL=FADS::PhysicsListCatalog::GetInstance()->GetMainPhysicsList();
  p_runMgr->SetPhysicsList(thePL);
  p_runMgr->SetUserInitialization(thePL);
  p_runMgr->SetUserInitialization(new FADS::FadsDetectorConstruction);
  trackingAction =new AthenaTrackingAction;
  stackingAction =new AthenaStackingAction;
  p_runMgr->SetUserAction(FADS::FadsRunAction::GetRunAction());
  p_runMgr->SetUserAction(FADS::FadsEventAction::GetEventAction());
  p_runMgr->SetUserAction(FADS::FadsSteppingAction::GetSteppingAction());
  p_runMgr->SetUserAction(FADS::FadsTrackingAction::GetTrackingAction());
  p_runMgr->SetUserAction(FADS::FadsStackingAction::GetStackingAction());

  // Verbosities
  m_verbosities.clear();
  declareProperty( "Verbosities" , m_verbosities );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4AtlasAlg::initialize()
{
  G4UImanager *ui = G4UImanager::GetUIpointer();

  if (!libList.empty())
    {
      ATH_MSG_INFO("G4AtlasAlg specific libraries requested ") ;
      std::string temp="/load "+libList;
      ui->ApplyCommand(temp);
    }
  if (!physList.empty())
    {
      ATH_MSG_INFO("requesting a specific physics list "<< physList) ;
      std::string temp="/Physics/GetPhysicsList "+physList;
      ui->ApplyCommand(temp);
    }
  FADS::GeneratorCenter * gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->SetIncludeParentsInG4Event( m_IncludeParentsInG4Event );
  if (!generator.empty())
    {
      ATH_MSG_INFO("requesting a specific generator "<< generator) ;
      gc->SelectGenerator(generator);
    } else {
    // make sure that there is a default generator (i.e. HepMC interface)
    gc->SelectGenerator("AthenaHepMCInterface");
  }
  if (!fieldMap.empty())
    {
      ATH_MSG_INFO("requesting a specific field map "<< fieldMap) ;
      ATH_MSG_INFO("the field is initialized straight away") ;
      std::string temp="/MagneticField/Select "+fieldMap;
      ui->ApplyCommand(temp);
      ui->ApplyCommand("/MagneticField/Initialize");
    }

  stackingAction->KillAllNeutrinos(m_KillAllNeutrinos);
  stackingAction->KillLowEPhotons(m_KillLowEPhotons);
  p_runMgr->SetReleaseGeo( m_releaseGeoModel );
  p_runMgr->SetLogLevel( int(msg().level()) ); // Synch log levels

  ATH_MSG_DEBUG(std::endl << std::endl << std::endl);
  ATH_MSG_INFO("++++++++++++  G4AtlasAlg initialized  ++++++++++++" << std::endl << std::endl);

  ATH_MSG_INFO("Setting checkmode to true");
  ui->ApplyCommand("/geometry/navigator/check_mode true");

  if (rndmGen=="athena" || rndmGen=="ranecu")	{
    // Set the random number generator to AtRndmGen
    if (m_rndmGenSvc.retrieve().isFailure()) {
      ATH_MSG_ERROR("Could not initialize ATLAS Random Generator Service");
      return StatusCode::FAILURE;
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
