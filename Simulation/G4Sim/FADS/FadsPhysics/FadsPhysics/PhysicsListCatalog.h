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
    PhysicsListSteering* m_theSteering;
    PLCatalog m_thePhysicsLists;
    static PhysicsListCatalog* s_thePointer;
    inline PhysicsListCatalog();
    FadsPhysicsList *m_currentPL;
    double m_defaultCutValue;
  public:
    bool IsCutValueSet() {return m_defaultCutValue>0;}
    void SetDefaultCutValue(double c) {m_defaultCutValue=c;}
    double GetDefaultCutValue() {return m_defaultCutValue;}
    void SetCurrentPhysicsList(FadsPhysicsList *pl)
    {
      m_currentPL=pl;
      if (m_defaultCutValue>0) m_currentPL->SetCutValue(m_defaultCutValue);
    }
    FadsPhysicsList *GetCurrentPhysicsList() {return m_currentPL;}
    static PhysicsListCatalog* GetInstance();
    void RegisterPhysicsList(FadsPhysicsList *d);
    FadsPhysicsList *GetPhysicsListEntry(std::string name);
    void PrintAll();
    G4VUserPhysicsList* GetMainPhysicsList() {return m_theSteering;}
    void RegisterPhysicsDefinition(UserPhysicsDefinition *p)
    {
      m_theSteering->m_userPhysics.push_back(p);
    }
  };

} // end namespace

#endif // PhysicsListCatalog
