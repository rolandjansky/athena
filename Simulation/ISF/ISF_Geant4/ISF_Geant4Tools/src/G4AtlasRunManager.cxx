/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_Geant4Tools/G4AtlasRunManager.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
//#include "ISF_Geant4UserActions/TrackProcessor.h"

#include "G4LogicalVolumeStore.hh"
#include "G4StateManager.hh"
#include "G4GeometryManager.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4RegionStore.hh"
#include "G4Version.hh"

//________________________________________________________________________
iGeant4::G4AtlasRunManager::G4AtlasRunManager()
  : G4RunManager(),
    //  m_pl(0),
    //  m_trackProcessorUserAction(0),
    m_releaseGeo(false),
    m_msg("G4AtlasRunManager"),
    m_senDetTool("SensitiveDetectorMasterTool"),
    m_fastSimTool("FastSimulationMasterTool")
{
  //m_trackProcessorUserAction=new iGeant4::TrackProcessor("ISG_G4TrackProcessor");
  //std::cout<<"iGeant4::G4AtlasRunManager::G4AtlasRunManager() "<<this<<std::endl;
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

  ATH_MSG_INFO( "iGeant4::G4AtlasRunManager::InitializeGeometry()" );
  ATH_MSG_INFO( "found lvs-size()==" << lvs->size() );

  G4LogicalVolumeStore::iterator volIt    = lvs->begin();
  G4LogicalVolumeStore::iterator volItEnd = lvs->end();
  for ( ; volIt!=volItEnd; ++volIt)
    {
      const G4String &volName = (*volIt)->GetName();

      if ( volName == "Muon::MuonSys" )
        {
          (*volIt)->SetSmartless( 0.1 );
          ATH_MSG_INFO( "Set smartlessness for Muon::MuonSys to 0.1" );
        }
      else if ( volName == "LArMgr::LAr::EMB::STAC")
        {
          (*volIt)->SetSmartless( 0.5 );
          ATH_MSG_INFO( "Set smartlessness for LArMgr::LAr::EMB::STAC to 0.5" );
        }
    }

  if (userDetector)
    {
      G4RunManager::InitializeGeometry();
    }
  else
    {
      ATH_MSG_WARNING( " User Detector not set!!! Geometry NOT initialized!!!" );
    }

  if (m_senDetTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the SD master tool" );
      G4ExceptionDescription description;
      description << "InitializeGeometry: Failed to retrieve ISensitiveDetectorMasterTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveSDMaster", FatalException, description);
      abort(); // to keep Coverity happy
    }
  if(m_senDetTool->initializeSDs().isFailure())
    {
      G4ExceptionDescription description;
      description << "InitializeGeometry: Call to ISensitiveDetectorMasterTool::initializeSDs failed.";
      G4Exception("G4AtlasRunManager", "FailedToInitializeSDs", FatalException, description);
      abort(); // to keep Coverity happy
    }
  return;
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::EndEvent()
{
  ATH_MSG_DEBUG( "G4AtlasRunManager::EndEvent" );
}

//________________________________________________________________________
void iGeant4::G4AtlasRunManager::InitializePhysics()
{
  ATH_MSG_INFO( "iGeant4::G4AtlasRunManager::InitializePhysics()" );
  kernel->InitializePhysics();
  physicsInitialized = true;

  // Fast simulations last
  if (m_fastSimTool.retrieve().isFailure())
    {
      ATH_MSG_ERROR ( "Could not retrieve the FastSim master tool" );
      G4ExceptionDescription description;
      description << "InitializePhysics: Failed to retrieve IFastSimulationMasterTool.";
      G4Exception("G4AtlasRunManager", "CouldNotRetrieveFastSimMaster", FatalException, description);
      abort(); // to keep Coverity happy
    }
  if(m_fastSimTool->initializeFastSims().isFailure())
    {
      G4ExceptionDescription description;
      description << "InitializePhysics: Call to IFastSimulationMasterTool::initializeFastSims failed.";
      G4Exception("G4AtlasRunManager", "FailedToInitializeFastSims", FatalException, description);
      abort(); // to keep Coverity happy
    }
  //std::cout<<"*AS* run init iGeant4::G4AtlasRunManager::InitializePhysics() "<<std::endl;
  //  kernel->RunInitialization();
  RunInitialization();
  //std::cout<<"*AS* run init <<< "<<std::endl;
  return;
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
  ATH_MSG_ERROR ( "G4AtlasRunManager could not access the detector store - PANIC!!!!" );
  abort();
  }

  IGeoModelSvc* geoModel = 0;
  StatusCode status=svcLocator->service("GeoModelSvc",geoModel);
  if(status.isFailure()) {
  ATH_MSG_WARNING (  " ----> Unable to retrieve GeoModelSvc" );
  } else {
  status = geoModel->clear();
  if(status.isFailure()) {
  ATH_MSG_WARNING (  " ----> GeoModelSvc::clear() failed" );
  } else {
  ATH_MSG_INFO ( " ----> GeoModelSvc::clear() succeeded " );
  }
  }
  m_releaseGeo=false; // Don't do that again...
  }
  */

  currentEvent = event;

  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      ATH_MSG_WARNING( "G4AtlasRunManager::SimulateFADSEvent: Event Aborted at Detector Simulation level" );
      currentEvent=0;
      return true;
    }

  // *AS* AnalyzeEvent(currentEvent);
  if (currentEvent->IsAborted())
    {
      ATH_MSG_WARNING( "G4AtlasRunManager::SimulateFADSEvent: Event Aborted at Analysis level" );
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
#if G4VERSION_NUMBER < 1010
  for (size_t itr=0;itr<previousEvents->size();itr++) { delete (*previousEvents)[itr]; }
#else
  this->CleanUpPreviousEvents();
#endif
  previousEvents->clear();

  if (userRunAction) { userRunAction->EndOfRunAction(currentRun); }

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
