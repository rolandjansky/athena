/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DummyPhysicsList_h
#define DummyPhysicsList_h

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

namespace FADS {

  class DummyPhysicsList: public G4VUserPhysicsList
  {
  public:
    DummyPhysicsList():G4VUserPhysicsList() {}
    virtual ~DummyPhysicsList() {}

  public:
    // Construct particle and physics
    virtual void ConstructParticle() {}
    virtual void ConstructProcess() {}

    //
    virtual void SetCuts() {}
  };

} // end namespace

#endif
