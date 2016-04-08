/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "UserActionSvc.h"

#include "G4RunManager.hh"
#include "G4RegionStore.hh"

// Constructor
 UserActionSvc::UserActionSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
   AthService(name, pSvcLocator),
   m_BoR(),m_EoR(),m_BoE(),m_EoE(),m_Ste(),m_BoT(),m_EoT(),m_StaCla(),m_StaPrepare(),m_StaNewStage(),
   m_legacy_RA(0),m_legacy_EA(0),m_legacy_TA(0),m_legacy_SA(0),m_legacy_StaA(0){

   declareProperty("BeginOfRunActions",m_BoR);
   declareProperty("EndOfRunActions",m_EoR);
   declareProperty("BeginOfEventActions",m_BoE);
   declareProperty("EndOfEventActions",m_EoE);
   declareProperty("SteppingActions",m_Ste);
   declareProperty("PreTrackingActions",m_BoT);
   declareProperty("PostTrackingActions",m_EoT);
   declareProperty("StackingActionsClassification",m_StaCla);
   declareProperty("StackingActionsPrepareNewEvent",m_StaPrepare);
   declareProperty("StackingActionsNewStage",m_StaNewStage);
 }
 

StatusCode UserActionSvc::initialize(){

  ATH_MSG_INFO( "Initializing UserActionSvc: " << name() );


  ATH_CHECK(m_BoR.retrieve());
  ATH_CHECK(m_EoR.retrieve());

  ATH_CHECK(m_BoE.retrieve());
  ATH_CHECK(m_EoE.retrieve());

  ATH_CHECK(m_Ste.retrieve());

  ATH_CHECK(m_BoT.retrieve());
  ATH_CHECK(m_EoT.retrieve());

  ATH_CHECK(m_StaCla.retrieve());
  ATH_CHECK(m_StaPrepare.retrieve());
  ATH_CHECK(m_StaNewStage.retrieve());

  // cache the pointers to the original interfaces, to remove the toolhandles
  // notify the actions of the roles they were assigned and of the order of execution

  fillUserActionsList(m_BoR,m_BoRquick,G4AtlasUA::BeginOfRun,"BeginOfRun");
  fillUserActionsList(m_BoE,m_BoEquick,G4AtlasUA::BeginOfEvent,"BeginOfEvent");
  fillUserActionsList(m_BoT,m_BoTquick,G4AtlasUA::PreTracking,"PreUserTracking");
  fillUserActionsList(m_Ste,m_Stequick,G4AtlasUA::Step,"Stepping");
  fillUserActionsList(m_EoT,m_EoTquick,G4AtlasUA::PostTracking,"PostUserTracking");
  fillUserActionsList(m_StaCla,m_StaClaquick,G4AtlasUA::Classification,"Classification");
  fillUserActionsList(m_StaPrepare,m_StaPreparequick,G4AtlasUA::PrepareNewEvent,"PrepareNewEvent");
  fillUserActionsList(m_StaNewStage,m_StaNewStagequick,G4AtlasUA::NewStage,"NewStage");
  fillUserActionsList(m_EoE,m_EoEquick,G4AtlasUA::EndOfEvent,"EndOfEvent");
  fillUserActionsList(m_EoR,m_EoRquick,G4AtlasUA::EndOfRun,"EndOfRun");



  registerToRunManager();

  return StatusCode::SUCCESS;

}


void UserActionSvc::setManagers(std::vector<IUserAction*>& actionlist){

  for(auto action: actionlist)
    action->setManagers(fpEventManager, fpTrackingManager, stackManager,fpSteppingManager);
}

void UserActionSvc::fillUserActionsList(ToolHandleArray<IUserAction>& thlist, std::vector<IUserAction*>& actionlist,G4AtlasUA::Role role,std::string tag){

  ATH_MSG_INFO("List of declared "<<tag<<" actions (in order of execution):");

  int order=0;
  for(auto action: thlist){
    // add stepping actions to global list only if no specific region is declared
    if(action->GetRegions().size()==0 || role!=G4AtlasUA::Step){
      ATH_MSG_INFO("   ->"<<action->name());//<<" with properties");
      actionlist.push_back(&(*action));
      actionlist.back()->AddRole(role,order);
      ++order;
    } else {
      ATH_MSG_INFO("   ->"<<action->name()<< " [Regional: will be assigned to "<<action->GetRegions().size()<<" regions at begin of run]");
    }
  }
  
}


void UserActionSvc::registerToRunManager(){

  ATH_MSG_INFO( "Registering actions to G4 run manager ");

  G4RunManager* manager=G4RunManager::GetRunManager();
  
  manager->SetUserAction((G4UserRunAction*)this);
  manager->SetUserAction((G4UserEventAction*)this);
  manager->SetUserAction((G4UserTrackingAction*)this);
  manager->SetUserAction((G4UserSteppingAction*)this);
  manager->SetUserAction((G4UserStackingAction*)this);

  // at this point the internal pointers to the managers have been filled by G4. we pass them to the actual user actions
  
  setManagers(m_BoRquick);
  setManagers(m_BoEquick);
  setManagers(m_BoTquick);
  setManagers(m_Stequick);
  setManagers(m_EoTquick);
  setManagers(m_StaClaquick);
  setManagers(m_StaPreparequick);
  setManagers(m_StaNewStagequick);
  setManagers(m_EoEquick);
  setManagers(m_EoRquick);


}

