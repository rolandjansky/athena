/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsAlg/FadsAlg.h"

#include "G4UImanager.hh"

#include "FadsAlg/FadsRunManager.h"

#include "FadsControl/DetectorConstructionManager.h"
#include "FadsControl/PrimaryGeneratorActionManager.h"
#include "FadsControl/PhysicsListManager.h"
#include "FadsControl/UserActionManager.h"
#include "FadsControl/RunManagerStore.h"

#include "FadsUtilities/Tokenizer.h"
#include "FadsPackageLoader/PackageLoader.h"


/////////////////////////////////////////////////////////////////////////////

FadsAlg::FadsAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , runManager(0)
{
  declareProperty("DetectorConstruction",m_detectorConstruction="");
  declareProperty("PhysicsList",m_physicsList="");
  declareProperty("PrimaryGenerator",m_primaryGenerator="");
  declareProperty("SteppingAction",m_steppingAction="");
  declareProperty("EventAction",m_eventAction="");
  declareProperty("RunAction",m_runAction="");
  declareProperty("TrackingAction",m_trackingAction="");
  declareProperty("StackingAction",m_stackingAction="");
  declareProperty("RunManager",m_runManager="");
  declareProperty("LoadLibs",m_loadLibs="");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FadsAlg::initialize(){

  // Part 1: Get the messaging service, print where you are
  ATH_MSG_INFO( "initialize()" );
  ATH_MSG_INFO( "initializing geant4" );
  ATH_MSG_INFO( "loading necessary libraries first" );

  if (!m_loadLibs.empty())
  {
  	FADS::Tokenizer tok(":",m_loadLibs);
	for (unsigned int i=0;i<tok.size();i++)
		FADS::PackageLoader a(tok[i].c_str());
  }

  RunManagerStoreFacade rm;
  rm.PrintRunManagers();
  if (m_runManager.empty()) {
  	ATH_MSG_WARNING("no Run Manager chosen. using the default one");
	rm.SelectRunManager("DefaultRunManager");
  } else {
  	rm.SelectRunManager(m_runManager);
  }
  runManager = dynamic_cast<FadsRunManager*>(G4RunManager::GetRunManager());
  if (!runManager){
    ATH_MSG_ERROR("Cast of run manager failed!  Will bail out.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Detector construction "<<m_detectorConstruction);
  DetectorConstructionManagerFacade a;
  a.PrintDetectorConstructions();
  if (!m_detectorConstruction.empty())
  	a.SelectDetectorConstruction(m_detectorConstruction);
	
  ATH_MSG_INFO("physics list "<<m_physicsList);
  PhysicsListManagerFacade b;
  b.PrintPhysicsLists();
  if (!m_physicsList.empty())
  	b.SelectPhysicsList(m_physicsList);
	
  ATH_MSG_INFO("primary generator "<<m_primaryGenerator);
  PrimaryGeneratorActionManagerFacade c;
  c.PrintPrimaryGenerators();
  if (!m_primaryGenerator.empty())
  	c.SelectPrimaryGenerator(m_primaryGenerator);
	
  ATH_MSG_INFO("user actions "<<m_primaryGenerator);
  UserActionManagerFacade d;
  d.PrintUserActions();
  if (!m_steppingAction.empty())
  	d.SetSteppingAction(m_steppingAction);
  if (!m_eventAction.empty())
  	d.SetEventAction(m_eventAction);
  if (!m_runAction.empty())
  	d.SetRunAction(m_runAction);
  if (!m_stackingAction.empty())
  	d.SetStackingAction(m_stackingAction);
  if (!m_trackingAction.empty())
  	d.SetTrackingAction(m_trackingAction);
  
  // Initialize G4 kernel
  //
  runManager->Initialize();

  // Get the pointer to the UI manager and set verbosities
  //
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/verbose 1");
  UI->ApplyCommand("/event/verbose 1");
  UI->ApplyCommand("/tracking/verbose 1");
  
  ATH_MSG_INFO("initializing the geant4 run");
  runManager->InitializeRun();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FadsAlg::execute() {
  ATH_MSG_INFO( "execute()" );

  ATH_MSG_INFO( "Trying to simulate one Geant4 event" );
  runManager->SimulateOneEvent();
  ATH_MSG_INFO("done");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode FadsAlg::finalize() {

  ATH_MSG_INFO( "finalize()" );
  runManager->TerminateRun();
  delete runManager;
  runManager=0;

  return StatusCode::SUCCESS;
}
