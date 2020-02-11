/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Include files

// local
#include "ExtraParticlesPhysicsTool.h"
#include "CustomParticle.hh"

// Geant4 headers
#include "G4MuIonisation.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4Transportation.hh"
#include "G4Version.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : ExtraParticlesPhysicsTool
//
// August-2019 : Miha Muskinja
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExtraParticlesPhysicsTool::ExtraParticlesPhysicsTool(const std::string &type,
                                                     const std::string &name,
                                                     const IInterface *parent)
    : base_class(type, name, parent) {

    declareProperty("ExtraParticlesConfig", m_extraParticlesConfig);
}

//=============================================================================
// Destructor
//=============================================================================
ExtraParticlesPhysicsTool::~ExtraParticlesPhysicsTool() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode ExtraParticlesPhysicsTool::initialize() {
    ATH_MSG_DEBUG("initializing...");
    this->SetPhysicsName(this->name());
    return StatusCode::SUCCESS;
}

//=============================================================================
// GetPhysicsOption
//=============================================================================
ExtraParticlesPhysicsTool *ExtraParticlesPhysicsTool::GetPhysicsOption() {
    return this;
}

//=============================================================================
// ConstructParticle
//=============================================================================
void ExtraParticlesPhysicsTool::ConstructParticle() {
    ATH_MSG_DEBUG("ConstructParticle for the extra particles being constructed");

    // the existing particle table
    G4ParticleTable *theParticleTable = G4ParticleTable::GetParticleTable();

    for (const auto &particle : m_extraParticlesConfig) {

        G4String name = particle.first;
        G4double mass = particle.second[0];
        G4double width = particle.second[1];
        G4int charge = particle.second[2];
        G4int pdg = particle.second[3];
        G4double lifetime = particle.second[4];
        G4bool stable = false;

        // don't add if the particle already exists
        if (theParticleTable->FindParticle(pdg))
            continue;

        // printout
        ATH_MSG_DEBUG("Adding: " << name << " " << pdg << " " << charge << " "
                                 << mass << " " << width << " " << lifetime);

        // create the new particle
        m_extraParticles.insert(new CustomParticle(name, mass, width, charge,
                                                   pdg, stable, lifetime));
    }
}

//=============================================================================
// ConstructProcess
//=============================================================================
void ExtraParticlesPhysicsTool::ConstructProcess() {
    for (auto *particle : m_extraParticles) {
        if (particle->GetPDGCharge() != 0) {
            ATH_MSG_DEBUG("Adding EM processes for "
                          << particle->GetParticleName());
            G4ProcessManager *proc = particle->GetProcessManager();
            proc->AddProcess(new G4hMultipleScattering, -1, 1, 1);
            proc->AddProcess(new G4hIonisation, -1, 2, 2);
        }
    }
}
