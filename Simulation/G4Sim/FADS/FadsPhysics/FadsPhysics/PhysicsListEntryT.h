/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListEntryT_H
#define PhysicsListEntryT_H

#include "FadsPhysics/FadsPhysicsList.h"
#include "G4VUserPhysicsList.hh"

#ifdef HAVE_NEW_IOSTREAMS
#include <iostream>
#else
#include <iostream.h>
#endif

#include <vector>
#include <string>

namespace FADS {

  template <class T> class PhysicsListEntryT:public FadsPhysicsList {
  private:
    G4VUserPhysicsList* thePhysicsList;
  public:
    G4VUserPhysicsList* GetPhysicsList()
    {
      return thePhysicsList;
    }
    PhysicsListEntryT<T>(std::string n): FadsPhysicsList(n)
    {
      // cout<<" this is the templated c-tor"<<endl;
      thePhysicsList=0;
    }
    ~PhysicsListEntryT ()
    {
      // the destructor should really get rid of the Physics List.
      // Now that the G4RunManager deletes the Physics List in its
      // destructor, this causes a crash on termination and must be
      // commented out, waiting for a version which will not decide
      // what to do for us (ADA)
      // if (thePhysicsList) delete thePhysicsList;
    }
    void ConstructParticle()
    {
      if (!thePhysicsList) Construct();
      thePhysicsList->ConstructParticle();
    }
    void ConstructProcess()
    {
      if (!thePhysicsList) Construct();
      thePhysicsList->ConstructProcess();
    }
    void SetCuts()
    {
      if (!thePhysicsList) Construct();
      thePhysicsList->SetCuts();
    }
    void Construct()
    {
      if (!thePhysicsList) thePhysicsList=new T;
      if (cutValue>0) thePhysicsList->SetDefaultCutValue(cutValue);
      // cout<<"this is construct"<<endl;
      // thePhysicsList->Construct();
    }

  };

} // end namespace

#endif // PhysicsListEntryT_H
