/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserPhysicsDefinition_H
#define UserPhysicsDefinition_H
// abstract interface to allow users to define their own physics processes
// independently from the physics list in use

#include "G4VUserPhysicsList.hh"

class UserPhysicsDefinition: public G4VUserPhysicsList {
public:
  UserPhysicsDefinition(): G4VUserPhysicsList() {}
  // provide a dummy implementation of ConstructParticle & ConstructProcess
  virtual void ConstructParticle() {}
  virtual void ConstructProcess() {}
  // disable SetCuts, as we don't want cuts to be defined from here
  void SetCuts() {;}
  virtual ~UserPhysicsDefinition() {}
};
#endif
