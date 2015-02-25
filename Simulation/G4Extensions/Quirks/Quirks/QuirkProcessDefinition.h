/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUIRKPROCESSDEFINITION_H
#define QUIRKPROCESSDEFINITION_H

#ifdef QUIRKS_STANDALONE
#include "G4VUserPhysicsList.hh"
typedef G4VUserPhysicsList UserPhysicsDefinition;
#else
#include "FadsPhysics/UserPhysicsDefinition.h"
#endif

class QuirkProcessDefinition : public UserPhysicsDefinition {
public:
    QuirkProcessDefinition();
    void ConstructParticle();
    void ConstructProcess();
    void SetCuts();

private:
    G4double m_mass;
    G4double m_charge;
    G4int m_pdgid;
    G4double m_stringForce;
    G4double m_firstStringLength;
    G4double m_maxBoost;
    G4double m_maxMergeT;
    G4double m_maxMergeMag;
    G4int m_enableDebug;
    G4double m_debugStep;
    G4int m_numDebugSteps;
};

#endif
