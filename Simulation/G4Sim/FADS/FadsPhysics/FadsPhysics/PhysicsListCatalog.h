/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListCatalog_H
#define PhysicsListCatalog_H

#include <map>
#include <string>
#include "FadsPhysics/FadsPhysicsList.h"
#include "FadsPhysics/PhysicsListSteering.h"
#include "FadsPhysics/UserPhysicsDefinition.h"

namespace FADS {

  typedef std::map<std::string,FadsPhysicsList *,std::less<std::string> > PLCatalog;

  class PhysicsListCatalog {
    friend class PhysicsListSteering;
  private:
    PhysicsListSteering* theSteering;
    PLCatalog thePhysicsLists;
    static PhysicsListCatalog* thePointer;
    inline PhysicsListCatalog();
    FadsPhysicsList *currentPL;
    double defaultCutValue;
  public:
    bool IsCutValueSet() {return defaultCutValue>0;}
    void SetDefaultCutValue(double c) {defaultCutValue=c;}
    double GetDefaultCutValue() {return defaultCutValue;}
    void SetCurrentPhysicsList(FadsPhysicsList *pl)
    {
      currentPL=pl;
      if (defaultCutValue>0) currentPL->SetCutValue(defaultCutValue);
    }
    FadsPhysicsList *GetCurrentPhysicsList() {return currentPL;}
    static PhysicsListCatalog* GetInstance();
    void RegisterPhysicsList(FadsPhysicsList *d);
    FadsPhysicsList *GetPhysicsListEntry(std::string name);
    void PrintAll();
    G4VUserPhysicsList* GetMainPhysicsList() {return theSteering;}
    void RegisterPhysicsDefinition(UserPhysicsDefinition *p)
    {
      theSteering->userPhysics.push_back(p);
    }
  };

} // end namespace

#endif // PhysicsListCatalog
