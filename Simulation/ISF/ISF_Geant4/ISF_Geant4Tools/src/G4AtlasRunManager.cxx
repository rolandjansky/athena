/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelInterfaces/IGeoModelSvc.h"

//#include "ISF_Geant4UserActions/TrackProcessor.h"

#include "ISF_Geant4Tools/G4AtlasRunManager.h"
#include "G4LogicalVolumeStore.hh"
#include "G4StateManager.hh"
#include "G4GeometryManager.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4RegionStore.hh"

//________________________________________________________________________
iGeant4::G4AtlasRunManager::G4AtlasRunManager()
: G4RunManager(),
//  m_pl(0),
//  m_trackProcessorUserAction(0), 
  m_releaseGeo(false), 
  m_log(0)
{
  //m_trackProcessorUserAction=new iGeant4::TrackProcessor("ISG_G4TrackProcessor");
  //std::cout<<"iGeant4::G4AtlasRunManager::G4AtlasRunManager() "<<this<<std::endl;
}

//________________________________________________________________________
iGeant4::G4AtlasRunManager::~G4AtlasRunManager()
{
  delete m_log;
}

//________________________________________________________________________
MsgStream iGeant4::G4AtlasRunManager::log()
{
  if (m_log) return *m_log;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* msgSvc = 0;
  if (svcLocator->service("MessageSvc", msgSvc).isFailure() || !msgSvc)
    std::cout << "G4AtlasRunManager: Trouble getting the message service.  Should never happen.  Will crash now." << std::endl;
  m_log = new MsgStream(msgSvc,"G4AtlasRunManager");
  return *m_log;
}

//________________________________________________________________________
iGeant4::G4AtlasRunManager* iGeant4::G4AtlasRunManager::GetG4AtlasRunManager()
{
  static G4AtlasRunManager* thisManager=0;
  if (!thisManager) thisManager=new G4AtlasRunManager;
  return thisManager;
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::InitializeGeometry() 
{
  G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();

  log() << MSG::INFO << "iGeant4::G4AtlasRunManager::InitializeGeometry()" << endreq;
  log() << MSG::INFO << "found lvs-size()==" << lvs->size() << endreq;
  
  G4LogicalVolumeStore::iterator volIt    = lvs->begin();
  G4LogicalVolumeStore::iterator volItEnd = lvs->end();
  for ( ; volIt!=volItEnd; ++volIt) {
    const G4String &volName = (*volIt)->GetName();

    if ( volName == "Muon::MuonSys" ) {
      (*volIt)->SetSmartless( 0.1 );
      log() << MSG::INFO << "Set smartlessness for Muon::MuonSys to 0.1" << endreq;
    } 
    else if ( volName == "LArMgr::LAr::EMB::STAC") {
      (*volIt)->SetSmartless( 0.5 );
      log() << MSG::INFO << "Set smartlessness for LArMgr::LAr::EMB::STAC to 0.5" << endreq;
    }
  }
  
  if (userDetector) 
    G4RunManager::InitializeGeometry();
  else 
    log() << MSG::WARNING << " User Detector not set!!! Geometry NOT initialized!!!" << endreq;
  
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::InitializePhysics() 
{
  log() << MSG::INFO << "iGeant4::G4AtlasRunManager::InitializePhysics()" << endreq;
  kernel->InitializePhysics();
  physicsInitialized = true;

  //std::cout<<"*AS* run init iGeant4::G4AtlasRunManager::InitializePhysics() "<<std::endl;
  //  kernel->RunInitialization();
  RunInitialization();
  //std::cout<<"*AS* run init <<< "<<std::endl;
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::EndEvent() 
{
  log() << MSG::DEBUG << "G4AtlasRunManager::EndEvent" << endreq;
}

//________________________________________________________________________
bool iGeant4::G4AtlasRunManager::ProcessEvent(G4Event* event)
{
  
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);
  
  //G4RegionStore* gs = G4RegionStore::GetInstance();
  //std::cout<<"*AS* G4RegionStore "<< gs->size() <<std::endl;

  //G4LogicalVolumeStore *lvs=G4LogicalVolumeStore::GetInstance();
  //std::cout<<"*AS* G4LogicalVolumeStore "<< lvs->size() <<std::endl;

  /*
  // Release GeoModel Geometry if necessary
  if (m_releaseGeo){
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    StoreGateSvc* detStore;
    StatusCode sc=svcLocator->service("DetectorStore",detStore);
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
  */

  currentEvent = event;
 
  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      log()<<MSG::WARNING<<"G4AtlasRunManager::SimulateFADSEvent: "<<
	"Event Aborted at Detector Simulation level"<<endreq;
      currentEvent=0;
      return true;
    }
 
  // *AS* AnalyzeEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      log()<<MSG::WARNING<<"G4AtlasRunManager::SimulateFADSEvent: "<<
	"Event Aborted at Analysis level"<<endreq;
      currentEvent=0;
      return true;
    }
 
  // stateManager->SetNewState(G4State_GeomClosed);
  StackPreviousEvent(currentEvent);
  bool abort=currentEvent->IsAborted();
  currentEvent = 0;

  // std::cout<<" SimulateFADSEvent : done simulating one event "<<std::endl;
  return abort;
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::RunTermination()
{
  // std::cout<<" this is G4AtlasRunManager::RunTermination() "<<std::endl;
  for (size_t itr=0;itr<previousEvents->size();itr++) { delete (*previousEvents)[itr]; }

  previousEvents->clear();
  
  if (userRunAction) userRunAction->EndOfRunAction(currentRun);
  
  delete currentRun;
  currentRun = 0;
  runIDCounter++;
  
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);
  
  G4GeometryManager::GetInstance()->OpenGeometry();
  
  kernel->RunTermination();
  
  userRunAction=0;
  userEventAction=0;
  userSteppingAction=0;
  userStackingAction=0;
  userTrackingAction=0;
  userDetector=0;
  userPrimaryGeneratorAction=0;
  

}
