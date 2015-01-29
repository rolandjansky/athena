/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "UserActionSvc.h"

#include "G4RunManager.hh"

// Constructor
 UserActionSvc::UserActionSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
   AthService(name, pSvcLocator){

   declareProperty("BeginOfRunActions",m_BoR);
   declareProperty("EndOfRunActions",m_EoR);
   declareProperty("BeginOfEventActions",m_BoE);
   declareProperty("EndOfEventActions",m_EoE);
   declareProperty("SteppingActions",m_Ste);
   declareProperty("PreTrackingAction",m_BoT);
   declareProperty("PostTrackingAction",m_EoT);
 }
 

StatusCode UserActionSvc::initialize(){

  ATH_CHECK(m_BoR.retrieve());
  ATH_CHECK(m_EoR.retrieve());

  ATH_CHECK(m_BoE.retrieve());
  ATH_CHECK(m_EoE.retrieve());

  ATH_CHECK(m_Ste.retrieve());

  ATH_CHECK(m_BoT.retrieve());
  ATH_CHECK(m_EoT.retrieve());

  // cache the pointers to the original interfaces, to remove the toolhandles
  for(auto action: m_BoT) m_BoTquick.push_back(&(*action));
  for(auto action: m_EoT) m_EoTquick.push_back(&(*action));

  for(auto action: m_BoR) m_BoRquick.push_back(&(*action));
  for(auto action: m_EoR) m_EoRquick.push_back(&(*action));

  for(auto action: m_BoE) m_BoEquick.push_back(&(*action));
  for(auto action: m_EoE) m_EoEquick.push_back(&(*action));
  
  for(auto action: m_Ste) m_Stequick.push_back(&(*action));

  // FIXME: do we want to do this here, or leave it to some higher-level initialization manager?
  RegisterToRunManager();

  return StatusCode::SUCCESS;

}


void UserActionSvc::RegisterToRunManager(){

  G4RunManager* manager=G4RunManager::GetRunManager();
  
  manager->SetUserAction((G4UserRunAction*)this);
  manager->SetUserAction((G4UserEventAction*)this);
  manager->SetUserAction((G4UserTrackingAction*)this);
  manager->SetUserAction((G4UserSteppingAction*)this);

}

StatusCode UserActionSvc::finalize(){
  
  return StatusCode::SUCCESS;

}

 // Destructor
 UserActionSvc::~UserActionSvc(){
 
 }

void  UserActionSvc::PreUserTrackingAction(const G4Track* aTrack){

  for(auto action: m_BoTquick) 
    action->PreTracking(aTrack);
  
}

void  UserActionSvc::PostUserTrackingAction(const G4Track* aTrack){

  for(auto action: m_EoTquick)    
    action->PostTracking(aTrack);
  
}
void  UserActionSvc::BeginOfEventAction(const G4Event* anEvent){

  for(auto action: m_BoEquick)
    action->BeginOfEvent(anEvent);
}

void  UserActionSvc::EndOfEventAction(const G4Event* anEvent){
  
  for(auto action: m_EoEquick)
    action->EndOfEvent(anEvent);
  
}

void  UserActionSvc::BeginOfRunAction(const G4Run* aRun ){

  for(auto action: m_BoRquick)
    action->BeginOfRun(aRun);

}

void  UserActionSvc::EndOfRunAction(const G4Run* aRun){

  for(auto action: m_EoRquick)
    action->EndOfRun(aRun);

}

void  UserActionSvc::UserSteppingAction(const G4Step* aStep){

  for(auto action: m_Stequick)
    action->Step(aStep);

} 
