/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "globals.hh"
#include "FadsPhysics/PhysicsListCatalog.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

  PhysicsListCatalog* PhysicsListCatalog::s_thePointer = 0;

  PhysicsListCatalog::PhysicsListCatalog(): m_currentPL(0),m_defaultCutValue(.7*CLHEP::mm)
  {
    m_theSteering = new PhysicsListSteering;
    m_theSteering->m_theList=m_currentPL;
  }

  PhysicsListCatalog* PhysicsListCatalog::GetInstance()
  {
    if (!s_thePointer) s_thePointer=new PhysicsListCatalog;
    return s_thePointer;
  }

  void PhysicsListCatalog::RegisterPhysicsList(FadsPhysicsList *d)
  {
    if (m_thePhysicsLists.find(d->GetName())!=m_thePhysicsLists.end()) {
      std::cout<<"Redefining physics list "<<d->GetName()<<std::endl;
    } else {
      m_thePhysicsLists[d->GetName()]=d;
    }
  }

  FadsPhysicsList* PhysicsListCatalog::GetPhysicsListEntry(std::string name)
  {
    if (m_thePhysicsLists.find(name)==m_thePhysicsLists.end()) {
      std::cout << "Physics list " << name << " not found!" << std::endl;
      return 0;
    } else{
      FadsPhysicsList* df=m_thePhysicsLists[name];
      m_currentPL=df;
      return df;
    }
  }

  void PhysicsListCatalog::PrintAll()
  {
    std::cout << "Printing the list of pre-defined physics lists:" << std::endl;
    std::cout << m_thePhysicsLists.size() << std::endl;
    PLCatalog::const_iterator it;
    for (it=m_thePhysicsLists.begin();it!=m_thePhysicsLists.end();it++) {
      std::cout<<" --- Physics list "<<(*it).first<<std::endl;
    }
  }

} // end namespace
