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

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override final;
  static const InterfaceID& interfaceID() { return IUserActionSvc::interfaceID(); }

  virtual StatusCode initialize() override final;          //!< Service init
  virtual StatusCode finalize() override final;            //!< Service finalize

  virtual void PreUserTrackingAction(const G4Track*) override final; //!< hook for the g4 run manager
  virtual void PostUserTrackingAction(const G4Track*) override final; //!< hook for the g4 run manager
  virtual void BeginOfEventAction(const G4Event* ) override final; //!< hook for the g4 run manager
  virtual void EndOfEventAction(const G4Event* ) override final; //!< hook for the g4 run manager
  virtual void BeginOfRunAction(const G4Run* ) override final; //!< hook for the g4 run manager
  virtual void EndOfRunAction(const G4Run* ) override final; //!< hook for the g4 run manager
  virtual void UserSteppingAction(const G4Step*) override final; //!< hook for g4 the run manager

  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack) override final; //!< hook for g4 the run manager
  virtual void PrepareNewEvent() override final;
  virtual void NewStage() override final;

  // some hooks to manage legacy actions from FADS
  void SetLegacyRA(G4UserRunAction* anAction) override final;
  void SetLegacyEA(G4UserEventAction* anAction) override final;
  void SetLegacySA(G4UserSteppingAction* anAction) override final;
  void SetLegacyTA(G4UserTrackingAction* anAction) override final;
  void SetLegacyStaA(G4UserStackingAction* anAction) override final;
  


  // needeed to replicate functionality provided by the old FADS singletons
  virtual void SetTrajectory(G4VTrajectory * aTraj) override {fpTrackingManager->SetStoreTrajectory(true);  fpTrackingManager->SetTrajectory(aTraj);};
  virtual G4TrackingManager* TrackingManager() const override {return fpTrackingManager;}; 
  virtual G4StackManager* StackManager() const override final {return stackManager;};
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


#ifndef G4ATLASSERVICES__G4UA_USERACTIONSVC_H
#define G4ATLASSERVICES__G4UA_USERACTIONSVC_H


// System includes
#include <thread>

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"

// Local includes
#include "G4AtlasRunAction.h"
#include "G4AtlasEventAction.h"
#include "G4AtlasStackingAction.h"
#include "G4AtlasTrackingAction.h"
#include "G4AtlasSteppingAction.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IStackingActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"

#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasTools/ThreadActionHolder.h"


namespace G4UA
{

  /// @class UserActionSvc
  /// @brief A service which manages the user actions for G4 simulation
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class UserActionSvc : public AthService,
                        public virtual IUserActionSvc
  {

    public:

      /// Standard constructor
      UserActionSvc(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the service
      StatusCode initialize() override;

      /// Initialize the user actions for the current thread
      StatusCode initializeActions() override final;

      /// Gaudi interface query
      virtual StatusCode queryInterface(const InterfaceID& riid,
                                        void** ppvInterface);

    private:

      /// @name Handles to ATLAS action tools
      /// @{

      /// Begin-run action tools
      ToolHandleArray<IBeginRunActionTool> m_beginRunActionTools;
      /// End-run action tools
      ToolHandleArray<IEndRunActionTool> m_endRunActionTools;
      /// Begin-event action tools
      ToolHandleArray<IBeginEventActionTool> m_beginEventActionTools;
      /// End-event action tools
      ToolHandleArray<IEndEventActionTool> m_endEventActionTools;
      /// Stacking action tools
      ToolHandleArray<IStackingActionTool> m_stackingActionTools;
      /// Pre-tracking action tools
      ToolHandleArray<IPreTrackingActionTool> m_preTrackingActionTools;
      /// Post-tracking action tools
      ToolHandleArray<IPostTrackingActionTool> m_postTrackingActionTools;
      /// Stepping action tools
      ToolHandleArray<ISteppingActionTool> m_steppingActionTools;

      /// @}

      /// @name ATLAS Geant4 user actions
      /// @{

      /// Thread-local run action
      ThreadActionHolder<G4AtlasRunAction> m_runActions;
      /// Thread-local event action
      ThreadActionHolder<G4AtlasEventAction> m_eventActions;
      /// Thread-local stacking action
      ThreadActionHolder<G4AtlasStackingAction> m_stackingActions;
      /// Thread-local tracking action
      ThreadActionHolder<G4AtlasTrackingAction> m_trackingActions;
      /// Thread-local stepping action
      ThreadActionHolder<G4AtlasSteppingAction> m_steppingActions;

      /// @}

  }; // class UserActionSvc

}

#endif
