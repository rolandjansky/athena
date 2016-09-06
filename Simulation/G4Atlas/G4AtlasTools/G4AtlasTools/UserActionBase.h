/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_USERACTIONBASE_H
#define G4ATLASTOOLS_USERACTIONBASE_H

// Base classes
#include "G4AtlasInterfaces/IUserAction.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <stdexcept>
#include <iostream>

#include "G4EventManager.hh"
#include "G4TrackingManager.hh"
#include "G4StackManager.hh"
#include "G4SteppingManager.hh"

class UserActionBase : virtual public IUserAction , public AthAlgTool {
 public:
  // Standard constructor and destructor
  UserActionBase(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~UserActionBase() {};//std::cout<<"in UAB dtor"<<std::endl;};

  // Base class methods from IUserAction

  virtual void BeginOfRun (const G4Run*) override {throw std::runtime_error("Action "+name()+"is scheduled for BeginOfRun, but no implementation is provided. Check your configuration.");};
  virtual void EndOfRun (const G4Run*) override {throw std::runtime_error("Action "+name()+" is scheduled for EndOfRun, but no implementation is provided. Check your configuration.");};

  virtual void BeginOfEvent (const G4Event*) override {throw std::runtime_error("Action "+name()+" is scheduled for BeginOfEvent, but no implementation is provided. Check your configuration.");};
  virtual void EndOfEvent (const G4Event*) override {throw std::runtime_error("Action "+name()+" is scheduled for EndOfEvent, but no implementation is provided. Check your configuration.");};

  virtual void Step (const G4Step*) override {throw std::runtime_error("Action "+name()+" is scheduled for Stepping, but no implementation is provided. Check your configuration.");};

  virtual void PreTracking (const G4Track*) override {throw std::runtime_error("Action "+name()+" is scheduled for PreTracking, but no implementation is provided. Check your configuration.");};
  virtual void PostTracking (const G4Track*) override {throw std::runtime_error("Action "+name()+" is scheduled for PostTracking, but no implementation is provided. Check your configuration.");};

  /** interface for the stacking action */
  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*) override {return fUrgent;};
  virtual void NewStage() override {throw std::runtime_error("Action "+name()+" is scheduled for NewStage, but no implementation is provided. Check your configuration.");};
  virtual void PrepareNewEvent() override {throw std::runtime_error("Action "+name()+" is scheduled for PrepareNewEvent, but no implementation is provided. Check your configuration.");};

  //  virtual std::vector< G4AtlasUA::Role> Roles() override {};

  virtual void AddRole( G4AtlasUA::Role, unsigned int order ) override final;
  virtual void printRoles() override final;

  virtual std::vector<std::string > GetRegions() override final {return m_regions;};

  virtual StatusCode queryInterface(const InterfaceID&, void**);

  virtual void setManagers(G4EventManager* em, G4TrackingManager* tm, G4StackManager* stam, G4SteppingManager* stem){
    m_fpEventManager=em;
    m_stackManager=stam;
    m_fpSteppingManager=stem;
    m_fpTrackingManager=tm;
  };

 protected:

  // private (for dervived classes) pointers to main G4 managers
  // these are passed by the UserActionService after registration of the actions to the G4RunManager
  G4EventManager* m_fpEventManager;
  G4StackManager* m_stackManager;
  G4SteppingManager* m_fpSteppingManager;
  G4TrackingManager* m_fpTrackingManager;

 private:

  std::map< G4AtlasUA::Role, unsigned int> m_roles; ///!< The roles of this action
  std::vector<std::string> m_regions; ///!< stepping actions can be activated only in some specific region

};




#endif
