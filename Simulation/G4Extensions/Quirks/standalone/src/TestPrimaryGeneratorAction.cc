/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"

#include "Quirks/TestPrimaryGeneratorAction.hh"

TestPrimaryGeneratorAction::TestPrimaryGeneratorAction() : particleGun(1) {
    particleGun.SetParticlePosition(G4ThreeVector());
}

void TestPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    G4int pdgid;
    G4double p1[3], p2[3];
    std::ifstream f;
    f.open("gen.txt");
    f >> pdgid;
    for (G4int i = 0; i < 3; i++) {
        f >> p1[i]; p1[i] *= MeV;
    }
    for (G4int i = 0; i < 3; i++) {
        f >> p2[i]; p2[i] *= MeV;
    }
    f.close();

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    G4ParticleDefinition* particle1 = particleTable->FindParticle(pdgid);
    particleGun.SetParticleDefinition(particle1);
    particleGun.SetParticleMomentum(G4ParticleMomentum(p1[0],p1[1],p1[2]));
    particleGun.GeneratePrimaryVertex(anEvent);

    G4ParticleDefinition* particle2 = particleTable->FindParticle(-pdgid);
    particleGun.SetParticleDefinition(particle2);
    particleGun.SetParticleMomentum(G4ParticleMomentum(p2[0],p2[1],p2[2]));
    particleGun.GeneratePrimaryVertex(anEvent);
}
