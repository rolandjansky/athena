/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef  G4ATLASSERVICES_USERACTIONSVC_H
#define  G4ATLASSERVICES_USERACTIONSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"

#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UserStackingAction.hh"

#include "G4AtlasInterfaces/G4InitMute.h"

#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IUserAction.h"

#include "G4TrackingManager.hh"

/** @class UserActionSvc UserActionSvc.h "G4AtlasServices/UserActionSvc.h"
 *  
 *  Implementation of an athena service to handle G4 user actions
 *  The class inherits from the G4 action classes, and registers itself
 *  to the run manager.
 *  Athena-specific actions (IUserAction) can be added to the service and will be
 *  hooked to the G4 kernel
 * 
 *  @author Andrea Di Simone
 *  @date   2015-01-12
 */


class UserActionSvc: virtual public IUserActionSvc, public AthService, public G4InitMute, public G4UserRunAction, public G4UserSteppingAction,
  public G4UserEventAction, public G4UserTrackingAction, public G4UserStackingAction {
  
 public:
  UserActionSvc( const std::string& name, ISvcLocator* svc );//!< Service constructor
  virtual ~UserActionSvc();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID() { return IUserActionSvc::interfaceID(); }

  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize

  virtual void PreUserTrackingAction(const G4Track*); //!< hook for the g4 run manager
  virtual void PostUserTrackingAction(const G4Track*); //!< hook for the g4 run manager
  virtual void BeginOfEventAction(const G4Event* ); //!< hook for the g4 run manager
  virtual void EndOfEventAction(const G4Event* ); //!< hook for the g4 run manager
  virtual void BeginOfRunAction(const G4Run* ); //!< hook for the g4 run manager
  virtual void EndOfRunAction(const G4Run* ); //!< hook for the g4 run manager
  virtual void UserSteppingAction(const G4Step*); //!< hook for g4 the run manager

  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack); //!< hook for g4 the run manager
  virtual void PrepareNewEvent();
  virtual void NewStage();

  // some hooks to manage legacy actions from FADS
  void SetLegacyRA(G4UserRunAction* anAction);
  void SetLegacyEA(G4UserEventAction* anAction);
  void SetLegacySA(G4UserSteppingAction* anAction);
  void SetLegacyTA(G4UserTrackingAction* anAction);
  void SetLegacyStaA(G4UserStackingAction* anAction);
  


  // needeed to replicate functionality provided by the old FADS singletons
  virtual void SetTrajectory(G4VTrajectory * aTraj) override {fpTrackingManager->SetStoreTrajectory(true);  fpTrackingManager->SetTrajectory(aTraj);};
  virtual G4TrackingManager* TrackingManager() const override {return fpTrackingManager;}; 
  virtual G4StackManager* StackManager() const {return stackManager;};
  virtual void ResetTrajectory() override {fpTrackingManager->SetStoreTrajectory(false);};
  
  // now the new actions

  // there is no priority handling here. the order in which the actions are executed is the one 
  // in which they are put in the array
  // this assumes some machinery is in place on the python side to allow priority manipulation
  // separate arrays for BeginOf* and EndOf*, to allow flexibility in the priority (i.e. an action
  // can in principle be first in BeginOfRun, second in BeginOfEvent, sixth in EndOfEvent, last in EndOfRun)

 private:

  ToolHandleArray<IUserAction> m_BoR; //!< Actions activated for BeginOfRun
  ToolHandleArray<IUserAction> m_EoR; //!< Actions activated for EndOfRun 
  ToolHandleArray<IUserAction> m_BoE; //!< Actions activated for BeginOfEvent 
  ToolHandleArray<IUserAction> m_EoE; //!< Actions activated for EndOfEvent 
  ToolHandleArray<IUserAction> m_Ste; //!< Actions activated for Stepping 
  ToolHandleArray<IUserAction> m_BoT; //!< Actions activated for PreTracking 
  ToolHandleArray<IUserAction> m_EoT; //!< Actions activated for PostTracking
  ToolHandleArray<IUserAction> m_StaCla; //!< Actions activated for Stacking, classifynewtrack
  ToolHandleArray<IUserAction> m_StaPrepare; //!< Actions activated for Stacking, preparenewevent
  ToolHandleArray<IUserAction> m_StaNewStage; //!< Actions activated for Stacking, newstage
  
  G4UserRunAction* m_legacy_RA;
  G4UserEventAction* m_legacy_EA;
  G4UserTrackingAction* m_legacy_TA;
  G4UserSteppingAction* m_legacy_SA;
  G4UserStackingAction* m_legacy_StaA;

  void registerToRunManager();

  void fillUserActionsList(ToolHandleArray<IUserAction>&, std::vector<IUserAction*>&,G4AtlasUA::Role,std::string);
  void setManagers(std::vector<IUserAction*>& actionlist);

  // these are to remove the overhead due to the toolhandles
  std::vector<IUserAction*> m_BoRquick; //!< Actions activated for BeginOfRun
  std::vector<IUserAction*> m_EoRquick; //!< Actions activated for EndOfRun 
  std::vector<IUserAction*> m_BoEquick; //!< Actions activated for BeginOfEvent 
  std::vector<IUserAction*> m_EoEquick; //!< Actions activated for EndOfEvent 
  std::vector<IUserAction*> m_Stequick; //!< Actions activated for Stepping 
  std::vector<IUserAction*> m_BoTquick; //!< Actions activated for PreTracking 
  std::vector<IUserAction*> m_EoTquick; //!< Actions activated for PostTracking
  std::vector<IUserAction*> m_StaClaquick; //!< Actions activated for PostTracking
  std::vector<IUserAction*> m_StaPreparequick; //!< Actions activated for PostTracking
  std::vector<IUserAction*> m_StaNewStagequick; //!< Actions activated for PostTracking
  
  // ADS: need to wrap a G4UserSteppingAction around a IUserAction to allow regional actions
  class RegionStepping final: public G4UserSteppingAction{
    
  public:
  RegionStepping(IUserAction* ua):G4UserSteppingAction(){m_UA=ua;};
    ~RegionStepping(){;}; // IUserAction must not be destroyed here
    inline void UserSteppingAction(const G4Step* aStep) override {m_UA->Step(aStep);};
  private:
    IUserAction* m_UA;
    
  };

  std::vector<RegionStepping*> m_SteRegion; //!< Actions activated for Stepping only in a particular region

};



#endif
