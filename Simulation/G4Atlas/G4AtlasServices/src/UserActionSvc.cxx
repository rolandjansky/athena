/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

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


//=============================================================================
// New design for the user action service
//=============================================================================


// Framework includes
#include "CxxUtils/make_unique.h"

// Geant4 includes
#include "G4RunManager.hh"

// Local includes
#include "UserActionSvc.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  // I was using private tool handle arrays, but the OutputLevels weren't
  // getting set correctly (in mig1), so I will just make them all public.
  //---------------------------------------------------------------------------
  UserActionSvc::UserActionSvc(const std::string& name,
                               ISvcLocator* pSvcLocator)
    : AthService(name, pSvcLocator),
      m_beginRunActionTools(),
      m_endRunActionTools(),
      m_beginEventActionTools(),
      m_endEventActionTools(),
      m_stackingActionTools(),
      m_preTrackingActionTools(),
      m_postTrackingActionTools(),
      m_steppingActionTools()
      //m_beginRunActionTools(this),
      //m_endRunActionTools(this),
      //m_beginEventActionTools(this),
      //m_endEventActionTools(this),
      //m_stackingActionTools(this),
      //m_preTrackingActionTools(this),
      //m_postTrackingActionTools(this),
      //m_steppingActionTools(this)
  {
    declareProperty("BeginRunActionTools", m_beginRunActionTools);
    declareProperty("EndRunActionTools", m_endRunActionTools);
    declareProperty("BeginEventActionTools", m_beginEventActionTools);
    declareProperty("EndEventActionTools", m_endEventActionTools);
    declareProperty("StackingActionTools", m_stackingActionTools);
    declareProperty("PreTrackingActionTools", m_preTrackingActionTools);
    declareProperty("PostTrackingActionTools", m_postTrackingActionTools);
    declareProperty("SteppingActionTools", m_steppingActionTools);
  }

  //---------------------------------------------------------------------------
  // Initialize the service
  //---------------------------------------------------------------------------
  StatusCode UserActionSvc::initialize()
  {
    ATH_MSG_INFO("Initializing. user action tools of each type, in order of execution:");

    ATH_MSG_INFO("  begin-run:     " << m_beginRunActionTools.size());
    for(auto action : m_beginRunActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_MSG_INFO("  end-run:       " << m_endRunActionTools.size());
    for(auto action : m_endRunActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_MSG_INFO("  begin-event:   " << m_beginEventActionTools.size());
    for(auto action : m_beginEventActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_MSG_INFO("  end-event:     " << m_endEventActionTools.size());
    for(auto action : m_endEventActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_MSG_INFO("  stacking:      " << m_stackingActionTools.size());
    for(auto action : m_stackingActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_MSG_INFO("  pre-tracking:  " << m_preTrackingActionTools.size());
    for(auto action : m_preTrackingActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_MSG_INFO("  post-tracking: " << m_postTrackingActionTools.size());
    for(auto action : m_postTrackingActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_MSG_INFO("  stepping:      " << m_steppingActionTools.size());
    for(auto action : m_steppingActionTools)
      ATH_MSG_INFO("      -> " << action.name());

    ATH_CHECK( m_beginRunActionTools.retrieve() );
    ATH_CHECK( m_endRunActionTools.retrieve() );
    ATH_CHECK( m_beginEventActionTools.retrieve() );
    ATH_CHECK( m_endEventActionTools.retrieve() );
    ATH_CHECK( m_stackingActionTools.retrieve() );
    ATH_CHECK( m_preTrackingActionTools.retrieve() );
    ATH_CHECK( m_postTrackingActionTools.retrieve() );
    ATH_CHECK( m_steppingActionTools.retrieve() );

   return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Initialize the user actions for the current thread.
  // In this code, "action" refers to the G4 action classes, and "plugin"
  // refers to the custom action objects that get assigned to the G4 action.
  // TODO: this prototype could be cleaner
  //---------------------------------------------------------------------------
  StatusCode UserActionSvc::initializeActions()
  {
    // This method is called concurrently in AthenaMT during initialization.
    // We conservatively just lock the whole thing to protect MsgStream and
    // downstream code.
    static std::mutex userActionMutex;
    std::lock_guard<std::mutex> userActionLock(userActionMutex);

    ATH_MSG_DEBUG("initializeActions");

    // sanity check: this is to make sure there are no other instances of this
    // svc that have been already initialized, or that nobody else in some other
    // code has already been registering actions to the run manager, which would
    // be a major error in the configuration checking the run action is probably
    // enough fo multiple instances of this service, since all roles are usually
    // set at the same time. but other code may as well have registered just one
    // role, so it is safer to do all checks here.

    if( G4RunManager::GetRunManager()->GetUserRunAction() ||
        G4RunManager::GetRunManager()->GetUserEventAction() ||
        G4RunManager::GetRunManager()->GetUserStackingAction() ||
        G4RunManager::GetRunManager()->GetUserTrackingAction() ||
        G4RunManager::GetRunManager()->GetUserSteppingAction() )
    {
      ATH_MSG_FATAL("UserActionSvc has found that actions were already " <<
                    "registered to the G4RunManager. Check your code/configuration");
      return StatusCode::FAILURE;
    }

    // Initialize the ATLAS run action.
    if(m_runActions.get()) {
      ATH_MSG_ERROR("Run action already exists for current thread!");
      return StatusCode::FAILURE;
    }
    auto runAction = CxxUtils::make_unique<G4AtlasRunAction>();
    // Assign begin-run plugins
    for(auto beginRunTool : m_beginRunActionTools)
      runAction->addBeginRunAction( beginRunTool->getBeginRunAction() );
    // Assign end-run plugins
    for(auto endRunTool : m_endRunActionTools)
      runAction->addEndRunAction( endRunTool->getEndRunAction() );

    G4RunManager::GetRunManager()->SetUserAction( runAction.get() );
    m_runActions.set( std::move(runAction) );

    // Initialize the ATLAS event action.
    if(m_eventActions.get()) {
      ATH_MSG_ERROR("Event action already exists for current thread!");
      return StatusCode::FAILURE;
    }
    auto eventAction = CxxUtils::make_unique<G4AtlasEventAction>();
    // Assign begin-event plugins
    for(auto beginEventTool : m_beginEventActionTools)
      eventAction->addBeginEventAction( beginEventTool->getBeginEventAction() );
    // Assign end-event plugins
    for(auto endEventTool : m_endEventActionTools)
      eventAction->addEndEventAction( endEventTool->getEndEventAction() );
    G4RunManager::GetRunManager()->SetUserAction( eventAction.get() );
    m_eventActions.set( std::move(eventAction) );

    // Initialize the ATLAS stacking action.
    if(m_stackingActions.get()) {
      ATH_MSG_ERROR("Stacking action already exists for current thread!");
      return StatusCode::FAILURE;
    }
    auto stackAction = CxxUtils::make_unique<G4AtlasStackingAction>();
    // Assign stacking plugins
    for(auto stackTool : m_stackingActionTools){
      auto stackPlugin = stackTool->getStackingAction();
      ATH_MSG_INFO("stackPlugin " << stackPlugin);
      stackAction->addAction( stackPlugin );
    }
    G4RunManager::GetRunManager()->SetUserAction( stackAction.get() );
    m_stackingActions.set( std::move(stackAction) );

    // Initialize the ATLAS tracking action.
    if(m_trackingActions.get()) {
      ATH_MSG_ERROR("Tracking action already exists for current thread!");
      return StatusCode::FAILURE;
    }
    auto trackAction = CxxUtils::make_unique<G4AtlasTrackingAction>();
    // Assign pre-tracking plugins
    for(auto preTrackTool : m_preTrackingActionTools)
      trackAction->addPreTrackAction( preTrackTool->getPreTrackingAction() );
    // Assign post-tracking plugins
    for(auto postTrackTool : m_postTrackingActionTools)
      trackAction->addPostTrackAction( postTrackTool->getPostTrackingAction() );
    G4RunManager::GetRunManager()->SetUserAction( trackAction.get() );
    m_trackingActions.set( std::move(trackAction) );

    // Initialize the ATLAS stepping action.
    if(m_steppingActions.get()) {
      ATH_MSG_ERROR("Stepping action already exists for current thread!");
      return StatusCode::FAILURE;
    }
    auto stepAction = CxxUtils::make_unique<G4AtlasSteppingAction>();
    // Assign stepping plugins
    for(auto stepTool : m_steppingActionTools)
      stepAction->addAction( stepTool->getSteppingAction() );
    G4RunManager::GetRunManager()->SetUserAction( stepAction.get() );
    m_steppingActions.set( std::move(stepAction) );

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Gaudi interface query
  //---------------------------------------------------------------------------
  StatusCode UserActionSvc::queryInterface(const InterfaceID& riid,
                                           void** ppvInterface)
  {
    if(IUserActionSvc::interfaceID().versionMatch(riid)){
      *ppvInterface = static_cast<IUserActionSvc*>(this);
    }
    else{
      // Interface is not directly available; try out a base class
      return AthService::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
  }

} // namespace G4UA
