/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListSteering_H
#define PhysicsListSteering_H

#include "G4VUserPhysicsList.hh"
#include "G4ParticleTable.hh"

#include <iostream>
#include <vector>

class UserPhysicsDefinition;

namespace FADS {

  class FadsPhysicsList;
  class PhysicsListCatalog;

  //#include "FadsPhysics/PhysicsListCatalog.h"

  class PhysicsListSteering: public G4VUserPhysicsList {
    friend class PhysicsListCatalog;
  public:
    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();
    ~PhysicsListSteering();
    void SetNeutronTimeCut(double);
    double GetNeutronTimeCut();
    void SetNeutronEnergyCut(double);
    double GetNeutronEnergyCut();
    void SetApplyCuts(bool a) { m_applyCuts = a; }
    bool GetApplyCuts() const { return m_applyCuts; }
  private:
    PhysicsListSteering();
    FadsPhysicsList* m_theList;
    G4VUserPhysicsList* m_PList;
    std::vector<UserPhysicsDefinition*> m_userPhysics;

    double m_neutronTimeCut;
    double m_neutronEnergyCut;
    bool m_applyCuts;
  };

}

#endif
