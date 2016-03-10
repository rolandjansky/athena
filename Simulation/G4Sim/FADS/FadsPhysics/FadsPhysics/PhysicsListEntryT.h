/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListEntryT_H
#define PhysicsListEntryT_H

#include "FadsPhysics/FadsPhysicsList.h"
#include "G4VUserPhysicsList.hh"

#include <iostream>
#include <vector>
#include <string>

namespace FADS {

  template <class T> class PhysicsListEntryT:public FadsPhysicsList {
  private:
    G4VUserPhysicsList* m_thePhysicsList;
  public:
    G4VUserPhysicsList* GetPhysicsList()
    {
      return m_thePhysicsList;
    }
    PhysicsListEntryT<T>(std::string n): FadsPhysicsList(n)
    {
      // cout<<" this is the templated c-tor"<<endl;
      m_thePhysicsList=0;
    }
    ~PhysicsListEntryT ()
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
      if (!m_thePhysicsList) m_thePhysicsList=new T;
      if (m_cutValue>0) m_thePhysicsList->SetDefaultCutValue(m_cutValue);
      // cout<<"this is construct"<<endl;
      // m_thePhysicsList->Construct();
    }

  };

} // end namespace

#endif // PhysicsListEntryT_H
