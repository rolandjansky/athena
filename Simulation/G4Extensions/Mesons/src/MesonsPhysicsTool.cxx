/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Include files

// local
#include "MesonsPhysicsTool.h"
#include "CustomMeson.hh"

// Geant4 headers
#include "G4MuIonisation.hh"
#include "G4ProcessManager.hh"
#include "G4Transportation.hh"
#include "G4Version.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"

// CLHEP headers
#include "CLHEP/Units/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MesonsPhysicsTool
//
// 01-July-2019 : Miha Muskinja
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MesonsPhysicsTool::MesonsPhysicsTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent)
    : base_class(type, name, parent) {

    declareProperty("MesonsConfig", m_mesonsConfig);
}

//=============================================================================
// Destructor
//=============================================================================
MesonsPhysicsTool::~MesonsPhysicsTool() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MesonsPhysicsTool::initialize() {
    ATH_MSG_DEBUG("initializing...");
    this->SetPhysicsName(this->name());
    return StatusCode::SUCCESS;
}

//=============================================================================
// GetPhysicsOption
//=============================================================================
MesonsPhysicsTool *MesonsPhysicsTool::GetPhysicsOption() { return this; }

//=============================================================================
// ConstructParticle
//=============================================================================
void MesonsPhysicsTool::ConstructParticle() {
    ATH_MSG_DEBUG("ConstructParticle for the Mesons being run");

    for (const auto& meson : m_mesonsConfig) {
        /* Unpack the config:
           values must be in the same order as in
           the 'MesonsList.py' config file.
        */
        G4String name = meson.first;
        G4double mass = meson.second[0];
        G4double width = meson.second[1];
        G4int charge = meson.second[2];
        G4double spin = meson.second[3];
        G4int parity = meson.second[4];
        G4int isospin3 = meson.second[5];
        G4int PDG = meson.second[6];
        G4double lifetime = meson.second[7];
        G4String subType = "meson";
        if (name.find("D") == 0 || name.find("anti_D") == 0)
            subType = "D";
        else if (name.find("B") == 0 || name.find("anti_B") == 0)
            subType = "B";

        // debug printout
        ATH_MSG_DEBUG("Constructing meson " << name);
        ATH_MSG_DEBUG("  mass: " << mass);
        ATH_MSG_DEBUG("  width: " << width);
        ATH_MSG_DEBUG("  charge: " << charge);
        ATH_MSG_DEBUG("  spin: " << spin);
        ATH_MSG_DEBUG("  parity: " << parity);
        ATH_MSG_DEBUG("  isospin3: " << isospin3);
        ATH_MSG_DEBUG("  PDG: " << PDG);
        ATH_MSG_DEBUG("  lifetime: " << lifetime);
        ATH_MSG_DEBUG("  subType: " << subType);

        // create the new meson
        m_setOfMesons.insert(new CustomMeson(name, mass, width, charge, spin,
                                             parity, isospin3, PDG, lifetime,
                                             subType));
    }
}

//=============================================================================
// ConstructProcess
//=============================================================================
void MesonsPhysicsTool::ConstructProcess() {
    for (auto *meson : m_setOfMesons) {
        if (meson->GetPDGCharge() != 0) {
            ATH_MSG_DEBUG("Adding EM processes for "
                          << meson->GetParticleName());
            G4ProcessManager *proc = meson->GetProcessManager();
            proc->AddProcess(new G4hMultipleScattering, -1, 1, 1);
            proc->AddProcess(new G4hIonisation, -1, 2, 2);
        }
    }
}
