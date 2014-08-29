/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UserPhysicsDefinitionProxy_H
#define UserPhysicsDefinitionProxy_H

#include "FadsPhysics/PhysicsListCatalog.h"
#include "FadsPhysics/UserPhysicsDefinition.h"

#include <iostream>

template <class T>
class UserPhysicsDefinitionProxy: public UserPhysicsDefinition {
private:
  T* thePhysicsDefinition;
public:
  UserPhysicsDefinitionProxy(): UserPhysicsDefinition()
  {
    std::cout<<" Proxy summoned:registering... "<<std::endl;
    Register();
    std::cout<<" creating the user's process list"<<std::endl;
    thePhysicsDefinition=new T;
  }
  void Register()
  {
    FADS::PhysicsListCatalog *grm=FADS::PhysicsListCatalog::GetInstance();
    grm->RegisterPhysicsDefinition(this);
  }
  void CreatePhysicsDefinition() {thePhysicsDefinition=new T;}
  void ConstructParticle()
  {
    if (!thePhysicsDefinition) CreatePhysicsDefinition();
    thePhysicsDefinition->ConstructParticle();
  }
  void ConstructProcess()
  {
    if (!thePhysicsDefinition) CreatePhysicsDefinition();
    thePhysicsDefinition->ConstructProcess();
  }
};

#endif
