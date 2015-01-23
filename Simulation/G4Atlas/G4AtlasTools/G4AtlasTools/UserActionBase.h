/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_USERACTIONBASE_H 
#define G4ATLASTOOLS_USERACTIONBASE_H

// Base classes
#include "G4AtlasInterfaces/IUserAction.h"
#include "AthenaBaseComps/AthAlgTool.h"

class UserActionBase : virtual public IUserAction , public AthAlgTool {
 public:
  // Standard constructor and destructor
  UserActionBase(const std::string& type, const std::string& name, const IInterface *parent);
  ~UserActionBase() {};

  // Base class methods from IUserAction


  virtual void BeginOfRun (const G4Run*) override {};
  virtual void EndOfRun (const G4Run*) override {};

  virtual void BeginOfEvent (const G4Event*) override {};
  virtual void EndOfEvent (const G4Event*) override {};

  virtual void Step (const G4Step*) override {};

  virtual void PreTracking (const G4Track*) override {};
  virtual void PostTracking (const G4Track*) override {};

  virtual std::vector< G4AtlasUA::Role> Roles() override {return m_roles;};

  //  virtual  G4AtlasUA::Priority Priority() override {return m_priority;};
  // virtual void SetPriority( G4AtlasUA::Priority pri) override {m_priority=pri;};

  // virtual  G4AtlasUA::Priority Priority(G4AtlasUA::Role theRole) override;
  //virtual void SetPriority(G4AtlasUA::Priority, G4AtlasUA::Role) override;


  virtual void AddRole( G4AtlasUA::Role);

 private:

  std::vector< G4AtlasUA::Role> m_roles; ///!< The roles of this action

  //  G4AtlasUA::Priority m_priority; ///!< The global priority of this action

  //  std::map< G4AtlasUA::Role,  G4AtlasUA::Priority> m_priorities; ///!< placeholder for role-specific priorities


};

#endif
