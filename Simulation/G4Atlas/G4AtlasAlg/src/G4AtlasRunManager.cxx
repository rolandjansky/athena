/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "G4AtlasAlg/G4AtlasRunManager.h"

#include "G4StateManager.hh"
#include "G4UImanager.hh"
#include "G4Timer.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"

#include "G4ios.hh"

#include "G4Event.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/IMessageSvc.h"

#include "FadsKinematics/GeneratorCenter.h"
#include "FadsPhysics/PhysicsListCatalog.h"
#include "FadsPhysics/DummyPhysicsList.h"
#include "G4VUserPrimaryGeneratorAction.hh"

#include <iostream>
#include <string>
#include <vector>

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"

#include "G4LogicalVolumeStore.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCutsTable.hh"
#include "G4GeometryManager.hh"


G4AtlasRunManager::G4AtlasRunManager()
  : G4RunManager(),
    m_pl(NULL), 
    m_sgSvc(NULL), 
    m_releaseGeo(false),
    m_log(0)
{  }


G4AtlasRunManager* G4AtlasRunManager::GetG4AtlasRunManager() {
	static G4AtlasRunManager* thisManager=0;
	if (!thisManager) thisManager=new G4AtlasRunManager;
	return thisManager;
}


void G4AtlasRunManager::InitializeGeometry() {
  G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
  for (unsigned int i=0;i<lvs->size();++i){
    if ( (*lvs)[i]->GetName() == "Muon::MuonSys" ){
      (*lvs)[i]->SetSmartless( 0.1 );
      log() << MSG::INFO << "Set smartlessness for Muon::MuonSys to 0.1" << endreq;
    } else if ( (*lvs)[i]->GetName() == "LArMgr::LAr::EMB::STAC") {
      (*lvs)[i]->SetSmartless( 0.5 );
      log() << MSG::INFO << "Set smartlessness for LArMgr::LAr::EMB::STAC to 0.5" << endreq;
    }
  }

  if (userDetector) {
 	G4RunManager::InitializeGeometry();
  } else {
    log() << MSG::WARNING <<" User Detector not set!!! Geometry NOT initialized!!!"<<endreq;
  }
}


void G4AtlasRunManager::EndEvent() {
  log() << MSG::DEBUG << "G4AtlasRunManager::EndEvent" << endreq;
}


void G4AtlasRunManager::InitializePhysics() {
  kernel->InitializePhysics();
  physicsInitialized = true;
}


G4Event* G4AtlasRunManager::GenerateEvent(G4int i_event)
{
  static FADS::GeneratorCenter* generatorCenter=FADS::GeneratorCenter::GetGeneratorCenter();

  SetCurrentG4Event(i_event);

  generatorCenter->GenerateAnEvent(currentEvent);

  return currentEvent;
}


bool G4AtlasRunManager::SimulateFADSEvent()
{

//	std::cout<<" SimulateFADSEvent : start simulating one event "<<std::endl;

	G4StateManager* stateManager = G4StateManager::GetStateManager();
	stateManager->SetNewState(G4State_GeomClosed);
//    	stateManager->SetNewState(G4State_EventProc);

    // Release GeoModel Geometry if necessary
    if (m_releaseGeo){
        ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
        StoreGateSvc* m_detStore;
        StatusCode sc=svcLocator->service("DetectorStore",m_detStore);
        if (sc.isFailure()){
            log()<<MSG::ERROR<<"G4AtlasRunManager could not access the detector store - PANIC!!!!"<<endreq;
            abort();
        }

        IGeoModelSvc* geoModel = 0;
        StatusCode status=svcLocator->service("GeoModelSvc",geoModel);
        if(status.isFailure()) {
            log()<<MSG::WARNING<< " ----> Unable to retrieve GeoModelSvc" << endreq;
        } else {
            status = geoModel->clear();
            if(status.isFailure()) {
                log()<<MSG::WARNING<< " ----> GeoModelSvc::clear() failed" << endreq;
            } else {
                log()<<MSG::INFO<< " ----> GeoModelSvc::clear() succeeded " << endreq;
            }
        }
        m_releaseGeo=false; // Don't do that again...
    }


    currentEvent = GenerateEvent(1);
	if (currentEvent->IsAborted())
	{
		log()<<MSG::WARNING<<"G4AtlasRunManager::SimulateFADSEvent: "<<
		           "Event Aborted at Generator level"<<endreq;
	    currentEvent=0;
		return true;
	}

    eventManager->ProcessOneEvent(currentEvent);
	if (currentEvent->IsAborted())
	{
		log()<<MSG::WARNING<<"G4AtlasRunManager::SimulateFADSEvent: "<<
		           "Event Aborted at Detector Simulation level"<<endreq;
		currentEvent=0;
		return true;
	}

    AnalyzeEvent(currentEvent);
	if (currentEvent->IsAborted())
	{
		log()<<MSG::WARNING<<"G4AtlasRunManager::SimulateFADSEvent: "<<
		           "Event Aborted at Analysis level"<<endreq;
		currentEvent=0;
		return true;
	}

//    	stateManager->SetNewState(G4State_GeomClosed);
   	StackPreviousEvent(currentEvent);
    bool abort=currentEvent->IsAborted();
   	currentEvent = 0;
	//	std::cout<<" SimulateFADSEvent : done simulating one event "<<std::endl;
	return abort;
}


void  G4AtlasRunManager::RunTermination()
{
  // std::cout<<" this is G4AtlasRunManager::RunTermination() "<<std::endl;
  for(size_t itr=0;itr<previousEvents->size();itr++)
  { delete (*previousEvents)[itr]; }
  previousEvents->clear();

  if(userRunAction) userRunAction->EndOfRunAction(currentRun);

  delete currentRun;
  currentRun = 0;
  runIDCounter++;

  std::cout << "Changing the state..." << std::endl;
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);

  std::cout << "Opening the geometry back up" << std::endl;
  G4GeometryManager::GetInstance()->OpenGeometry();

  std::cout << "Terminating the run...  State is " << stateManager->GetStateString( stateManager->GetCurrentState() ) << std::endl;
  kernel->RunTermination();
  std::cout << "All done..." << std::endl;

  // std::cout<<" setting all pointers in G4AtlasRunManager to 0"<<std::endl;
  userRunAction=0;
  userEventAction=0;
  userSteppingAction=0;
  userStackingAction=0;
  userTrackingAction=0;
  // physicsList=0;
  userDetector=0;
  userPrimaryGeneratorAction=0;

  // std::cout<<" this is G4AtlasRunManager::RunTermination(): done "<<std::endl;
}

void G4AtlasRunManager::SetCurrentG4Event(int iEvent)
{
	currentEvent=new G4Event(iEvent);
}

MsgStream G4AtlasRunManager::log()
{
    if (m_log) return *m_log;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    IMessageSvc* p_msgSvc = 0;
    if (svcLocator->service("MessageSvc", p_msgSvc).isFailure() || !p_msgSvc)
        std::cout << "G4AtlasRunManager: Trouble getting the message service.  Should never happen.  Will crash now." << std::endl;
    m_log = new MsgStream(p_msgSvc,"G4AtlasRunManager");
    return *m_log;
}

