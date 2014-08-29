/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "globals.hh"
#include "FadsPhysics/PhysicsListCatalog.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace FADS {

  PhysicsListCatalog* PhysicsListCatalog::thePointer = 0;

  PhysicsListCatalog::PhysicsListCatalog(): currentPL(0),defaultCutValue(.7*CLHEP::mm)
  {
    theSteering = new PhysicsListSteering;
    theSteering->theList=currentPL;
  }

  PhysicsListCatalog* PhysicsListCatalog::GetInstance()
  {
    if (!thePointer) thePointer=new PhysicsListCatalog;
    return thePointer;
  }

  void PhysicsListCatalog::RegisterPhysicsList(FadsPhysicsList *d)
  {
    if (thePhysicsLists.find(d->GetName())!=thePhysicsLists.end()) {
      std::cout<<"Redefining physics list "<<d->GetName()<<std::endl;
    } else {
      thePhysicsLists[d->GetName()]=d;
    }
  }

  FadsPhysicsList* PhysicsListCatalog::GetPhysicsListEntry(std::string name)
  {
    if (thePhysicsLists.find(name)==thePhysicsLists.end()) {
      std::cout << "Physics list " << name << " not found!" << std::endl;
      return 0;
    } else{
      FadsPhysicsList* df=thePhysicsLists[name];
      currentPL=df;
      return df;
    }
  }

  void PhysicsListCatalog::PrintAll()
  {
    std::cout << "Printing the list of pre-defined physics lists:" << std::endl;
    std::cout << thePhysicsLists.size() << std::endl;
    PLCatalog::const_iterator it;
    for (it=thePhysicsLists.begin();it!=thePhysicsLists.end();it++) {
      std::cout<<" --- Physics list "<<(*it).first<<std::endl;
    }
  }

} // end namespace