StatusCode UserActionSvc::finalize(){
  
  ATH_MSG_INFO( "Finalizing UserActionSvc: " << name() );


  return StatusCode::SUCCESS;

}

 // Destructor
 UserActionSvc::~UserActionSvc(){
   //std::cout<<"in UASvc dtor"<<std::endl;
   delete m_legacy_RA;
   delete m_legacy_EA;
   delete m_legacy_TA;
   delete m_legacy_SA;
   delete m_legacy_StaA;
   // delete the wrappers for the regional stepping actions
   for(auto regAction: m_SteRegion)
     delete regAction;
 }


void UserActionSvc::SetLegacyRA(G4UserRunAction* anAction){

  m_legacy_RA=anAction;

}

void UserActionSvc::SetLegacyEA(G4UserEventAction* anAction){

  m_legacy_EA=anAction;

}

void UserActionSvc::SetLegacySA(G4UserSteppingAction* anAction){

  m_legacy_SA=anAction;

}

void UserActionSvc::SetLegacyTA(G4UserTrackingAction* anAction){

  m_legacy_TA=anAction;

}

void UserActionSvc::SetLegacyStaA(G4UserStackingAction* anAction){

  m_legacy_StaA=anAction;

}




void  UserActionSvc::PreUserTrackingAction(const G4Track* aTrack){

  // first execute the legacy actions

  if(m_legacy_TA) m_legacy_TA->PreUserTrackingAction(aTrack);
  
  // now execute the new actions
  for(auto action: m_BoTquick) 
    action->PreTracking(aTrack);
  
}

void  UserActionSvc::PostUserTrackingAction(const G4Track* aTrack){

  // first execute the legacy actions

   if(m_legacy_TA) m_legacy_TA->PostUserTrackingAction(aTrack);

  // now execute the new actions

  for(auto action: m_EoTquick)    
    action->PostTracking(aTrack);
  
}
void  UserActionSvc::BeginOfEventAction(const G4Event* anEvent){

  // first execute the legacy actions

   if(m_legacy_EA) m_legacy_EA->BeginOfEventAction(anEvent);

  // now execute the new actions

  for(auto action: m_BoEquick)
    action->BeginOfEvent(anEvent);
}

void  UserActionSvc::EndOfEventAction(const G4Event* anEvent){
  
  // first execute the legacy actions
  
   if(m_legacy_EA) m_legacy_EA->EndOfEventAction(anEvent);

  // now execute the new actions

  for(auto action: m_EoEquick)
    action->EndOfEvent(anEvent);
  
}

void  UserActionSvc::BeginOfRunAction(const G4Run* aRun ){

  for(auto action: m_Ste){
    for(auto region: action->GetRegions()){
      G4Region* pRegion = G4RegionStore::GetInstance()->GetRegion(region);
      if(!pRegion){
	ATH_MSG_ERROR( "Region "<<region<<" requested by "<<action->name()<<" not found");
	ATH_MSG_ERROR( "Job will go on, but the action will have no effect");
      } else{
	ATH_MSG_INFO("Assigning stepping action "<<action->name()<<" to region "<<region);
	// note that the region does not take ownership of the action
	RegionStepping* steregion=new RegionStepping(&(*action));
	pRegion->SetRegionalSteppingAction(steregion);
	// need to keep the pointers to delete them
	m_SteRegion.push_back(steregion);
	
      }
    }
      
  }
  
  // first execute the legacy actions
  
  if(m_legacy_RA) m_legacy_RA->BeginOfRunAction(aRun);

  // now execute the new actions

  for(auto action: m_BoRquick){
    action->BeginOfRun(aRun);
  }

}

void  UserActionSvc::EndOfRunAction(const G4Run* aRun){

  // first execute the legacy actions

   if(m_legacy_RA) m_legacy_RA->EndOfRunAction(aRun);

  // now execute the new actions
  for(auto action: m_EoRquick)
    action->EndOfRun(aRun);

}

void  UserActionSvc::UserSteppingAction(const G4Step* aStep){

  // first execute the legacy actions
  
   if(m_legacy_SA) m_legacy_SA->UserSteppingAction(aStep);

  // now execute the new actions
  for(auto action: m_Stequick)
    action->Step(aStep);

} 

 G4ClassificationOfNewTrack  UserActionSvc::ClassifyNewTrack(const G4Track* aTrack){

   if(m_legacy_StaA)  m_legacy_StaA->ClassifyNewTrack(aTrack);

   G4ClassificationOfNewTrack classification(fUrgent);
   for(auto action: m_StaClaquick)
    {
      classification = action->ClassifyNewTrack(aTrack);
      if(fKill==classification) { return fKill; }
    }
   // FIXME need to find a better way to handle conflicting classifications... See ATLASSIM-2421
  return classification;
 }

void  UserActionSvc::NewStage(){
  
  // first execute the legacy actions
  
  if(m_legacy_StaA) m_legacy_StaA->NewStage();
  
  // now execute the new actions
  for(auto action: m_StaNewStagequick)
    action->NewStage();
  
  
}

void  UserActionSvc::PrepareNewEvent(){
  
  // first execute the legacy actions
  
  if(m_legacy_StaA) m_legacy_StaA->PrepareNewEvent();
  
  // now execute the new actions
  for(auto action: m_StaPreparequick)
    action->PrepareNewEvent();
  
}



StatusCode UserActionSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IUserActionSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserActionSvc*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }

  return StatusCode::SUCCESS;
}
