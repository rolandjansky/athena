/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Include files

// local
#include "G4EMProcessesPhysicsTool.h"

// Geant4 headers
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include <cmath>

//-----------------------------------------------------------------------------
// Implementation file for class : G4EMProcessesPhysicsTool
//
// August-2020 : Miha Muskinja
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4EMProcessesPhysicsTool::G4EMProcessesPhysicsTool(const std::string &type,
                                                   const std::string &name,
                                                   const IInterface *parent)
    : base_class(type, name, parent)
{
    declareProperty("ParticleList", m_particleList);
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode G4EMProcessesPhysicsTool::initialize()
{
    ATH_MSG_DEBUG("initializing...");
    this->SetPhysicsName(this->name());
    return StatusCode::SUCCESS;
}

//=============================================================================
// GetPhysicsOption
//=============================================================================
G4EMProcessesPhysicsTool *G4EMProcessesPhysicsTool::GetPhysicsOption()
{
    return this;
}

//=============================================================================
// ConstructParticle
//=============================================================================
void G4EMProcessesPhysicsTool::ConstructParticle() {}

//=============================================================================
// ConstructProcess
//=============================================================================
void G4EMProcessesPhysicsTool::ConstructProcess()
{
    G4ParticleTable::G4PTblDicIterator *particleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
    particleIterator->reset();

    while ((*particleIterator)())
    {
        G4ParticleDefinition *particle = particleIterator->value();
        if (std::find(m_particleList.begin(), m_particleList.end(), std::abs(particle->GetPDGEncoding())) != m_particleList.end())
        {
            ATH_MSG_INFO("Adding EM processes for "
                         << particle->GetParticleName());
            G4ProcessManager *proc = particle->GetProcessManager();
            proc->AddProcess(new G4hMultipleScattering, -1, 1, 1);
            proc->AddProcess(new G4hIonisation, -1, 2, 2);
        }
    } // End of the particle iterator
}
