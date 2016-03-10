/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FactoryMadeList_H
#define FactoryMadeList_H

#include "FadsPhysics/FadsPhysicsList.h"
#include "G4VUserPhysicsList.hh"
#include "G4PhysListFactory.hh"

#include <iostream>
#include <vector>
#include <string>

namespace FADS {

  class FactoryMadeList:public FadsPhysicsList {
   private:
    G4VUserPhysicsList* m_thePhysicsList;
   public:

    G4VUserPhysicsList* GetPhysicsList() override final { return m_thePhysicsList; }

    FactoryMadeList(std::string n): FadsPhysicsList(n) , m_thePhysicsList(0) {}

    ~FactoryMadeList ()
    {
      // the destructor should really get rid of the Physics List.
      // Now that the G4RunManager deletes the Physics List in its
      // destructor, this causes a crash on termination and must be
      // commented out, waiting for a version which will not decide
      // what to do for us (ADA)
      // if (m_thePhysicsList) delete m_thePhysicsList;
    }

    void ConstructParticle()
    {
      if (!m_thePhysicsList) Construct();
      m_thePhysicsList->ConstructParticle();
    }
    void ConstructProcess()
    {
      if (!m_thePhysicsList) Construct();
      m_thePhysicsList->ConstructProcess();
    }
    void SetCuts()
    {
      if (!m_thePhysicsList) Construct();
      m_thePhysicsList->SetCuts();
    }
    void Construct()
    {
      if (!m_thePhysicsList){
        G4PhysListFactory factory;
        if(GetName() != "" && factory.IsReferencePhysList(GetName())){
          m_thePhysicsList = factory.GetReferencePhysList(GetName());
        } 
        if (!m_thePhysicsList) throw "Physics list not found";
      }
      if (m_cutValue>0) m_thePhysicsList->SetDefaultCutValue(m_cutValue);
    }

  };

} // end namespace

#endif // FactoryMadeList_H
