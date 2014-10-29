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
    G4VUserPhysicsList* thePhysicsList;
   public:

    G4VUserPhysicsList* GetPhysicsList() override final { return thePhysicsList; }

    FactoryMadeList(std::string n): FadsPhysicsList(n) , thePhysicsList(0) {}

    ~FactoryMadeList ()
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
      if (!thePhysicsList){
        G4PhysListFactory factory;
        if(GetName() != "" && factory.IsReferencePhysList(GetName())){
          thePhysicsList = factory.GetReferencePhysList(GetName());
        } 
        if (!thePhysicsList) throw "Physics list not found";
      }
      if (cutValue>0) thePhysicsList->SetDefaultCutValue(cutValue);
    }

  };

} // end namespace

#endif // FactoryMadeList_H
