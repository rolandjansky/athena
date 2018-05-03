/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTPhysicsTool.h"
#include "TRTTransitionRadiation.h"

#include "G4ProcessManager.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"
#include "G4Version.hh"
#include "G4AutoDelete.hh"

#include "globals.hh"
#include <iostream>

//-----------------------------------------------------------------------------
// Implementation file for class : TRTPhysicsTool
//
// 18-05-2015 Edoardo Farina
//-----------------------------------------------------------------------------

#if G4VERSION_NUMBER > 1009
#define PARTICLEITERATOR aParticleIterator
#else
#define PARTICLEITERATOR theParticleIterator
#endif


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TRTPhysicsTool::TRTPhysicsTool( const std::string& type,
                                const std::string& nam,const IInterface* parent )
  : G4VPhysicsConstructor(nam), base_class ( type, nam , parent )
{
  declareProperty("XMLFile", m_xmlFile="TRgeomodelgeometry.xml");
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TRTPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("TRTPhysicsTool initialize()");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}

//=============================================================================
// Return the physics constructor
//=============================================================================
G4VPhysicsConstructor* TRTPhysicsTool::GetPhysicsOption()
{
  // I am the physics constructor, so return self.
  return this;
}

//=============================================================================
// Particle construction; not implemented
//=============================================================================
void TRTPhysicsTool::ConstructParticle()
{
}

//=============================================================================
// Physics process construction
//=============================================================================
void TRTPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG("TRTPhysicsTool::ConstructProcess()");

  // Use the Geant4 garbage collection mechanism to clean this up.
  // It's just a convenient way to clean up in multi-threading.
  auto trProc = new TRTTransitionRadiation("XTR", m_xmlFile) ;
  G4AutoDelete::Register(trProc);

  PARTICLEITERATOR->reset();
  while( (*PARTICLEITERATOR)() ) {
    G4ParticleDefinition* particle = PARTICLEITERATOR->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if ( particle->GetPDGCharge() != 0.0 && particle->GetPDGMass() != 0.0 ) {
      trProc->SetVerboseLevel(1);
      ATH_MSG_DEBUG("TRT Process Added to "<<particle->GetParticleName());
      pmanager->AddDiscreteProcess(trProc);
    }
  }
}
