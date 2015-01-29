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

#include "G4InitMute.h"

#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IUserAction.h"



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
  public G4UserEventAction, public G4UserTrackingAction, public G4InitUnmute {
  
 public:
  UserActionSvc( const std::string& name, ISvcLocator* svc );//!< Service constructor
  virtual ~UserActionSvc();
 
  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize

  virtual void PreUserTrackingAction(const G4Track*); //!< hook for the g4 run manager
  virtual void PostUserTrackingAction(const G4Track*); //!< hook for the g4 run manager
  virtual void BeginOfEventAction(const G4Event* ); //!< hook for the g4 run manager
  virtual void EndOfEventAction(const G4Event* ); //!< hook for the g4 run manager
  virtual void BeginOfRunAction(const G4Run* ); //!< hook for the g4 run manager
  virtual void EndOfRunAction(const G4Run* ); //!< hook for the g4 run manager
  virtual void UserSteppingAction(const G4Step*); //!< hook for g4 the run manager

 private:


  void RegisterToRunManager();

  // there is no priority handling here. the order in which the actions are executed is the one 
  // in which they are put in the array
  // this assumes some machinery is in place on the python side to allow priority manipulation

  ToolHandleArray<IUserAction> m_BoR; //!< Actions activated for BeginOfRun
  ToolHandleArray<IUserAction> m_EoR; //!< Actions activated for EndOfRun 
  ToolHandleArray<IUserAction> m_BoE; //!< Actions activated for BeginOfEvent 
  ToolHandleArray<IUserAction> m_EoE; //!< Actions activated for EndOfEvent 
  ToolHandleArray<IUserAction> m_Ste; //!< Actions activated for Stepping 
  ToolHandleArray<IUserAction> m_BoT; //!< Actions activated for PreTracking 
  ToolHandleArray<IUserAction> m_EoT; //!< Actions activated for PostTracking

  // these are to remove the overhead due to the toolhandles
  std::vector<IUserAction*> m_BoRquick; //!< Actions activated for BeginOfRun
  std::vector<IUserAction*> m_EoRquick; //!< Actions activated for EndOfRun 
  std::vector<IUserAction*> m_BoEquick; //!< Actions activated for BeginOfEvent 
  std::vector<IUserAction*> m_EoEquick; //!< Actions activated for EndOfEvent 
  std::vector<IUserAction*> m_Stequick; //!< Actions activated for Stepping 
  std::vector<IUserAction*> m_BoTquick; //!< Actions activated for PreTracking 
  std::vector<IUserAction*> m_EoTquick; //!< Actions activated for PostTracking


//  std::multimap< int, ToolHandle<IUserAction> > m_BoR; //!< Actions activated for BeginOfRun
//  std::multimap< int, ToolHandle<IUserAction> > m_EoR; //!< Actions activated for EndOfRun 
//  std::multimap< int, ToolHandle<IUserAction> > m_BoE; //!< Actions activated for BeginOfEvent 
//  std::multimap< int, ToolHandle<IUserAction> > m_EoE; //!< Actions activated for EndOfEvent 
//  std::multimap< int, ToolHandle<IUserAction> > m_Ste; //!< Actions activated for Stepping 
//  std::multimap< int, ToolHandle<IUserAction> > m_BoT; //!< Actions activated for PreTracking 
//  std::multimap< int, ToolHandle<IUserAction> > m_EoT; //!< Actions activated for PostTracking
//
  
};



#endif
