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
  T* m_thePhysicsDefinition;
public:
  UserPhysicsDefinitionProxy(): UserPhysicsDefinition()
  {
    //std::cout<<" Proxy summoned:registering... "<<std::endl;
    Register();
    //std::cout<<" creating the user's process list"<<std::endl;
    m_thePhysicsDefinition=new T;
  }
  void Register()
  {
    FADS::PhysicsListCatalog *grm=FADS::PhysicsListCatalog::GetInstance();
    grm->RegisterPhysicsDefinition(this);
  }
  void CreatePhysicsDefinition() {m_thePhysicsDefinition=new T;}
  void ConstructParticle()
  {
    if (!m_thePhysicsDefinition) CreatePhysicsDefinition();
    m_thePhysicsDefinition->ConstructParticle();
  }
  void ConstructProcess()
  {
    if (!m_thePhysicsDefinition) CreatePhysicsDefinition();
    m_thePhysicsDefinition->ConstructProcess();
  }
};

#endif
