/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestPrimaryGeneratorAction_hh
#define TestPrimaryGeneratorAction_hh

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

class G4ParticleGun;
class G4Event;

class TestPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    TestPrimaryGeneratorAction();
    void GeneratePrimaries(G4Event* anEvent);

private:
    G4ParticleGun particleGun;
};

#endif
